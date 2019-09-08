#include "tcp_server.h"

#include <common/common.h>
#include <common/message_format.h>
#include <iostream>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

namespace atto {
namespace tcp_server {

static bool srvTerminate = false;

void start() {
    unsigned short port = atto::SendTcpPort; /* port server binds to                */
    struct sockaddr_in client;               /* client address information          */
    struct sockaddr_in server;               /* server address information          */
    int s;                                   /* socket for accepting connections    */
    int ns;                                  /* socket connected to client          */
    socklen_t namelen;                       /* length of client name               */
    fd_set m_recvSocketSet;

    // TCP socket for accepting connections.
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
        return;
    }

    // Bind the socket to the server address.
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, reinterpret_cast<sockaddr *>(&server), sizeof(server)) < 0)
    {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
        return;
    }

    // Listen for connections. Specify the backlog as 1.
    if (listen(s, 1) != 0)
    {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
    }

    // Accept a connection.
    namelen = sizeof(client);
    if ((ns = accept(s,
                     reinterpret_cast<sockaddr*>(&client),
                     &namelen)) == -1) {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
    }

    std::cout << "TCPServer: Waiting for messages via TCP..." << std::endl;
    while (!srvTerminate) {
        auto tv = atto::MakeSelectDelay();
        FD_ZERO(&m_recvSocketSet);
        FD_SET(ns, &m_recvSocketSet);
        int nready = ::select(ns + 1,
                              &m_recvSocketSet,
                              nullptr,
                              nullptr,
                              &tv);
        if (nready == 0 || nready == -1) {
            continue;
        }

        if (FD_ISSET(ns, &m_recvSocketSet)) {
            atto::MessageFormat msg;
            auto tcp_bytes_recv = recvfrom(ns,
                                           &msg, atto::MessageFormatSize,
                                           0,
                                           reinterpret_cast<sockaddr *>(&client),
                                           &namelen);
            std::cout << "TCPServer: Received message: " << tcp_bytes_recv << " " << atto::toString(msg) << std::endl;
        }
    }

    close(ns);
    close(s);
}

void start_() {
    unsigned short port = atto::SendTcpPort; /* port server binds to                */
    struct sockaddr_in client;               /* client address information          */
    struct sockaddr_in server;               /* server address information          */
    int s;                                   /* socket for accepting connections    */
    int ns;                                  /* socket connected to client          */
    socklen_t namelen;                       /* length of client name               */

    // TCP socket for accepting connections.
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
        return;
    }

    // Bind the socket to the server address.
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, reinterpret_cast<sockaddr *>(&server), sizeof(server)) < 0)
    {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
        return;
    }

    // Listen for connections. Specify the backlog as 1.
    if (listen(s, 1) != 0)
    {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
        return;
    }

    // Accept a connection.
    namelen = sizeof(client);
    if ((ns = accept(s,
                     reinterpret_cast<sockaddr*>(&client),
                     &namelen)) == -1) {
        int e = errno;
        std::cout << atto::strError(e) << std::endl;
        return;
    }

    std::cout << "TCPServer: Waiting for messages via TCP..." << std::endl;
    while (!srvTerminate) {
        atto::MessageFormat msg;
        if (recv(ns, &msg, atto::MessageFormatSize, 0) == -1) {
            int e = errno;
            std::cout << "TCPServer: Stoped with error: " << atto::strError(e) << std::endl;
            continue;
        }
        std::cout << "TCPServer: Received message: " << atto::toString(msg) << std::endl;
    }

    close(ns);
    close(s);
}

}
}
