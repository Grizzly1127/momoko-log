#ifndef MOMOKOLOG_LOGSTREAM_H
#define MOMOKOLOG_LOGSTREAM_H

#include "momoko-log/noncopyable.h"
#include <stdlib.h>
#include <cstring>
#include <string>
#include <iostream>

namespace momoko
{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template<int SIZE>
class LogBuffer : noncopyable
{
public:
    LogBuffer()
        : cur_(data_)
    {
    }
    ~LogBuffer()
    {
    }

    void append(const char* buf, int len) {
        if (avail() > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }
    void add(size_t len) { cur_ += len; }
    char* current() { return cur_; }

    void reset() { cur_ = data_; }
    void bzero() { memset(data_, 0, sizeof(data_)); }

    int avail() const { return static_cast<int>(end() - cur_); }
private:
    const char* end() const { return data_ + sizeof(data_); }
    char data_[SIZE];
    char* cur_;
};

class LogStream : noncopyable
{
    typedef LogStream self;
public:
    typedef LogBuffer<kSmallBuffer> Buffer;

    self& operator<<(bool v) { buffer_.append(v ? "1" : "0", 1); return *this; }
    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);
    self& operator<<(const void*);
    self& operator<<(float v) { *this << static_cast<double>(v); return *this; }
    self& operator<<(double);
    self& operator<<(char v) { buffer_.append(&v, 1); return *this; }
    self& operator<<(const char* str) {
        if (str)
        {
            buffer_.append(str, static_cast<int>(strlen(str)));
        }
        else
        {
            buffer_.append("(null)", 6);
        }
        return *this;
    }
    self& operator<<(const unsigned char* v) { return operator<<(reinterpret_cast<const char*>(v)); }
    self& operator<<(const std::string& v) { buffer_.append(v.c_str(), static_cast<int>(v.size())); return *this; }
    self& operator<<(const Buffer& v) { buffer_.append(v.data(), v.length()); return *this; };

    void append(const char* data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    template<typename T>
    void formatInteger(T);

private:
    Buffer buffer_;

    static const int kMaxNumericSize = 32;
};

} // namespace momoko

#endif // MOMOKOLOG_LOGSTREAM_H
