#include "momoko-log/logger.h"
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

std::atomic<long> count(0);

void ThreadProc(int index)
{
    while (true) {
        ++count;
        LOG_INFO << "thread index: " << index << ", count: " << count << ", 0123456789" << "abcdefghijklmnopqrstuvwxyz";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    LOG_SET_ASYNC(1)

    std::vector<std::thread> tds;
    for (int i = 0; i < 4; ++i) {
        tds.push_back(std::thread(ThreadProc, i));
    }

    for(size_t i = 0; i < 4; ++i) {
        tds[i].detach();
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
