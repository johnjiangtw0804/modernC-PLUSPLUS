#include <iostream>
#include <mutex>
#include <thread>

class MyLockGuard
{
public:
    MyLockGuard(std::mutex &m);
    ~MyLockGuard();

    MyLockGuard(const MyLockGuard &) = delete;
    MyLockGuard& operator=(const MyLockGuard &) = delete;

    MyLockGuard(const MyLockGuard&&) = delete;
    MyLockGuard& operator=(const MyLockGuard&&) = delete;

private:
    // to prevent calling a NULL pointer
    std::mutex &mutex;
};

MyLockGuard::MyLockGuard(std::mutex &m)
    : mutex(m)
{
    mutex.lock();
    std::cout << "🔒 Lock acquired by thread " << std::this_thread::get_id() << std::endl;
}

MyLockGuard::~MyLockGuard()
{
    mutex.unlock();
    std::cout << "🔓 Lock released by thread " << std::this_thread::get_id() << std::endl;
}

void critical_section(std::mutex &m)
{
    MyLockGuard lock(m);
    std::cout << "🧠 Critical section running in thread "
              << std::this_thread::get_id() << std::endl;
}

int main()
{
    std::mutex mtx;
    std::thread t1(critical_section, std::ref(mtx));
    std::thread t2(critical_section, std::ref(mtx));

    t1.join();
    t2.join();

    std::cout << "✅ Threads finished." << std::endl;
}
