#include <atomic>
#include <thread>
#include <cassert>
#include <iostream>

std::atomic<int> data{0};
std::atomic<bool> flag{false};
int result = 0;

void writer() {
    data.store(42, std::memory_order_relaxed);
    flag.store(true, std::memory_order_release);
}

void reader() {
    while (!flag.load(std::memory_order_acquire));
    result = data.load(std::memory_order_relaxed);
}

int main() {
    std::thread t1(writer);
    std::thread t2(reader);
    t1.join();
    t2.join();

    std::cout << "Result = " << result << "\n";
    assert(result == 42);
}
