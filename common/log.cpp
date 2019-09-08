#include "log.h"

namespace atto {
namespace log {

static std::mutex mutex_cout;

std::mutex & LogMutex() {
    return mutex_cout;
}

void cout(const std::string & msg1) {
    std::lock_guard<std::mutex> lock_cout(LogMutex());
    std::cout << msg1 << std::endl;
}

void cout(const std::string & msg1, const std::string & msg2) {
    std::lock_guard<std::mutex> lock_cout(LogMutex());
    std::cout << msg1 << ": " << msg2 << std::endl;
}

void cout_endl() {
    std::lock_guard<std::mutex> lock_cout(LogMutex());
    std::cout << std::endl;
}

}
}
