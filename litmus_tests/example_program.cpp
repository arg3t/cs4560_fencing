#include <iostream>
#include <atomic>

std::atomic<int> x, y;

int main() {
    x.store(100, std::memory_order_relaxed);
    y.store(0, std::memory_order_relaxed);
    int i = x.load(std::memory_order_relaxed);
loop:
    if (i > 0) {
        y.store(i, std::memory_order_relaxed);
        --i;
        printf("i: %d\n", i);
        goto loop;
    } 
    else {
        return 0;
    }
}