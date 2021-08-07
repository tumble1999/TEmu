#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <cstdint>
#include <chrono>
#include <random>

class Chip8 {
public:
	// 35 opcodes
	uint16_t opcode;
	// 4K memory
	uint8_t memory[4096]{};

	//Registers
	uint8_t registers[16]{}; // 15 8-bit General Purpose Registers
	uint16_t index{}; // Index Register
	uint16_t pc{}; // Porgram Counter

	/*
	System Memory map
	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
	*/

	//Graphics
	//Screen 2048 px(64px * 32px)
	uint32_t video[64 * 32]{};

	//Timer Registers
	uint8_t delay_timer{};
	uint8_t sound_timer{}; // Buzz when timer reachers 0

	//16 levels of stack
	uint16_t stack[16]{};
	uint16_t stack_ptr{}; // Stack Pointer

	//Hex Based Keyboard
	uint8_t key[16]{};

	//Random
	std::default_random_engine randGen;
	std::uniform_int_distribution<int> randByte{ 0, std::numeric_limits<uint8_t>::max() };

	typedef void (Chip8::* Chip8Func)();
	Chip8Func table[0xF + 1]{ &Chip8::OP_NULL };
	Chip8Func table0[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func table8[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func tableE[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func tableF[0x65 + 1]{ &Chip8::OP_NULL };

	Chip8();
	~Chip8();

	void loadGame(const char* rom);
	void update();

	void Table0() { ((*this).*(table0[opcode & 0x000Fu]))(); }
	void Table8() { ((*this).*(table8[opcode & 0x000Fu]))(); }
	void TableE() { ((*this).*(tableE[opcode & 0x000Fu]))(); }
	void TableF() { ((*this).*(tableF[opcode & 0x00FFu]))(); }

	//Instructions
	void OP_NULL() {}
	void OP_00E0(); // CLS
	void OP_00EE(); // RET
	void OP_1nnn(); // JP addr
	void OP_2nnn(); // CALL addr
	void OP_3xkk(); // SE Vx, byte
	void OP_4xkk(); // SNE Vx, byte
	void OP_5xy0(); // SE Vx, Vy
	void OP_6xkk(); // LD Vx, byte
	void OP_7xkk(); // ADD Vx, byte
	void OP_8xy0(); // LD Vx, Vy
	void OP_8xy1(); // OR Vx, Vy
	void OP_8xy2(); // AND Vx, Vy
	void OP_8xy3(); // XOR Vx, Vy
	void OP_8xy4(); // ADD Vx, Vy
	void OP_8xy5(); // SUB Vx, Vy
	void OP_8xy6(); // SHR Vx
	void OP_8xy7(); // SUBN Vx, Vy
	void OP_8xyE(); // SHR Vx{, Vy}
	void OP_9xy0(); // SNE Vx, Vy
	void OP_Annn(); // LD index, addr
	void OP_Bnnn(); // JP V0, addr
	void OP_Cxkk(); // RND Vx, byte
	void OP_Dxyn(); // DRW Vx, Vy, nibble
	void OP_Ex9E(); // SKP Vx
	void OP_ExA1(); // SKNP Vx
	void OP_Fx07(); // LD Vx, DT
	void OP_Fx0A(); // LD Vx, K
	void OP_Fx15(); // LD DT, Vx
	void OP_Fx18(); // LD ST, Vx
	void OP_Fx1E(); // ADD index, Vx
	void OP_Fx29(); // LD F, Vx
	void OP_Fx33(); // LD B, Vx
	void OP_Fx55(); // LD [index], Vx
	void OP_Fx65(); // LD Vx, [index]
};

#endif // !_CHIP8_H_

