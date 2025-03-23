#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> x{0}, y{0};
int r1 = 0, r2 = 0;

void thread1() {
    r1 = x.load(std::memory_order_relaxed);
    y.store(1, std::memory_order_relaxed);
}

void thread2() {
    r2 = y.load(std::memory_order_relaxed);
    x.store(1, std::memory_order_relaxed);
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);
    t1.join();
    t2.join();

    if (r1 == 0 && r2 == 0) {
        std::cout << "Load buffering observed: r1 = 0, r2 = 0\n";
    }
}
