#ifndef BASE_NONCOPYABLE_H
#define BASE_NONCOPYABLE_H

class noncopyable {
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator = (noncopyable& ) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif
