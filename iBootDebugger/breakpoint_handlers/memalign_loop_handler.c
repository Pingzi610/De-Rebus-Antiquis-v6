#include <stdint.h>
#include <target.h>

void _start() {
    memalign_loop_handler();
}