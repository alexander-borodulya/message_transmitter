#include <client.h>
#include <test_container_functional.h>
#include <test_threads_model.h>

static std::mutex exitMutex;
static std::condition_variable exitCondVar;

int main()
{
    // Thread names
    std::string th1Name = "upd_thread_1";
    std::string th2Name = "upd_thread_2";
    std::string th3Name = "tcp_thread_3";

    // Create client
    atto::Client client;

    // Create 3 threads, th1 and th2 are UDP receivers, th2 is TCP sender.
    std::thread th1(&atto::Client::start, &client, th1Name);
    std::thread th2(&atto::Client::start, &client, th2Name);
    std::thread th3(&atto::Client::sendViaTcpSocket, &client, th3Name);

    std::unique_lock<std::mutex> exitLock(exitMutex);
    exitCondVar.wait(exitLock);

    return 0;
}
