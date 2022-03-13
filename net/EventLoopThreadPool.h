#ifndef NET_EVENTLOOPTHREADPOOL_H
#define NET_EVENTLOOPTHREADPOOL_H
#include <functional>
#include <memory>
#include <string>
#include <vector>

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop* baseloop, const std::string& nameArg);
    ~EventLoopThreadPool();

    void start(const ThreadInitCallback cb = ThreadInitCallback());
    void setThreadNum(int num);

    EventLoop* getNextLoop();
    std::vector<EventLoop*> getAllLoops();

private:
    EventLoop* baseloop_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;

    int numThread_;
    int next_;
    std::string names_;
    int started_;
};
#endif
