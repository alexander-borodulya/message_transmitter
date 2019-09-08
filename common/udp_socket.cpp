#include "udp_socket.h"

// ATTO
#include <common/message_format.h>
#include <common/common.h>

// STD
#include <iostream>
#include <thread>

// SYS
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>

namespace atto {

UDPSocket::UDPSocket(UDPSocket::SocketType type)
    : m_socketType(type)
{
    init_socket();
}

UDPSocket::~UDPSocket()
{
    shutdown();
}

void UDPSocket::bind(const in_addr_t &address, uint16_t port)
{
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = address;
    servaddr.sin_port = htons(port);
    if (m_socketType == SocketType::Server) {
        m_bindStatus = bindServerSocket();
    }
}

void UDPSocket::bind(const std::string &address, uint16_t port)
{
    const in_addr_t inAddr = inet_addr(address.c_str());
    bind(inAddr, port);
}

void UDPSocket::init_socket()
{
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        int e = errno;
        std::cout << "UDPSocket: socket: " << atto::strError(e) << std::endl;
    }

    int socketFlags = fcntl(m_socket, F_GETFL, 0);
    socketFlags = fcntl(m_socket, F_SETFL, socketFlags | O_NONBLOCK);

    servaddr_len = sizeof (servaddr);
    memset(&servaddr, 0, servaddr_len);

    cliaddr_len = sizeof (cliaddr);
    memset(&cliaddr, 0, cliaddr_len);
}

bool UDPSocket::bindServerSocket()
{
    bool bind_success = ::bind(m_socket,
                               reinterpret_cast<const sockaddr*>(&servaddr),
                               sizeof (servaddr)) == 0;
    if (!bind_success) {
        int e = errno;
        std::cout << "UDPSocket: bind failed: " << atto::strError(e) << std::endl;
    }
    return bind_success;
}

void UDPSocket::preSelect()
{
    FD_ZERO(&m_recvSocketSet);
    FD_ZERO(&m_sendSocketSet);
    FD_ZERO(&m_exceSocketSet);
    FD_SET(m_socket, &m_recvSocketSet);
    FD_SET(m_socket, &m_sendSocketSet);
    FD_SET(m_socket, &m_exceSocketSet);
}

int UDPSocket::select()
{
    preSelect();
    timeval selectDelay = MakeSelectDelay();
    int nready = ::select(m_socket + 1,
                          &m_recvSocketSet,
                          &m_sendSocketSet,
                          nullptr,
                          &selectDelay);
    return nready;
}

bool UDPSocket::readyRecv()
{
    bool ready = FD_ISSET(m_socket, &m_recvSocketSet);
    return ready;
}

bool UDPSocket::readySend()
{
    bool ready = FD_ISSET(m_socket, &m_sendSocketSet);
    return ready;
}

bool UDPSocket::send_bytes(const void *p, size_t s)
{
    ssize_t n = -1;
    std::string addrStr;
    bool was_sent = true;
    if (isServer()) {
        n = sendto(m_socket,
                   p, s,
                   m_sendFlags,
                   reinterpret_cast<const sockaddr*>(&cliaddr),
                   sizeof(cliaddr));
        addrStr = atto::toString(cliaddr);
    } else {
        n = sendto(m_socket,
                   p, s,
                   m_sendFlags,
                   reinterpret_cast<const sockaddr*>(&servaddr),
                   sizeof(servaddr));
        addrStr = atto::toString(servaddr);
    }

    std::cout << "UDPSocket: send_bytes: destination: " << addrStr << std::endl;

    if (n < 0) {
        int e = errno;
        std::cout << "UDPSocket: send_bytes: Error: " << atto::strError(e) << std::endl;
        was_sent = false;
    }
    return was_sent;
}

bool UDPSocket::recv_bytes(void *p, size_t s)
{
    bool was_received = true;

    ssize_t n = -1;
    std::string addrStr;
    if (isServer()) {
        n = recvfrom(m_socket,
                     p, s,
                     m_recvFlags,
                     reinterpret_cast<sockaddr*>(&cliaddr),
                     &cliaddr_len);
        addrStr = atto::toString(cliaddr);
    } else {
        n = recvfrom(m_socket,
                     p, s,
                     m_recvFlags,
                     reinterpret_cast<sockaddr*>(&servaddr),
                     &servaddr_len);
        addrStr = atto::toString(servaddr);
    }

    std::cout << "UDPSocket: recv_bytes: source: " << addrStr << std::endl;

    if (n < 0) {
        int e = errno;
        std::cout << "UDPSocket: recv_bytes: Error: " << atto::strError(e) << std::endl;
        was_received = false;
    }

    return was_received;
}

void UDPSocket::shutdown()
{
    int close_status = close(m_socket);
    if (close_status < 0) {
        std::cout << "UDPSocket: Socket close failed" << std::endl;
    }
    FD_CLR(m_socket, &m_recvSocketSet);
    FD_CLR(m_socket, &m_sendSocketSet);
    FD_CLR(m_socket, &m_exceSocketSet);
    m_socket = 0;
    m_bindStatus = false;
}

bool UDPSocket::recv_command(int &c)
{
    bool was_received = recv_bytes(&c, sizeof (c));
    return was_received;
}





}
