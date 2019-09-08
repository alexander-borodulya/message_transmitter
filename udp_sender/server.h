#pragma once

// ATTO
#include <common/message_format.h>
#include <common/udp_socket.h>

namespace atto {

    class Server {
    public:
        Server();
        void start();

    protected:
        bool recv_command(int & c);
        bool send_message(const atto::MessageFormat & msg);

    private:
        UDPSocket m_socket;
        bool m_terminate = false;
    };

    inline bool Server::recv_command(int &c)
    {
        bool was_received = m_socket.recv_bytes(&c, atto::CommandSize);
        return was_received;
    }

    inline bool Server::send_message(const MessageFormat &msg)
    {
        bool was_sent = m_socket.send_bytes(&msg, atto::MessageFormatSize);
        return was_sent;
    }

}
