#ifndef MOMOKOLOG_TIMESTAMP_H
#define MOMOKOLOG_TIMESTAMP_H

#include "momoko-log/copyable.h"

#include <string>

using namespace std;

namespace momoko
{

class Timestamp
{
public:
    static int64_t now();
    static const int kMicroSecondsPerSecond = 1000 * 1000;
};

} // namespace momoko

#endif // MOMOKOLOG_TIMESTAMP_H
