#ifndef _TEGB_GB_H_
#define _TEGB_GB_H_

#include <cstdint>
#include <TEmu/VM.h>

class GB: public TEmu::VM<uint8_t> {
private:
	/*
	Memory Map
	0x0000-0x3FFF ROM BANK 0

	0xFE00-0xFE9F Sprite Attribute Table (OEM)
	0xC000-0xCFFF 4KB Work RAM
	0xFF00-0xFF7F I/O Rehisters
	0xFF80-0XFFFE High Ram (HRAM)
	0XFFFF        Interupts
	*/

	uint8_t video[0x2000]{}
public:
	GB();
	~GB();

	void loadGame(const char* rom);
};

#endif // _TEGB_GB_H_
