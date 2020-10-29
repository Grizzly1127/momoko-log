#ifndef MOMOKOLOG_LOGGER_H
#define MOMOKOLOG_LOGGER_H

#include "momoko-log/log_stream.h"
#include "momoko-log/async_log.h"
#include <functional>

namespace momoko
{

class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class SourceFile
    {
    public:
        SourceFile(const char* file)
            : file_(file)
        {
            const char* slash = strrchr(file, '/');
            if (slash)
            {
                file_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(file_));
        }
        const char* file_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func_name);

    ~Logger();

    LogStream& stream() { return impl_.stream_; }
    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);
    static void setAsync();
    typedef std::function<void(const LogStream::Buffer&)> OutputFunc;
    static void setOutputFunc(OutputFunc func);

    class Impl
    {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, const SourceFile& file, int line);

        void formatTime();
        void getThreadId();

        int64_t time_;
        LogStream stream_;
        LogLevel level_;
        SourceFile file_;
        int line_;
    };

private:
    Impl impl_;
    static bool is_async_;
};

extern Logger::LogLevel g_log_level;
inline Logger::LogLevel Logger::logLevel()
{
    return g_log_level;
}

extern bool g_is_async_;
inline void Logger::setAsync()
{
    g_is_async_ = true;
}

} // namespace momoko




#define SET_LOGLEVEL(x) momoko::Logger::setLogLevel(x);

// set to asynchronous logger
#define LOG_SET_ASYNC(x) if ( x != 0 ) { \
    static momoko::AsyncLog g_async_; \
    momoko::Logger::setOutputFunc( \
            [&](const momoko::LogStream::Buffer& buf) { g_async_.append(buf.data(), buf.length()); }); \
    momoko::Logger::setAsync(); \
}

#define LOG_TRACE if (momoko::Logger::logLevel() <= momoko::Logger::TRACE) \
        (momoko::Logger(__FILE__, __LINE__, momoko::Logger::TRACE, __func__).stream())
#define LOG_DEBUG if (momoko::Logger::logLevel() <= momoko::Logger::DEBUG) \
        (momoko::Logger(__FILE__, __LINE__, momoko::Logger::DEBUG, __func__).stream())
#define LOG_INFO  momoko::Logger(__FILE__, __LINE__, momoko::Logger::INFO).stream()
#define LOG_WARN  momoko::Logger(__FILE__, __LINE__, momoko::Logger::WARN).stream()
#define LOG_ERROR momoko::Logger(__FILE__, __LINE__, momoko::Logger::ERROR).stream()
#define LOG_FATAL momoko::Logger(__FILE__, __LINE__, momoko::Logger::FATAL).stream()

#endif // MOMOKOLOG_LOGGER_H
