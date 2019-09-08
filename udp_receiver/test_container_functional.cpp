#include "test_container_functional.h"

#include <container.h>

#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <map>
#include <cstdint>
#include <cassert>

namespace atto {
namespace functional_test {

void testContainer_insert_contains()
{
    std::map<uint64_t, atto::MessageFormat> m;
    atto::container c;
    for (uint64_t i = 0; i < 10; ++i) {
        auto msg = atto::MakeMessageWithId(i);
        m.insert({i, msg});
        c.insert(i, msg);
    }

    std::cout << std::boolalpha;
    for (uint64_t i = 0; i < 10; ++i) {
        bool foundM = m.find(i) != m.end();
        bool foundC = c.contains(i);
        const auto msgM = m.at(i);
        const auto msgC = c.at(i);
        bool msgEqual = msgM == msgC;

        std::cout << "Key (" << i << "), found in std::map (" << foundM << "), found in atto::container (" << foundC << ")" << std::endl
                  << "Values are equal (" << msgEqual << "), " << atto::toStringCompact(msgM) << " == " << atto::toStringCompact(msgC) << std::endl << std::endl;

        assert(foundC);
        assert(msgEqual);
    }
    std::cout << std::noboolalpha;
}


void testConteiner_remove()
{
    atto::container c;

    for (uint64_t i = 0; i < 20; ++i) {
        uint64_t index = c.size();
        auto msg = atto::MakeMessageWithId(5);
        c.insert(index, msg);
    }

    for (uint64_t i = 0; i < 20; ++i) {
        if ((i % 2) == 0) {
            uint64_t index = c.beginIndex();
            auto msg = c.beginValue();
            c.remove(index);
            std::cout << "Removed: " << atto::toString(msg) << std::endl;
            std::cout << c.toString() << std::endl << std::endl;
        }
    }
}

void testContainer_threads()
{
    std::mutex mutex_cout;
    atto::container c;
    int count = 0;

    // The function that inserts messages into the container
    auto fillContainer = [&](const std::string & tag) {
        {
            std::lock_guard<std::mutex> lock(mutex_cout);
            std::cout << "[+] " << tag << std::endl;
        }

        for (uint64_t i = 0; i < 10000; ++i) {
            auto msg = atto::MakeMessageWithId(i);
            {
                std::lock_guard<std::mutex> lock(mutex_cout);
                std::cout << "    " << tag << ", about to insert " << i << ", " << atto::toStringCompact(msg) << std::endl;
            }

            c.insert(i, msg);

            {
                std::unique_lock<std::mutex> lock(mutex_cout);
                ++count;
            }
        }

        {
            std::lock_guard<std::mutex> lock(mutex_cout);
            std::cout << "[-] " << tag << std::endl;
        }
    };

    // Using std::threads...
    {
        std::thread th1(fillContainer, "th1");
        std::thread th2(fillContainer, "th2");
        th1.join();
        th2.join();
    }

    // Using lambdas...
    {
        std::vector<std::future<void>> futureVec;

        const int nlambdas = 10;
        for (int i = 0; i < nlambdas; ++i) {
            futureVec.push_back(
                std::async(
                    std::launch::async,
                    fillContainer,
                    "thread_" + std::to_string(i)));
        }

        for (auto & fv : futureVec) {
            fv.get();
        }

        std::cout << "Number of a message insertion attempts, " << count << std::endl;
        std::cout << "Number of messages in the container, " << c.size() << std::endl;
    }
}
}
}
