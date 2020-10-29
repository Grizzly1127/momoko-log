#include "momoko-log/async_log.h"

#include "momoko-log/log_file.h"
#include <iostream>
#include <unistd.h>
#include <assert.h>

namespace momoko
{

AsyncLog::AsyncLog(int flush_interval, int roll_size)
    : flush_interval_(flush_interval),
      roll_size_(roll_size),
      running_(true),
      thread_(std::bind(&AsyncLog::writeThread, this), "AsyncLogginThread"),
      current_buffer_(new Buffer),
      next_buffer_(new Buffer),
      buffers_()
{
    current_buffer_->bzero();
    next_buffer_->bzero();
    buffers_.reserve(8);
}

void AsyncLog::append(const char* buf, int len)
{
    std::unique_lock<std::mutex> guard(mutex_);
    if (current_buffer_->avail() > len)
    {
        current_buffer_->append(buf, len);
    }
    else
    {
        buffers_.push_back(std::move(current_buffer_));
        if (next_buffer_)
        {
            current_buffer_ = std::move(next_buffer_);
        }
        else
        {
            // 如果写入线程还没处理完，并且 current 和 next buffer都满了的话，则new一个 current 队列
            current_buffer_.reset(new Buffer);
        }
        current_buffer_->append(buf, len);
        cond_.notify_one();
    }
}

void AsyncLog::writeThread()
{
    BufferPtr new_buffer1(new Buffer);
    BufferPtr new_buffer2(new Buffer);
    new_buffer1->bzero();
    new_buffer2->bzero();
    BufferVector buffers_to_write;
    buffers_to_write.reserve(8);
    LogFile output(roll_size_);

    while (running_)
    {
        {
            std::unique_lock<std::mutex> guard(mutex_);
            cond_.wait_for(guard, std::chrono::milliseconds(flush_interval_));

            buffers_.push_back(std::move(current_buffer_));
            current_buffer_ = std::move(new_buffer1);
            buffers_to_write.swap(buffers_);
            if (!next_buffer_)
            {
                next_buffer_ = std::move(new_buffer2);
            }
        }

        assert(!buffers_to_write.empty());

        if (buffers_to_write.size() > 16)
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log messages %zd larger buffers\n", buffers_to_write.size() - 2);
            fputs(buf, stderr);
            // 如果日志太多，则丢掉一些数据
            buffers_to_write.erase(buffers_to_write.begin() + 2, buffers_to_write.end());
        }

        for (const auto& buffer : buffers_to_write)
        {
            output.append(buffer->data(), static_cast<size_t>(buffer->length()));
        }

        if (buffers_to_write.size() > 2)
        {
            buffers_to_write.resize(2);
        }

        if (!new_buffer1)
        {
            assert(!buffers_to_write.empty());
            new_buffer1 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer1->reset();
        }

        if (!new_buffer2)
        {
            assert(!buffers_to_write.empty());
            new_buffer2 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer2->reset();
        }
        buffers_to_write.clear();
        output.flush();
    }
    output.flush();
}

} // namespace momoko
