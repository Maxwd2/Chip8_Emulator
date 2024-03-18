#pragma once

#include <cstdint>

class Chip8 {
    public:
        // Various data needed to use the chip8 system 
        uint8_t registers[16];
        uint8_t memory[4096];
        uint16_t index;
        uint16_t pc;
        uint16_t stack[16];
        uint8_t sp;
        uint16_t opcode;

        void LoadROM(char const* filename);
    private:
};