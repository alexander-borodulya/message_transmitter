#pragma once

#include <mutex>
#include <iostream>

namespace atto {

    namespace log {

        std::mutex & LogMutex();

        template<typename T>
        void cout(const std::string & msg1, const std::string & msg2, const T & v) {
            std::lock_guard<std::mutex> lock_cout(LogMutex());
            std::cout << msg1 << ": " << msg2 << ": " << v << std::endl;
        }

        void cout(const std::string & msg1);
        void cout(const std::string & msg1, const std::string & msg2);
        void cout_endl();
    }
}
