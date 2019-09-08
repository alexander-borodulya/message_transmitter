#pragma once

#include <arpa/inet.h>

#include <string>

namespace atto {
    const int RecvUdpPort = 8080;
    const int SendTcpPort = 9090;
    const int MaxAttempts = 8;

    std::string strError(int err_code);
    void sleep(int seconds);
    void sleepMS(int ms);
    timeval MakeSelectDelay(int s = 3, int us = 0);
    std::string toString(const sockaddr_in & addr);
}
