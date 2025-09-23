#include "chip8.hpp"
#include <fstream>
#include <string>

//const unsigned int START_ADDR = 0x200;

const unsigned int FONTSET_START_ADDR = 0x50;
const unsigned int FONTSET_SIZE = 80;

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0x60, 0x90, 0x20, 0x40, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x80, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0x60, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

//Chip8::Chip8() {
//    pc = START_ADDR;
//}

// Function for reading in ROM file, taken from https://austinmorlan.com/posts/chip8_emulator/
void Chip8::LoadROM(const std::string filename) {
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)	{
			memory[START_ADDR + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
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
    // if value at register vx is equal to 0x00FFu then skip
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
    // if value in register vx equals value in register vy then skip
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    uint8_t vx = registers[x];
    uint8_t y = (instruction & 0x00F0u) >> 4u;
    uint8_t vy = registers[y];
    if (vx == vy) { // skip
        pc += 2;
        //this.OP_1NNN ?
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

// Add vx and vy and set the VF to 1 if there is carry
// VF = registers[0xF] = registers[15]
void Chip8::OP_8XY4() {
    uint8_t nn = instruction & 0x00FFu;
    uint8_t x = (instruction & 0x0F00u) >> 8u;

    registers[15] = ((registers[x] + nn) > 255) ? 1 : 0;
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

void Chip8::OP_ANNN() {
    uint16_t nnn = instruction & 0x0FFFu;
    index = nnn;
}

void Chip8::OP_BNNN() {
    // configure other options
    uint16_t nnn = instruction & 0x0FFFu;
    pc = nnn + registers[0];
}

void Chip8::OP_CXNN() {
    uint8_t nn = instruction & 0x00FFu;
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    registers[x] = randByte(randGen) & nn;
}

void Chip8::OP_DXYN() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    uint8_t y = (instruction & 0x00F0u) >> 4u;
    uint8_t n = instruction & 0x000Fu;

    uint8_t vx = registers[x] % 64;
    uint8_t vy = registers[y] % 32;

    uint8_t x_coord = vx;
    uint8_t y_coord = vy;
    registers[0xF] = 0;

    for (unsigned int i = 0; i < n; i++) {
        // get the nth byte of sprite data
        uint8_t sprite_byte = memory[index + i];
        x_coord = vx;
        // for each of the 8 pixels/bits in this sprite row...
        for (unsigned int j = 0; j < 8; j++) {
            uint8_t curr_sprite_pixel/*_bit*/ = sprite_byte & (0x80u >> j);
            uint32_t* curr_screen_pixel = &video[(y_coord * 64) + x_coord];

            if (curr_sprite_pixel) {
                if (*curr_screen_pixel) {
                    *curr_screen_pixel = 0x0;
                    registers[0xF] = 1;
                }
                else {
                    *curr_screen_pixel = 0xFFFFFFFF;
                }
            }
            x_coord += 1;
        }
        y_coord += 1;
    }

    ////sprites are straight vertical lines 1-15 bytes tall?
    //for (unsigned int row = 0; row < n; row++) {
    //    uint8_t sprite_byte = memory[index + row];

    //    for (unsigned int col = 0; col < 8; col++) {
    //        uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
    //        //uint32_t* screen_pixel = &video[(y_coord + row) * 64 + (x_coord + col)];
    //        uint32_t* screen_pixel = &video[(vy * 64) + (vx)];


    //        if (sprite_pixel) {
    //            if (*screen_pixel == 0xFFFFFFFF) {
    //                registers[15] = 1;
    //            }
    //            *screen_pixel ^= 0xFFFFFFFF;
    //        }
    //    }
    //}
}

void Chip8::OP_EX9E() {
    // TODO: will need to implement input for this
}

void Chip8::OP_EXA1() {
    // TODO: will need to implement input for this
}

void Chip8::OP_FX07() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    //TODO: registers[x] = delay timer;
}

void Chip8::OP_FX15() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    //TODO: delay timer = registers[x];
}

void Chip8::OP_FX18() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    //TODO: sound timer = registers[x];
}

void Chip8::OP_FX1E() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    index += registers[x];
    //check for overflow, set VF
}

void Chip8::OP_FX0A() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;

    bool input_detected = false;
    for (uint8_t i = 0; i < 16; i++) {
        if (keypad[i]) {
            registers[x] = i;
            input_detected = true;
            break;
        }
    }

    if (!input_detected) {
        pc -= 2;
    }
}

void Chip8::OP_FX29() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    uint8_t vx = registers[x];
    index = FONTSET_START_ADDR + (5 * vx);
}

void Chip8::OP_FX33() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    uint8_t vx = registers[x];
    memory[index] = vx / 100;
    memory[index + 1] = (vx % 100) / 10;
    memory[index + 2] = (vx % 100) % 10;
}

// Store registers V0 through Vx in memory starting at location I.
void Chip8::OP_FX55() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i < x; i++) {
        memory[index + i] = registers[i]; // correct offset.
    }
}

// Fill registers V0 to Vx with memory starting at I
void Chip8::OP_FX65() {
    uint8_t x = (instruction & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i < x; i++) {
        registers[i] = memory[index + i]; // correct offset.
    }
}


void Chip8::Cycle() {
    instruction = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;

    uint8_t tableIdx = (instruction & 0xF000u) >> 12u;
    //((*this).*(table[(instruction & 0xF000u) >> 12u]))();
    //(this->*(table[(instruction & 0xF000u) >> 12u]))();
    (this->*(table[tableIdx]))();

    if (delayTimer > 0) {
        --delayTimer;
    }
    if (soundTimer > 0) {
        --soundTimer;
    }
}