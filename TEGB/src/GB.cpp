#include "GB.h"

#include <TEmu\Util.h>

GB::GB()
	:VM(0x2000,8)
{
}

GB::~GB()
{
}

void GB::loadGame(const char* rom)
{
	std::string path = "roms/";
	path += rom;
	TEmu::FileBin romBin = TEmu::GetFileBin(path.c_str());
	loadBuffer(0x0000, romBin.buffer, romBin.size);


}
