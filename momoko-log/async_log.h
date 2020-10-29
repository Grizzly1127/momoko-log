#ifndef MOMOKOLOG_ASYNCLOG_H
#define MOMOKOLOG_ASYNCLOG_H

#include "momoko-log/log_stream.h"
#include "momoko-log/noncopyable.h"

#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>

namespace momoko
{

class AsyncLog : noncopyable
{
    typedef LogBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

public:
    AsyncLog(int flush_interval = 500, int roll_size = 20 * 1024 * 1024);
    ~AsyncLog() {
        if (running_) {
            stop();
        }
    }

    void append(const char* buf, int len);

    void stop() {
        running_ = false;
        thread_.join();
    }

private:
    void writeThread();

    const int flush_interval_;
    const int roll_size_;
    std::atomic<bool> running_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    BufferPtr current_buffer_;
    BufferPtr next_buffer_;
    BufferVector buffers_;
};

} // namespace momoko

#endif // MOMOKOLOG_ASYNCLOG_H
