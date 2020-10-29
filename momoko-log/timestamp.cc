#include "momoko-log/timestamp.h"

#include <sys/time.h>
#include <chrono>
#include <ctime>

namespace momoko
{

int64_t Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec;
}

} // namespace momoko
