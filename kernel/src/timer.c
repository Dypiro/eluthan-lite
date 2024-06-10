#include <stdint.h>
void delay(uint64_t milliseconds) {
    // Get the current timestamp counter value
    uint64_t start = __builtin_ia32_rdtsc();

    // Calculate the target timestamp counter value
    uint64_t target = start + (milliseconds * (uint64_t)1000000); // Assuming TSC operates at 1 GHz

    // Wait until the current timestamp counter reaches the target
    while (__builtin_ia32_rdtsc() < target) {
        // Do nothing, just wait
    }
}