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
        /**** /\ ADD THESE LATER /\ ***/
        
        void LoadROM(char const* filename);
 
        Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
        {
            randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
        }

        void Chip8::OP_00E0() { //clear screen
            memset(video, 0, sizeof(video));
        }

        void Chip8::OP_1NNN() { // jump
            uint16_t addr = instruction & 0x0FFFu;
            pc = addr;
        }

        void Chip8::OP_2NNN() {
            stack[sp] = pc;
            sp++;

            uint16_t addr = instruction & 0x0FFFu;
            pc = addr;
        }

        void Chip8::OP_00EE() {
            sp--;
            pc = stack[sp];
        }

        void Chip8::OP_3XNN() {
            // if value at register vx is equal to 0x00FFu
            uint8_t nn = instruction & 0x00FFu;
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t vx = registers[x];
            if (vx == nn) { // skip
                pc += 2;
            }
        }

        void Chip8::OP_4XNN() {
            uint8_t nn = instruction & 0x00FFu;
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t vx = registers[x];
            if (vx != nn) { // skip
                pc += 2;
            }
        }

        void Chip8::OP_5XY0() {
            // if value in register vx equals value in register vy
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t vx = registers[x];
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            uint8_t vy = registers[y];
            if (vx == vy) { // skip
                pc += 2;
                //this.OP_1NNN?
            }
        }

        void Chip8::OP_9XY0() {
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t vx = registers[x];
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            uint8_t vy = registers[y];
            if (vx != vy) { // skip
                pc += 2;
            }

        }

        void Chip8::OP_6XNN() {
            uint8_t nn = instruction & 0x00FFu;
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            registers[x] = nn;
        }

        void Chip8::OP_7XNN() {
            uint8_t nn = instruction & 0x00FFu;
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            registers[x] += nn; // account for overflow here?
        }
        
        void Chip8::OP_8XY0() {
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            registers[x] = registers[y];
        }

        void Chip8::OP_8XY1() {
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            registers[x] = registers[x] | registers[y];
        }

        void Chip8::OP_8XY2() {
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            registers[x] = registers[x] & registers[y];
        }

        void Chip8::OP_8XY3() {
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            registers[x] = registers[x] ^ registers[y];
        }

        void Chip8::OP_8XY4() {
            uint8_t nn = instruction & 0x00FFu;
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            registers[15] = ((registers[x] + nn) > 255) ? 1 : 0;
            /*if ((registers[x] + nn) > 255) {
                registers[15] = 1; 
            } else { registers[15] = 0; }*/
            registers[x] += nn; // account for overflow here?
        }

        // if first operand larger than second, set VF to 1, otherwise 0
        void Chip8::OP_8XY5() {
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            registers[15] = (registers[x] >= registers[y]) ? 1 : 0;
            registers[x] -= registers[y];
        }
        // if first operand larger than second, set VF to 1, otherwise 0
        void Chip8::OP_8XY7() {
            uint8_t y = (instruction & 0x00F0u) >> 4u;
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            registers[15] = (registers[y] >= registers[x]) ? 1 : 0;
            registers[x] = registers[y] - registers[x];
        }

        void Chip8::OP_8XY6() {
            /* OPTIONAL: set VX to value of VY */
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t to_shift = registers[x];
            registers[15] = ((to_shift & 0x01u) == 0x01u) ? 1 : 0;
            registers[x] = to_shift >> 1u;
        }

        void Chip8::OP_8XYE() {
            /* OPTIONAL: set VX to value of VY */
            uint8_t x = (instruction & 0x0F00u) >> 8u;
            uint8_t to_shift = registers[x];
            registers[15] = ((to_shift & 0x80u) == 0x80u) ? 1 : 0;
            registers[x] = to_shift << 1u;
        }

        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;
    private:
};