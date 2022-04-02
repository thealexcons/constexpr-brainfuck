#include <iostream>

#include "brainfuck.h"

int main() {
    static constexpr auto add3 = ParseProgram(",>+++<[->+<]>.");

    uint8_t memory[1024] = {};
    add3.Execute(memory);

    return 0;
}
