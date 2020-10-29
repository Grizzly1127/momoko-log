#ifndef MOMOKOLOG_NONCOPYABLE_H
#define MOMOKOLOG_NONCOPYABLE_H

namespace momoko
{

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

} // namespace momoko

#endif // MOMOKOLOG_NONCOPYABLE_H
