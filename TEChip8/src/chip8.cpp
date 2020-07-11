#include "chip8.h"

#include <fstream>
#include <string>

#include "screen_size.h"

const unsigned int FONT_START = 0x50;
const unsigned int ROM_START = 0x200;

struct FileBin {
	char* buffer{};
	std::streampos size;
};

FileBin getFileBin(const char* path) {
	printf("Opening file %s\n", path);
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		printf("Error opening file %s\n", path);
		exit(1);
	}

	FileBin binary;
	binary.size = file.tellg();
	binary.buffer = new char[binary.size];

	file.seekg(0, std::ios::beg);
	file.read(binary.buffer, binary.size);
	file.close();
	return binary;
}

Chip8::Chip8()
	: randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	//Initialize program Counter
	pc = ROM_START;

	FileBin fontset = getFileBin("fontset");
	for (unsigned int i = 0; i < fontset.size; i++) {
		memory[FONT_START + i] = fontset.buffer[i];
	}

	//Initialise RNG
	randByte = std::uniform_int_distribution<int>(0, 255U);

	// Set up function pointer table
	table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;

	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;

	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

	tableF[0x07] = &Chip8::OP_Fx07;
	tableF[0x0A] = &Chip8::OP_Fx0A;
	tableF[0x15] = &Chip8::OP_Fx15;
	tableF[0x18] = &Chip8::OP_Fx18;
	tableF[0x1E] = &Chip8::OP_Fx1E;
	tableF[0x29] = &Chip8::OP_Fx29;
	tableF[0x33] = &Chip8::OP_Fx33;
	tableF[0x55] = &Chip8::OP_Fx55;
	tableF[0x65] = &Chip8::OP_Fx65;
}

Chip8::~Chip8()
{
}

void Chip8::loadGame(const char* rom)
{
	std::string path = "roms/";
	path += rom;
	FileBin romBin = getFileBin(path.c_str());
	
	for (long i = 0; i < romBin.size; i++) {
		memory[ROM_START + i] = romBin.buffer[i];
	}

	delete[] romBin.buffer;
}

void Chip8::update() {
	// Fetch
	opcode = (memory[pc] << 8u) | memory[pc + 1];
	printf("0x%X\n", opcode);

	// Increment the PC before we execute anything
	pc += 2;

	// Decode and Execute
	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

	// Decrement the delay timer if it's been set
	if (delay_timer > 0)
	{
		--delay_timer;
	}

	// Decrement the sound timer if it's been set
	if (sound_timer > 0)
	{
		--sound_timer;
	}
}

void Chip8::OP_00E0()
{
	printf("CLS");
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE() // CLS
{
	--stack_ptr;
	pc = stack[stack_ptr];
}

void Chip8::OP_1nnn() // RET
{
	uint16_t address = opcode & 0x0FFFu;
	pc = address;
}

void Chip8::OP_2nnn() // JP addr
{
	uint16_t address = opcode & 0x0FFFu;

	stack[stack_ptr] = pc;
	++stack_ptr;
	pc = address;
}

void Chip8::OP_3xkk() // SE Vx, byte
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] == byte)
	{
		pc += 2;
	}
}

void Chip8::OP_4xkk() // SNE Vx, byte
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] != byte)
	{
		pc += 2;
	}
}

void Chip8::OP_5xy0() // SE Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_6xkk() // LD Vx, byte
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;
}

void Chip8::OP_7xkk() // ADD Vx, byte
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

void Chip8::OP_8xy0() // LD Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1() // OR Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2() // AND Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3() // XOR Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4() // ADD Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5() // SUB Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6() // SHR Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

void Chip8::OP_8xy7() // SUBN Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE() // SHR Vx{, Vy}
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

void Chip8::OP_9xy0() // SNE Vx, Vy
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_Annn() // LD index, addr
{
	uint16_t address = opcode & 0x0FFFu;

	index = address;
}

void Chip8::OP_Bnnn() // JP V0, addr
{
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}

void Chip8::OP_Cxkk() // RND Vx, byte
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn() // DRW Vx, Vy, nibble
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_Ex9E() // SKP Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t k = registers[Vx];

	if (key[k])
	{
		pc += 2;
	}
}

void Chip8::OP_ExA1() // SKNP Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t k = registers[Vx];

	if (!key[k])
	{
		pc += 2;
	}
}

void Chip8::OP_Fx07() // LD Vx, DT
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delay_timer;
}

void Chip8::OP_Fx0A() // LD Vx, K
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (key[0])
	{
		registers[Vx] = 0;
	}
	else if (key[1])
	{
		registers[Vx] = 1;
	}
	else if (key[2])
	{
		registers[Vx] = 2;
	}
	else if (key[3])
	{
		registers[Vx] = 3;
	}
	else if (key[4])
	{
		registers[Vx] = 4;
	}
	else if (key[5])
	{
		registers[Vx] = 5;
	}
	else if (key[6])
	{
		registers[Vx] = 6;
	}
	else if (key[7])
	{
		registers[Vx] = 7;
	}
	else if (key[8])
	{
		registers[Vx] = 8;
	}
	else if (key[9])
	{
		registers[Vx] = 9;
	}
	else if (key[10])
	{
		registers[Vx] = 10;
	}
	else if (key[11])
	{
		registers[Vx] = 11;
	}
	else if (key[12])
	{
		registers[Vx] = 12;
	}
	else if (key[13])
	{
		registers[Vx] = 13;
	}
	else if (key[14])
	{
		registers[Vx] = 14;
	}
	else if (key[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx15() // LD DT, Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delay_timer = registers[Vx];
}

void Chip8::OP_Fx18() // LD ST, Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	sound_timer = registers[Vx];
}

void Chip8::OP_Fx1E() // ADD index, Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	index += registers[Vx];
}

void Chip8::OP_Fx29() // LD F, Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];

	index = FONT_START + (5 * digit);
}

void Chip8::OP_Fx33() // LD B, Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index] = value % 10;
}

void Chip8::OP_Fx55() // LD [index], Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}

void Chip8::OP_Fx65() // LD Vx, [index]
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
}
