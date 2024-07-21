#pragma once

#include <cstdint>
#include <chrono>
#include <random>


//


class Chip8 {
public:

    // Various data needed to use the chip8 system 
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t index;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t sp;
    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};
    uint16_t instruction;
    /**** \/ ADD THESE LATER \/ ***/
    // uint8_t x;
    // uint8_t y;
    // uint8_t n;
    // uint16_t nn;
    // uint16_t nnn;
    // table[]{};?
    /**** /\ ADD THESE LATER /\ ***/

    void LoadROM(char const* filename);

    void Cycle();

    Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
        randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
    }
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    // UNIQUE OPCODES
    void OP_1NNN();
    void OP_2NNN();
    void OP_3XNN();
    void OP_4XNN();
    void OP_5XY0();
    void OP_6XNN();
    void OP_7XNN();
    void OP_9XY0();
    void OP_ANNN();
    void OP_BNNN();
    void OP_CXNN();
    void OP_DXYN();

    // 0 OPCODES
    void OP_00E0();
    void OP_00EE();

    // 8 OPCODES
    void OP_8XY0();
    void OP_8XY1();
    void OP_8XY2();
    void OP_8XY3();
    void OP_8XY4();
    void OP_8XY5();
    void OP_8XY6();
    void OP_8XY7();
    void OP_8XYE();

    // E OPCODES
    void OP_EXA1();
    void OP_EX9E();

    // F OPCODES
    void OP_FX07();
    void OP_FX0A();
    void OP_FX15();
    void OP_FX18();
    void OP_FX1E();
    void OP_FX29();
    void OP_FX33();
    void OP_FX55();
    void OP_FX65();


    // Set up function pointer table
        // A table full of pointers to functions
    /*NOTE: For opcodes with multiple intructions(0, 8, E, F),
        these must be parsed with an additional function*/
    table[0x1] = &Chip8::OP_1NNN;
    table[0x2] = &Chip8::OP_2NNN;
    table[0x3] = &Chip8::OP_3XNN;
    table[0x4] = &Chip8::OP_4XNN;
    table[0x5] = &Chip8::OP_5XY0;
    table[0x6] = &Chip8::OP_6XNN;
    table[0x7] = &Chip8::OP_7XNN;
    table[0x9] = &Chip8::OP_9XY0;
    table[0xA] = &Chip8::OP_ANNN;
    table[0xB] = &Chip8::OP_BNNN;
    table[0xC] = &Chip8::OP_CXNN;
    table[0xD] = &Chip8::OP_DXYN;

    //SUB-TABLES:
    table[0x0] = &Chip8::Table0;
    table[0x8] = &Chip8::Table8;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    // FILL IN SUB-TABLES:
        // Fill NULL entries:
    for (size_t i = 0; i < 0xE; i++) {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }
    for (size_t i = 0; i < 0x65; i++) {
        tableF[i] = &Chip8::OP_NULL;
    }
        // table indicies for 0,8,E correlate to last digit
        // in instruction
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8XY0;
    table8[0x1] = &Chip8::OP_8XY1;
    table8[0x2] = &Chip8::OP_8XY2;
    table8[0x3] = &Chip8::OP_8XY3;
    table8[0x4] = &Chip8::OP_8XY4;
    table8[0x5] = &Chip8::OP_8XY5;
    table8[0x6] = &Chip8::OP_8XY6;
    table8[0x7] = &Chip8::OP_8XY7;
    table8[0xE] = &Chip8::OP_8XYE;
    
    tableE[0x1] = &Chip8::OP_EXA1;
    tableE[0xE] = &Chip8::OP_EX9E;
        // table indicies for F correlate to last 2 digists
        // in instruction
    tableF[0x07] = &Chip8::OP_FX07;
    tableF[0x0A] = &Chip8::OP_FX0A;
    tableF[0x15] = &Chip8::OP_FX15;
    tableF[0x18] = &Chip8::OP_FX18;
    tableF[0x1E] = &Chip8::OP_FX1E;
    tableF[0x29] = &Chip8::OP_FX29;
    tableF[0x33] = &Chip8::OP_FX33;
    tableF[0x55] = &Chip8::OP_FX55;
    tableF[0x65] = &Chip8::OP_FX65;

    // HANDLE SUB-TABLES:
    void Table0() {
        ((*this).*(table0[instruction & 0x000Fu]))();
    }
    void Table8() {
        ((*this).*(table8[instruction & 0x000Fu]))();
    }
    void TableE() {
        ((*this).*(tableE[instruction & 0x000Fu]))();
    }
    void TableF() {
        ((*this).*(tableF[instruction & 0x00FFu]))();
    }

    private:

        void OP_NULL();
};


//typedef void (Chip8::* Chip8Func)();
//Chip8Func table[0xF + 1];
//Chip8Func table0[0xE + 1];
//Chip8Func table8[0xE + 1];
//Chip8Func tableE[0xE + 1];
//Chip8Func tableF[0x65 + 1];