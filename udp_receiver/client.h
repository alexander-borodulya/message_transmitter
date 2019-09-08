#pragma once

#include <common/message_format.h>
#include <container.h>

#include <common/udp_socket.h>
#include <common/tcp_socket.h>

#include <mutex>
#include <condition_variable>

namespace atto {

    class Client {
    public:
        Client();
        void start(const std::string & th_name);
        void sendViaTcpSocket(const std::string &th_name);

    protected:
        static bool sendViaTcpRequired(const atto::MessageFormat & msg);
        bool sendCommand(int c);
        bool recvMessage(atto::MessageFormat & msg);
        void saveMessage(const atto::MessageFormat & msg);

    private:
        bool m_terminate = false;

        // m_container is a container that saves all the received messages by the cline upd socket.
        atto::container m_container;

        // m_containerTcpQueue is a container that saves all the messages that would be send via tcp socket.
        atto::container m_containerTcpQueue;

        // Udp socket that receives the incomming messages from the server.
        atto::UDPSocket m_udpSocket;

        // Tcp socket that the client is using for sending the messages that meets the criteria to be sent (i.e. Message id is equal to 10).
        atto::TCPSocket m_tcpSocket;

        // The thread that listens for the incoming TCP messages.
        std::thread m_tcpServer;

        std::mutex m_mutex_message;

        std::mutex m_mutex_tcp;
        std::condition_variable m_cv_tcp;

    };

    inline bool Client::sendViaTcpRequired(const MessageFormat &msg)
    {
        return msg.MessageId == atto::TCPResendID;
    }

    inline bool Client::sendCommand(int c)
    {
        bool was_sent = m_udpSocket.send_bytes(&c, atto::CommandSize);
        return was_sent;
    }

    inline bool Client::recvMessage(MessageFormat &msg)
    {
        bool was_received = m_udpSocket.recv_bytes(&msg, atto::MessageFormatSize);
        return was_received;
    }

    inline void Client::saveMessage(const MessageFormat &msg)
    {
        std::cout << "Cliend: saveMessage: " << atto::toStringCompact(msg) << std::endl;
        m_container.add(msg);
    }
}
