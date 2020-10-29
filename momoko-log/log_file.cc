#include "momoko-log/log_file.h"

#include <iostream>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

namespace momoko
{

namespace file
{

void FileWriter::append(const char* line, const size_t len)
{
    size_t n = ::fwrite(line, 1, len, file_);
    size_t remain = len - n;
    while (remain > 0)
    {
        size_t x = ::fwrite(line + n, 1, remain, file_);
        if (x == 0)
        {
            int err = ferror(file_);
            if (err)
            {
                fprintf(stderr, "FileBase::append() failed %d\n", err);
            }
            break;
        }
        n += x;
        remain -= x;
    }
    writtern_bytes_ += len;
}

}

LogFile::LogFile(off_t roll_size)
    : roll_size_(roll_size),
      file_index_(0)
{
    setBaseName();
    rollFile();
}
LogFile::~LogFile() = default;

void LogFile::rollFile()
{
    std::string file_name = getLogFileName();
    file_.reset(new file::FileWriter(file_name.c_str()));
    unlink(linkname_);
    symlink(file_name.c_str(), linkname_);
}

void LogFile::setBaseName()
{
    char log_abs_path[PATH_MAX] = {0};
    ::getcwd(log_abs_path, sizeof(log_abs_path));
    strcat(log_abs_path, "/log/");
    if (::access(log_abs_path, 0) == -1)
    {
        ::mkdir(log_abs_path, 0755);
    }

    char process_abs_path[PATH_MAX] = {0};
    long len = ::readlink("/proc/self/exe", process_abs_path, sizeof(process_abs_path));
    if (len <= 0) {
        return;
    }
    char* process_name = strrchr(process_abs_path, '/') + 1;
    snprintf(linkname_, sizeof(linkname_), "%s%s.log", log_abs_path, process_name);
    snprintf(basename_, sizeof(basename_), "%s%s.%d.", log_abs_path, process_name, ::getpid());
}

std::string LogFile::getLogFileName()
{
    std::string file_name(basename_);
    char timebuf[32] = {0};
    struct tm tm;
    time_t now = time(NULL);
    ::gmtime_r(&now, &tm);
    strftime(timebuf, sizeof(timebuf), "%Y%m%d-%H%M%S.", &tm);
    file_name += timebuf;

    char index[8] = {0};
    snprintf(index, sizeof(index), "%03d.log", file_index_);
    ++file_index_;
    file_name += index;
    return file_name;
}

void LogFile::append(const char* line, const size_t len)
{
    std::unique_lock<std::mutex> guard(mutex_);
    file_->append(line, len);

    if (file_->writternBytes() > roll_size_)
    {
        rollFile();
    }
}

void LogFile::flush()
{
    std::unique_lock<std::mutex> guard(mutex_);
    file_->flush();
}

} // namespace momoko
