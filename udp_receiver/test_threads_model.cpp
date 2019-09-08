#include <common/log.h>
#include <client.h>
#include <container.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <vector>

namespace atto {
namespace functional_test {

class Demo {
public:
    Demo()
        : message(0)
    {
    }

    bool needSendViaTcp() const {
        return (message % 5) == 0;
    }

    void start(const std::string & th_name) {
        log::cout(th_name, "start");

        while (!m_terminate) {
            std::unique_lock<std::mutex> lock_message(mutex_message);

            // RECV
            message += 1;
            log::cout(th_name, "select + recv...");
            log::cout(th_name, "message", message);

            if ((message % 5) == 0) {

                log::cout(th_name, "needSendViaTcp, c_messages.size", c_msg_queue.size());

                std::unique_lock<std::mutex> lock_tcp(mutex_tcp);

                auto s = c_msg_queue.size();
                c_msg_queue.insert(s, atto::MakeMessageWithId(message));

                log::cout(th_name, "needSendViaTcp, c_messages.size()", c_msg_queue.size());
                cv_tcp.notify_one();
            }

            log::cout_endl();

            // atto::sleepMS(1000);
        }
    }

    void sendViaTCP(const std::string & th_name)  {
        log::cout(th_name, "sendViaTCP...");

        while (!m_terminate) {
            std::unique_lock<std::mutex> lock_tcp(mutex_tcp);
            cv_tcp.wait(lock_tcp, [&]{
                log::cout(th_name, "c_msg_queue.empty", c_msg_queue.empty());
                return !c_msg_queue.empty();
            });

            auto index = c_msg_queue.beginIndex();
            auto m = c_msg_queue.beginValue();
            c_msg_queue.remove(index);
            log::cout(th_name, "c_msg_queue.size", c_msg_queue.size());

            log::cout(th_name, "tcp msg", atto::toString(m));
            if ((m.MessageId % 5) != 0) {
                log::cout(th_name, "error, bad message", atto::toString(m));
            }

            log::cout_endl();
        }
    }

private:
    bool m_terminate = false;

    uint64_t message;
    atto::container c_msg_queue;

    std::mutex mutex_message;
    std::condition_variable cv_message;

    std::mutex mutex_tcp;
    std::condition_variable cv_tcp;

    std::mutex mutex_cout;
};

static std::mutex exitMutex;
static std::condition_variable exitCondVar;

void testThreadsModel()
{
    std::string th1Name = "upd_thread_1";
    std::string th2Name = "upd_thread_2";
    std::string th3Name = "tcp_thread_3";

    Demo d;

    std::thread th1(&Demo::start, &d, th1Name);
    std::thread th2(&Demo::start, &d, th2Name);
    std::thread th3(&Demo::sendViaTCP, &d, th3Name);

    atto::sleepMS(100);
    std::unique_lock<std::mutex> exitLock(exitMutex);
    exitCondVar.wait(exitLock);
}

}
}
