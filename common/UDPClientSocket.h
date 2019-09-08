#pragma once

// ATTO
#include <message_format.h>
#include <common.h>
#include <types.h>
#include <UDPSocket.h>

// STD
#include <iostream>

// SYS
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace atto {

class UDPClientSocket {
private:
    BlockingType m_blockingType;
    int m_socket;
    fd_set m_recvSocketSet;
    fd_set m_sendSocketSet;
    fd_set m_exceSocketSet;
    sockaddr_in servaddr;
    socklen_t servaddr_len;

public:

    UDPClientSocket() {
        m_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_socket < 0) {
            int e = errno;
            std::cout << "socket: " << atto::strError(e) << std::endl;
        }

        // Make the socket non-blocking
        int socketFlags = fcntl(m_socket, F_GETFL, 0);
        if (fcntl(m_socket, F_SETFL, socketFlags | O_NONBLOCK) != 0) {
            int e = errno;
            std::cout << "UDPClientSocket: fcntl: Error: " << atto::strError(e) << std::endl;
        }
        std::cout << "UDPServerSocket: fcntl: " << socketFlags << std::endl;

        memset(&servaddr, 0, sizeof(servaddr));

        // Filling server information
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(atto::RecvUdpPort);

        servaddr_len = sizeof(servaddr);
    }

    bool recv_command(int & c) {
        bool was_received = true;
        int recvFlags = MSG_WAITALL; recvFlags = 0;
        ssize_t n = recvfrom(m_socket,
                             &c, sizeof(c),
                             recvFlags,
                             reinterpret_cast<struct sockaddr*>(&servaddr), &servaddr_len);
        if (n < 0) {
            int e = errno;
            std::cout << "UDPClientSocket: recv_command: Error: " << atto::strError(e) << std::endl;
            was_received = false;
        }
        return was_received;
    }

    bool recv_message(atto::MessageFormat & msg) {
        bool was_received = true;
        int recvFlags = 0;
        ssize_t n = recvfrom(m_socket,
                             &msg, atto::MessageFormatSize,
                             recvFlags,
                             (struct sockaddr*)&servaddr, &servaddr_len);
        if (n < 0) {
            int e = errno;
            std::cout << "UDPClientSocket: recv_command: Error: " << atto::strError(e) << std::endl;
            was_received = false;
        }
        return was_received;
    }

    bool send_command(int c) {
        bool was_send = true;
        ssize_t n = sendto(m_socket,
                           &c, sizeof (c),
                           0,
                           reinterpret_cast<const sockaddr*>(&servaddr), servaddr_len);
        if (n < 0) {
            int e = errno;
            std::cout << "UDPClientSocket: send_command: Error: " << atto::strError(e) << std::endl;
            was_send = false;
        }
        return was_send;
    }

    void start () {
        std::cout << "Client: start: ..." << std::endl;

        auto preSelectInit = [this] () {
            FD_ZERO(&m_recvSocketSet);
            FD_ZERO(&m_sendSocketSet);
            FD_ZERO(&m_exceSocketSet);
            FD_SET(m_socket, &m_recvSocketSet);
            FD_SET(m_socket, &m_sendSocketSet);
            FD_SET(m_socket, &m_exceSocketSet);
        };




        int cmd = 200;
        int sendCount = 0;
        int selectCount = 0;

        while (true) {
            std::cout << "Server: start: select: checking " << selectCount++ << "..." << std::endl;

            preSelectInit();

            timeval tv = MakeSelectDelay();

            int retval = select(m_socket + 1,
                                &m_recvSocketSet,
                                &m_sendSocketSet,
                                nullptr,
                                &tv);

            if (retval == -1) {
                std::cout << "Client: start: select: Error in Select: " << atto::strError(errno) << std::endl;
                continue;
            }

            if (retval == 0) {
                std::cout << "Client: start: select: Timeout occurred/Connection closed" << std::endl;
            }

            if (FD_ISSET(m_socket, &m_sendSocketSet)) {
                std::cout << "Client: start: send_command: About to send command, " << cmd << ", sendCount " << sendCount << std::endl;
                if (send_command(cmd)) {
                    std::cout << "Client: start: send_command succeeded: cmd: " << cmd << ", sendCount " << sendCount << std::endl;
                    ++sendCount;
                } else {
                    std::cout << "Client: start: send_command failed" << std::endl;
                }
            }

            if (FD_ISSET(m_socket, &m_recvSocketSet)) {
                atto::MessageFormat m;
                if (recv_message(m)) {
                    std::cout << "Client: start: recv_message succeeded: " << atto::toString(m) << std::endl;
                } else {
                    std::cout << "Client: start: recv_message failed" << std::endl;
                }
            }

            atto::sleepMS(250);
        }
    }

    void start_blocking() {
        int c = 0;

        while (true) {
            if (send_command(c)) {
                std::cout << "Client: start: send_command: succeeded: " << c << std::endl;
                break;
            } else {
                std::cout << "Client: start: send_command: failed: " << c << std::endl;
                std::cout << "Client: start: sending again..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                continue;
            }
        }

        while (isOpen()) {
            atto::MessageFormat m;
            if (recv_message(m)) {
                std::cout << "Client: start: recv_message: succeeded: " << atto::toString(m) << std::endl;
            } else {
                std::cout << "Client: start: recv_message: failed" << std::endl;
            }
            atto::sleep(1);
        }
    }

    void shutdown() {
        int close_status = close(m_socket);
        if (close_status < 0) {
            std::cout << "UDPClientSocket: Socket close failed" << std::endl;
        }
        m_socket = 0;
    }

    bool isOpen() const {
        return m_socket > 0;
    }

    ~UDPClientSocket() {
        shutdown();
    }



}
