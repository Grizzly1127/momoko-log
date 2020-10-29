# momokolog


momokolog 是基于 C++11 开发的高性能异步日志框架，该框架只能用于 Linux 中，不支持跨平台。  
momokolog 日志库特点如下：

1. 每秒可写百万条数据到磁盘中。
2. 多线程程序并发写日志到一个日志文件中。
3. 文件滚动。
4. 日志多级别输出。
5. 接口简单(重载了多种数据类型的 `<<` 操作符)，使用方便。
6. 采用批处理方式记录日志。

momokolog 日志输出格式固定，以避免频繁改动日志格式对后面日志数据分析加强难度。

日志格式：

```text
timestamp threadId logLevel file:line ->  content
```

# Build

```bash
./build.sh
```

# Code Examples

```c++
#include "momokolog/logger.h"

int main() {
    // set log level
    //SET_LOGLEVEL(momokolog::Logger::INFO)

    // set to asynchronous logger
    LOG_SET_ASYNC(1)

    LOG_INFO << "info message, num." << 1;
    LOG_WARN << "warning message, num." << 2;
    LOG_ERROR << "error message, num." << 3;
    return 0;
}
```
