#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;  // 全局互斥锁
int sharedData = 0;

template <typename Mutex>
class my_lock_guard {
   public:
    explicit my_lock_guard(Mutex& m) : mutex(m) {
        mutex.lock();  // 构造时加锁
    }
    ~my_lock_guard() {
        mutex.unlock();  // 析构时解锁
    }
    // 禁止拷贝和赋值
    my_lock_guard(const my_lock_guard&) = delete;
    my_lock_guard& operator=(const my_lock_guard&) = delete;

   private:
    Mutex& mutex;
};

void incrementData() {
    std::lock_guard<std::mutex> lock(mtx);  // 构造时加锁
    sharedData++;                           // 临界区操作
}  // 析构时自动解锁

void myIncrementData() {
    my_lock_guard<std::mutex> lock(mtx);  // 使用自定义的锁
    sharedData++;                         // 临界区操作
}  // 析构时自动解锁

int main() {
    std::thread t1(incrementData);
    std::thread t2(incrementData);
    t1.join();
    t2.join();
    std::cout << "Standard lockguard Final value: " << sharedData
              << std::endl;  // 输出 2

    sharedData = 0;

    std::thread mt1(myIncrementData);
    std::thread mt2(myIncrementData);
    mt1.join();
    mt2.join();
    std::cout << "My lockguard Final value: " << sharedData
              << std::endl;  // 输出 2
    return 0;
}