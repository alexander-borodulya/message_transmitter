#include "common.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string>
#include <string.h>
#include <thread>
#include <mutex>

namespace atto {

std::string strError(int err_code)
{
    bool isEAGAIN = err_code == EAGAIN;
    bool isEWOULDBLOCK = err_code == EWOULDBLOCK;
    std::string emsg = std::to_string(err_code) + ", " + strerror(err_code);
    if (isEAGAIN) {
        emsg += ", EAGAIN";
    }
    if (isEWOULDBLOCK) {
        emsg += ", EWOULDBLOCK";
    }
    return emsg;
}

void sleep(int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void sleepMS(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

timeval MakeSelectDelay(int s, int us)
{
    timeval t;
    t.tv_sec = s;
    t.tv_usec = us;
    return t;
}

// Determine who sent the data
std::string toString(const sockaddr_in & addr)
{
    hostent* hostp = gethostbyaddr(reinterpret_cast<const char *>(&addr.sin_addr.s_addr),
                                   sizeof(addr.sin_addr.s_addr),
                                   AF_INET);
    if (!hostp) {
        int e = errno;
        return "gethostbyaddr error: " + atto::strError(e);
    }

    const char* hostaddrp = inet_ntoa(addr.sin_addr);
    if (hostaddrp == nullptr) {
        int e = errno;
        return "inet_ntoa error: " + atto::strError(e);
    }

    std::string result;
    result += hostp->h_name;
    result += " (";
    result += hostaddrp;
    result += ":";
    result += std::to_string(addr.sin_port);
    result += ")";
    return  result;
}
}
