#pragma once

// ATTO
#include "message_format.h"
#include "common.h"

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

class UDPSocket {
public:
    enum class SocketType {
        Client,
        Server
    };

    UDPSocket(SocketType type);
    ~UDPSocket();

    void bind(const in_addr_t & address, uint16_t port);
    void bind(const std::string & address, uint16_t port);

    void init_socket();
    bool bindServerSocket();

    void preSelect ();
    int select();

    bool readyRecv();
    bool readySend();
    bool send_bytes(const void* p, size_t s);
    bool recv_bytes(void* p, size_t s);
    void shutdown();

    bool isOpen() const;
    bool hasClient() const;
    bool hasServer() const;
    bool isServer() const;

private:
    bool recv_command(int & c);
    bool send_command(int c);
    bool recv_message(atto::MessageFormat & msg);
    bool send_message(const atto::MessageFormat & msg);
    bool isValid(const sockaddr_in & addr_in) const;

private:
    SocketType m_socketType;

    int m_socket = -1;
    int m_sendFlags = 0; // MSG_CONFIRM
    int m_recvFlags = 0; // MSG_WAITALL;
    fd_set m_recvSocketSet;
    fd_set m_sendSocketSet;
    fd_set m_exceSocketSet;
    bool m_bindStatus = true;

    sockaddr_in servaddr;
    socklen_t servaddr_len;

    sockaddr_in cliaddr;
    socklen_t cliaddr_len;
};

inline bool UDPSocket::isOpen() const
{
    return m_socket > 0 && (isServer() ? m_bindStatus : true);
}

inline bool UDPSocket::hasClient() const
{
    return isValid(cliaddr);
}

inline bool UDPSocket::hasServer() const
{
    return isValid(servaddr);
}

inline bool UDPSocket::isServer() const
{
    return m_socketType == SocketType::Server;
}

inline bool UDPSocket::send_command(int c)
{
    bool was_sent = send_bytes(&c, sizeof (c));
    return was_sent;
}

inline bool UDPSocket::recv_message(MessageFormat &msg)
{
    bool was_received = recv_bytes(&msg, atto::MessageFormatSize);
    return was_received;
}

inline bool UDPSocket::send_message(const MessageFormat &msg)
{
    bool was_sent = send_bytes(&msg, atto::MessageFormatSize);
    return was_sent;
}

inline bool UDPSocket::isValid(const sockaddr_in &addr_in) const
{
    return addr_in.sin_addr.s_addr
            && addr_in.sin_family
            && addr_in.sin_port;
}

}
