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
            uint8_t x = instruction & 0x0F00u;
            uint8_t vx = registers[x];
            if (vx == nn) { // skip
                pc += 2;
            }
        }

        void Chip8::OP_4XNN() {
            uint8_t nn = instruction & 0x00FFu;
            uint8_t x = instruction & 0x0F00u;
            uint8_t vx = registers[x];
            if (vx != nn) { // skip
                pc += 2;
            }
        }

        void Chip8::OP_5XY0() {
            // if value in register vx equals value in register vy
            uint8_t x = instruction & 0x0F00u;
            uint8_t vx = registers[x];
            uint8_t y = instruction & 0x00F0u;
            uint8_t vy = registers[y];
            if (vx == vy) { // skip
                pc += 2;
                //this.OP_1NNN?
            }
        }

        void Chip8::OP_9XY0() {
            uint8_t x = instruction & 0x0F00u;
            uint8_t vx = registers[x];
            uint8_t y = instruction & 0x00F0u;
            uint8_t vy = registers[y];
            if (vx != vy) { // skip
                pc += 2;
            }

        }

        void Chip8::OP_6XNN() {
            uint8_t nn = instruction & 0x00FFu;
            uint8_t x = instruction & 0x0F00u;
            registers[x] = nn;
        }

        void Chip8::OP_7XNN() {
            uint8_t nn = instruction & 0x00FFu;
        }

        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;
    private:
};