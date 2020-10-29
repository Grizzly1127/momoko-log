#ifndef MOMOKOLOG_LOG_FILE_H
#define MOMOKOLOG_LOG_FILE_H

#include "momoko-log/noncopyable.h"

#include <mutex>
#include <memory>
#include <stdio.h>
#include <limits.h>

namespace momoko
{

namespace file
{

class FileWriter: noncopyable
{
public:
    explicit FileWriter(std::string file_name)
        : file_(::fopen(file_name.c_str(), "ae")),
          writtern_bytes_(0)
    {
        ::setbuffer(file_, buffer_, sizeof(buffer_));
    }
    ~FileWriter() { ::fclose(file_); }

    off_t writternBytes() const { return writtern_bytes_; };
    void append(const char* line, const size_t len);
    void flush() { ::fflush(file_); }

private:
    FILE* file_;
    char buffer_[64 * 1024];
    off_t writtern_bytes_;
};

} // namespace base


class LogFile : noncopyable
{
public:
    LogFile(off_t roll_size);
    ~LogFile();

    void append(const char* line, const size_t len);
    void flush();
    void rollFile();

private:
    void setBaseName();
    std::string getLogFileName();

private:
    char linkname_[PATH_MAX];
    char basename_[PATH_MAX];
    off_t roll_size_;
    int file_index_;
    std::mutex mutex_;
    std::unique_ptr<file::FileWriter> file_;
};

} // namespace momoko

#endif // MOMOKOLOG_LOG_FILE_H
