
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

/* MaxCapacity is the maximum number of possible instructions in the program,
 * which is just the length of the program string */
template <size_t MaxCapacity>
struct Program {
    std::array<Instruction, MaxCapacity> Instructions;
    std::array<size_t, MaxCapacity> JumpTargets;
    size_t InstructionCount;
};

/* Since the instruction pointer only ever increments by a known constant at compile-time
 * (specifically, 1) or jumps to another position which is also known at compile-time because
 * of the constexpr parser, we can extract the instruction pointer to a template parameter instead.
 * We can replace the switch statement with if constexpr statements to force the compiler to execute
 * the tail-recursive function calls and generate the assembly code at compile-time. */
template <const auto& Program, size_t InstrPtr = 0>
constexpr void ExecuteProgram(uint8_t* dataPtr)  {
    if constexpr (InstrPtr >= Program.InstructionCount) {
        return;

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::IncrementPtr) {
        dataPtr++;
        return ExecuteProgram<Program, InstrPtr+1>(dataPtr);

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::DecrementPtr) {
        dataPtr--;
        return ExecuteProgram<Program, InstrPtr+1>(dataPtr);

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::IncrementData) {
        (*dataPtr)++;
        return ExecuteProgram<Program, InstrPtr+1>(dataPtr);

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::DecrementData) {
        (*dataPtr)--;
        return ExecuteProgram<Program, InstrPtr+1>(dataPtr);

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::Read) {
        *dataPtr = static_cast<uint8_t>(getchar());
        return ExecuteProgram<Program, InstrPtr+1>(dataPtr);

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::Write) {
        putchar(*dataPtr);
        return ExecuteProgram<Program, InstrPtr+1>(dataPtr);

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::Jump) {
        return ExecuteProgram<Program, Program.JumpTargets[InstrPtr]>(dataPtr);

    } else if constexpr (Program.Instructions[InstrPtr] == Instruction::JumpIfZero) {
        if (*dataPtr == 0) {
            return ExecuteProgram<Program, Program.JumpTargets[InstrPtr]>(dataPtr);
        } else {
            return ExecuteProgram<Program, InstrPtr+1>(dataPtr);
        }
    }
}


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
            auto start = jumpStack[--jumpStackPtr];
            auto end = program.InstructionCount++;

            program.Instructions[end] = Instruction::Jump;
            program.JumpTargets[end] = start;
            program.JumpTargets[start] = end + 1;
        }
    }

    return program;
}


#endif //COMPILE_TIME_CODE_GENERATOR_BRAINFUCK_H
