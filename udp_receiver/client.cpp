#include <client.h>

#include <common/log.h>
#include <common/tcp_server.h>

#include <thread>

namespace atto {

Client::Client()
    : m_udpSocket(UDPSocket::SocketType::Client)
    , m_tcpServer(atto::tcp_server::start_)
{
    m_udpSocket.bind("127.0.0.1", atto::RecvUdpPort);
    m_tcpSocket.connect("127.0.0.1", atto::SendTcpPort);
}

void Client::start(const std::string &th_name) {
    log::cout(th_name, "Client: start: ...");

    // Sync command
    int cmd = 200;

    while (m_udpSocket.isOpen()) {
        // Protect message
        std::unique_lock<std::mutex> messageLock(m_mutex_message);

        log::cout(th_name, "Client: start: select...");
        int retval = m_udpSocket.select();

        if (retval == -1) {
            int e = errno;
            log::cout(th_name, "Client: start: select: Error in Select: ", atto::strError(e));
            continue;
        }

        if (retval == 0) {
            log::cout(th_name, "Client: start: select: Socket isn't ready");
            continue;
        }

        if (m_udpSocket.readySend()) {
            log::cout(th_name, "Client: start: send_command: About to send sync command");
            if (sendCommand(cmd)) {
                log::cout(th_name, "Client: start: send_command succeeded");
            } else {
                log::cout(th_name, "Client: start: send_command failed");
            }
        }

        if (!m_udpSocket.hasServer()) {
            log::cout(th_name, "Clien: start: Could find server...");
            continue;
        }

        if (m_udpSocket.readyRecv()) {
            atto::MessageFormat m;
            if (recvMessage(m)) {
                log::cout(th_name, "Client: start: recv_message succeeded: ", atto::toStringCompact(m));
                saveMessage(m);
                if (sendViaTcpRequired(m)) {
                    std::unique_lock<std::mutex> lock_tcp(m_mutex_tcp);
                    log::cout(th_name, "Client: start: About to send via TCP...");
                    auto tcpQueueSize = m_containerTcpQueue.size();
                    m_containerTcpQueue.insert(tcpQueueSize, m);
                    m_cv_tcp.notify_one();
                }
            } else {
                log::cout(th_name, "Client: start: recv_message failed");
            }
        }

        log::cout_endl();
        atto::sleepMS(200);
    }
}

void Client::sendViaTcpSocket(const std::string & th_name)
{
    const std::string logPrefix = th_name + ": sendViaTcpSocket";
    log::cout(logPrefix, "...");

    while (!m_terminate) {
        std::unique_lock<std::mutex> lock_tcp(m_mutex_tcp);
        m_cv_tcp.wait(lock_tcp, [&] {
            return !m_containerTcpQueue.empty();
        });

        auto index = m_containerTcpQueue.beginIndex();
        auto msg = m_containerTcpQueue.beginValue();
        m_containerTcpQueue.remove(index);

        log::cout(logPrefix, "queue size", m_containerTcpQueue.size());
        if ((msg.MessageId % atto::TCPResendID) != 0) {
            log::cout(logPrefix, "error, bad message id", atto::toString(msg));
        }

        log::cout(logPrefix, "sending message", atto::toStringCompact(msg));

        bool wasSent = false;
        if (m_tcpSocket.connected()) {
            wasSent = m_tcpSocket.sendBytes(&msg, atto::MessageFormatSize);
        } else {
            log::cout(logPrefix, "TCPSocket isn't connected, sending skipped");
        }

        log::cout_endl();
    }
}



}
