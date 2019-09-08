#pragma once

// ATTO
#include <message_format.h>
#include <common.h>
#include <types.h>

// STD
#include <iostream>
#include <thread>

// SYS
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

namespace atto {

class UDPServerSocket {
public:
    UDPServerSocket() {
        m_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_socket < 0) {
            int e = errno;
            std::cout << "UDPServerSocket: socket: " << atto::strError(e) << std::endl;
        }

        // Make the socket non-blocking
        int socketFlags = fcntl(m_socket, F_GETFL, 0);
        socketFlags = fcntl(m_socket, F_SETFL, socketFlags | O_NONBLOCK);
        m_blockingType = BlockingType::SocketNonBlocking;
        std::cout << "UDPServerSocket: fcntl: " << socketFlags << std::endl;

        // Init and filling server information
        servaddr_len = sizeof (servaddr);
        memset(&servaddr, 0, servaddr_len);

        servaddr.sin_family = AF_INET; // IPv4
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(atto::Port);

        // Init client information
        cliaddr_len = sizeof (cliaddr);
        memset(&cliaddr, 0, cliaddr_len);

        // Bind the socket with the server address
        m_bindStatus = bind_socket();
    }

    bool bind_socket() {
        bool bind_success = bind(m_socket,
                                 (struct sockaddr*)&servaddr,
                                 sizeof (servaddr)) == 0;
        if (!bind_success) {
            int e = errno;
            std::cout << "UDPServerSocket: bind failed: " << atto::strError(e) << std::endl;
        }
        return bind_success;
    }

    ~UDPServerSocket() {
        shutdown();
    }

    void start() {
        std::cout << "Server: start: ..." << std::endl;

        auto preSelectInit = [this] () {
            FD_ZERO(&m_recvSocketSet);
            FD_ZERO(&m_sendSocketSet);
            FD_ZERO(&m_exceSocketSet);
            FD_SET(m_socket, &m_recvSocketSet);
            FD_SET(m_socket, &m_sendSocketSet);
            FD_SET(m_socket, &m_exceSocketSet);
        };

        {

            int cmdID = -1;
            size_t msgCount = 0;
            int selectCount = 0;

            while (true) {
                std::cout << "Server: start: select: checking " << selectCount++ << "..." << std::endl;

                preSelectInit();

                timeval selectDelay = MakeSelectDelay();
                int nready = select(m_socket + 1,
                                    &m_recvSocketSet,
                                    &m_sendSocketSet,
                                    nullptr, //&m_exceSocketSet,
                                    &selectDelay
                                    );

                if (nready == -1) {
                    std::cout << "Server: start: select: Error in Select: " << atto::strError(errno) << std::endl;
                    continue;
                }

                if (nready == 0) {
                    std::cout << "Server: start: select: Timeout occurred / Connection closed / Connection wasn't established" << std::endl;
                }

                if (FD_ISSET(m_socket, &m_recvSocketSet)) {
                    std::cout << "Server: recv_command: About to receive cmdID..." << std::endl;
                    if (this->recv_command(cmdID)) {
                        std::cout << "Server: recv_command: cmdID: " << cmdID << std::endl;
                    }  else {
                        std::cout << "Server: recv_command failed" << std::endl;
                    }
                }

                if (hasClient() && FD_ISSET(m_socket, &m_sendSocketSet)) {
                    std::cout << "Server: send_message: message number " << msgCount << ", sending..." << std::endl;
                    const auto msg = atto::MakeMessage(msgCount);
                    int attempts = 1;
                    if (this->send_message(msg)) {
                        std::cout << "Server: send_message: succeeded: " << atto::toStringShort(msg) << std::endl;
                    } else {
                        std::cout << "Server: send_message: failed" << atto::toString(msg) << std::endl;
                        ++attempts;
                    }
                    ++msgCount;
                }

                atto::sleepMS(1000);
            }
        }
    }


    void startBlocking() {
        if (!isOpen()) {
            std::cout << "Server: start: failed to start server" << std::endl;
            return;
        }

        int c = -1;
        std::cout << "Server: start: waiting for the receiver..." << std::endl;
        if (recv_command(c)) {
            std::cout << "Server: start: recv_command: succeeded: " << c << std::endl;
        } else {
            std::cout << "Server: start: recv_command: failed" << std::endl;
        }

        size_t msgCount = 0;
        while (isOpen()) {
            uint64_t id = msgCount % 10;
            uint64_t data = msgCount;
            atto::MessageFormat msg(0, 0, id, data);
            std::cout << "Server: send_message: sending..." << std::endl;
            if (send_message(msg)) {
                std::cout << "Server: send_message: succeeded: " << atto::toStringShort(msg) << std::endl;
            } else {
                std::cout << "Server: send_message: failed" << atto::toString(msg) << std::endl;
            }
            ++msgCount;

//            atto::sleep(1);
        }
    }

    void shutdown() {
        int close_status = close(m_socket);
        if (close_status < 0) {
            std::cout << "TcpSocket: Socket close failed" << std::endl;
        }
        FD_CLR(m_socket, &m_recvSocketSet);
        FD_CLR(m_socket, &m_sendSocketSet);
        FD_CLR(m_socket, &m_exceSocketSet);
        m_socket = 0;
        m_bindStatus = false;
    }

    bool isOpen() const {
        return m_socket > 0 && m_bindStatus;
    }

    bool isNonBlocking() const {
        return m_blockingType == BlockingType::SocketNonBlocking;
    }

private:

    bool recv_command(int & c) {
        bool was_received = recv_bytes(&c, sizeof (c));
        return was_received;
    }

    bool send_command(int c) {
        bool was_sent = send_bytes(&c, sizeof (c));
        return was_sent;
    }

    bool recv_message(atto::MessageFormat & msg) {
        bool was_received = recv_bytes(&msg, atto::MessageFormatSize);
        return was_received;
    }

    bool send_message(const atto::MessageFormat & msg) {
        bool was_sent = send_bytes(&msg, atto::MessageFormatSize);
        return was_sent;
    }

    bool send_bytes(const void* p, size_t s) {
        bool was_sent = true;
        int sendFlags = 0;

        ssize_t n = sendto(m_socket,
                           p, s,
                           sendFlags,
                           reinterpret_cast<sockaddr*>(&cliaddr),
                           sizeof(cliaddr));
        if (n < 0) {
            int e = errno;
            std::cout << "UDPServerSocket: send_bytes: Error: " << atto::strError(e) << std::endl;
            was_sent = false;
        }
        return was_sent;
    }

    bool recv_bytes(void* p, size_t s) {
        bool was_received = true;
        bool nonBlocking = isNonBlocking();
        int f = nonBlocking ? 0 : MSG_WAITALL;
        ssize_t n = recvfrom(m_socket,
                             p, s,
                             f,
                             reinterpret_cast<sockaddr*>(&cliaddr),
                             &cliaddr_len);

        const std::string cliAddrStr = toPrettyString(cliaddr);
        std::cout << "UDPServerSocket: recv_bytes: source: " << cliAddrStr << std::endl;

        if (n < 0) {
            int e = errno;
            if (nonBlocking) {
                std::cout << "UDPServerSocket: recv_bytes: No incoming data: " << atto::strError(e) << std::endl;
            } else {
                std::cout << "UDPServerSocket: recv_bytes: Error: " << atto::strError(e) << std::endl;
            }
            was_received = false;
        }

        return was_received;
    }

    bool hasClient() {
        return isValid(cliaddr);
    }

    bool isValid(const sockaddr_in & addr_in) {
        return addr_in.sin_addr.s_addr && addr_in.sin_family && addr_in.sin_port;
    }

    // Determine who sent the data
    std::string toPrettyString(const sockaddr_in & addr) {
        hostent* hostp = gethostbyaddr((const char *)&addr.sin_addr.s_addr,
                                       sizeof(addr.sin_addr.s_addr),
                                       AF_INET);
        if (!hostp) {
            return "ERROR on gethostbyaddr";
        }

        const char* hostaddrp = inet_ntoa(addr.sin_addr);
        if (hostaddrp == nullptr) {
            return "ERROR on inet_ntoa";
        }

        std::string result;
        result += hostp->h_name;
        result += " (";
        result += hostaddrp;
        result += ":";
        result += std::to_string(addr.sin_port);
        result += ")";
        return  result;
    }

private:
    BlockingType m_blockingType;
    int m_socket = -1;
    fd_set m_recvSocketSet;
    fd_set m_sendSocketSet;
    fd_set m_exceSocketSet;
    bool m_bindStatus = true;

    sockaddr_in servaddr;
    socklen_t servaddr_len;

    sockaddr_in cliaddr;
    socklen_t cliaddr_len;
};

}
