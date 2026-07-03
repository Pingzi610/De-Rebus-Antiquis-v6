#include <stdint.h>
#include <target.h>

void _start() {
    memalign_end_handler();
}