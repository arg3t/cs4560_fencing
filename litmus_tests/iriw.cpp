#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> x{0}, y{0};
int r1 = 0, r2 = 0, r3 = 0, r4 = 0;

void writer1() {
    x.store(1, std::memory_order_relaxed);
}

void writer2() {
    y.store(1, std::memory_order_relaxed);
}

void reader1() {
    r1 = x.load(std::memory_order_relaxed);
    r2 = y.load(std::memory_order_relaxed);
}

void reader2() {
    r3 = y.load(std::memory_order_relaxed);
    r4 = x.load(std::memory_order_relaxed);
}

int main() {
    std::thread t1(writer1);
    std::thread t2(writer2);
    std::thread t3(reader1);
    std::thread t4(reader2);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "r1=" << r1 << " r2=" << r2
              << " r3=" << r3 << " r4=" << r4 << "\n";
}
