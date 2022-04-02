
#ifndef COMPILE_TIME_CODE_GENERATOR_BRAINFUCK_H
#define COMPILE_TIME_CODE_GENERATOR_BRAINFUCK_H

#include <array>
#include <iostream>

enum class Instruction {
    IncrementPtr,       // >
    DecrementPtr,       // <
    IncrementData,      // +
    DecrementData,      // -
    Read,               // ,
    Write,              // .
    JumpIfZero,         // [
    Jump                // ]
};

template <size_t Capacity>
struct Program {
    std::array<Instruction, Capacity> Instructions;
    std::array<size_t, Capacity> JumpTargets;
    size_t InstructionCount;

    void Execute(uint8_t* dataPtr) const {
        auto instrPtr = 0u;
        while (instrPtr < this->InstructionCount) {
            switch (this->Instructions[instrPtr]) {
                case Instruction::IncrementPtr:
                    dataPtr++;
                    instrPtr++;
                    break;
                case Instruction::DecrementPtr:
                    dataPtr--;
                    instrPtr++;
                    break;
                case Instruction::IncrementData:
                    (*dataPtr)++;
                    instrPtr++;
                    break;
                case Instruction::DecrementData:
                    (*dataPtr)--;
                    instrPtr++;
                    break;
                case Instruction::Read:
                    *dataPtr = static_cast<uint8_t>(getchar());
                    instrPtr++;
                    break;
                case Instruction::Write:
                    putchar(*dataPtr);
                    instrPtr++;
                    break;
                case Instruction::Jump:
                    instrPtr = this->JumpTargets[instrPtr];
                    break;
                case Instruction::JumpIfZero:
                    instrPtr = *dataPtr == 0 ? this->JumpTargets[instrPtr] : instrPtr + 1;
                    break;
                default:
                    return;
            }
        }
    }

};


// Compile-time parsing
template <size_t Len>
constexpr Program<Len> ParseProgram(const char (&str)[Len]) {  // the (&str) syntax allows for template argument deduction
    Program<Len> program{};
    std::array<size_t, Len> jumpStack{};
    size_t jumpStackPtr = 0;

    for (auto ptr = str; *ptr; ++ptr) {
        if (*ptr == '>') {
            program.Instructions[program.InstructionCount++] = Instruction::IncrementPtr;
        } else if (*ptr == '<') {
            program.Instructions[program.InstructionCount++] = Instruction::DecrementPtr;
        } else if (*ptr == '+') {
            program.Instructions[program.InstructionCount++] = Instruction::IncrementData;
        } else if (*ptr == '-') {
            program.Instructions[program.InstructionCount++] = Instruction::DecrementData;
        } else if (*ptr == '.') {
            program.Instructions[program.InstructionCount++] = Instruction::Write;
        } else if (*ptr == ',') {
            program.Instructions[program.InstructionCount++] = Instruction::Read;
        } else if (*ptr == '[') {
            jumpStack[jumpStackPtr++] = program.InstructionCount;
            program.Instructions[program.InstructionCount++] = Instruction::JumpIfZero;
        } else if (*ptr == ']') {
            auto open = jumpStack[--jumpStackPtr];
            auto close = program.InstructionCount++;

            program.Instructions[close] = Instruction::Jump;
            program.JumpTargets[close] = open;
            program.JumpTargets[open] = close + 1;
        }
    }

    return program;
}


#endif //COMPILE_TIME_CODE_GENERATOR_BRAINFUCK_H
