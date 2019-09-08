#pragma once

// ATTO
#include <common/common.h>
#include <common/message_format.h>

namespace atto {

    class TCPSocket {
    public:
        TCPSocket();
        ~TCPSocket();

        bool isValid() const;
        bool connected() const;
        void connect(const std::string addr, uint16_t port);
        bool sendBytes(const void* p, size_t s);

    protected:
        void initSocket();

    private:
        int m_socket = -1;
        sockaddr_in servaddr;
        socklen_t servaddr_len;
        bool m_connected = false;
        fd_set m_sendSocketSet;
    };

    inline bool TCPSocket::isValid() const {
        return m_socket > 0;
    }

    inline bool TCPSocket::connected() const {
        return m_connected;
    }
}

