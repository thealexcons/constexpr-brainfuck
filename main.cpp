
#include "brainfuck.h"

int main() {
    static constexpr auto add2 = ParseProgram(",>++<[->+<]>.");

    uint8_t memory[512] = {};
    ExecuteProgram<add2>(memory);

    /* Assuming tail-recursion optimisations are available, the compiler will translate
     * the ExecuteProgram() call above with a call to getchar, add 2 and a call to putchar.
     * Note that we are outputting one byte at the end, so for multiple-digit answers you
     * will see the wrong answer. */

    return 0;
}
