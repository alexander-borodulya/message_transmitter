#include "tcp_socket.h"

// STD
#include <iostream>
#include <string.h>

// SYS
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

namespace atto {

TCPSocket::TCPSocket() {
    initSocket();
}

TCPSocket::~TCPSocket() {
    if (connected()) {
        FD_CLR(m_socket, &m_sendSocketSet);
        close(m_socket);
    }
}

void TCPSocket::connect(const std::string addr, uint16_t port) {
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    m_connected = ::connect(m_socket,
                            reinterpret_cast<struct sockaddr*>(&servaddr),
                            sizeof(servaddr)) == 0;
    if (!m_connected) {
        int e = errno;
        std::cout << "TCPSocket: connect: " << atto::strError(e) << std::endl;
        if (e == EINPROGRESS) {
            m_connected = true;
        }
    }
}

bool TCPSocket::sendBytes(const void *p, size_t s) {
    bool wasSent = false;
    wasSent = send(m_socket, p, s, 0) != -1;
    if (!wasSent) {
        int e = errno;
        std::cout << "TCPSocket: send: " << atto::strError(e) << std::endl;
    }
    return wasSent;
}

void TCPSocket::initSocket() {
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        int e = errno;
        std::cout << "TCPSocket: socket: " << atto::strError(e) << std::endl;
    }
    servaddr_len = sizeof(servaddr);
    memset(&servaddr, 0, servaddr_len);
}

}
