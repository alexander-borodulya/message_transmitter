#include "server.h"

namespace atto {

Server::Server()
    : m_socket(UDPSocket::SocketType::Server)
{
    m_socket.bind(INADDR_ANY, atto::RecvUdpPort);
}

void Server::start()
{
    std::cout << "Server: start: ..." << std::endl;

    int clientCmdID = -1;
    int selectCount = 0;
    size_t sendMsgCount = 0;

    while (!m_terminate) {
        std::cout << "Server: start: select: checking " << selectCount++ << "..." << std::endl;

        int nready = m_socket.select();
        if (nready == -1) {
            std::cout << "Server: start: select: Skipped, error in Select: " << atto::strError(errno) << std::endl;
            continue;
        }
        if (nready == 0) {
            std::cout << "Server: start: select: Timeout occurred / Connection closed / Connection wasn't established" << std::endl;
        }

        if (m_socket.readyRecv()) {
            std::cout << "Server: recv_command: About to receive cmdID..." << std::endl;
            if (this->recv_command(clientCmdID)) {
                std::cout << "Server: recv_command: cmdID: " << clientCmdID << std::endl;
            }  else {
                std::cout << "Server: recv_command failed" << std::endl;
            }
        }

        if (m_socket.hasClient() && m_socket.readySend()) {
            std::cout << "Server: send_message: message number " << sendMsgCount << ", sending..." << std::endl;
            const auto msg = atto::MakeMessageDiv20(sendMsgCount);
            if (send_message(msg)) {
                std::cout << "Server: send_message: succeeded: " << atto::toStringCompact(msg) << std::endl;
            } else {
                std::cout << "Server: send_message: failed" << atto::toString(msg) << std::endl;
            }
            ++sendMsgCount;
        }

        atto::sleepMS(1000);
    }
}

}
