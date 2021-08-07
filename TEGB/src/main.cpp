#include <chrono>
#include <iostream>

#include <SDL/SDL.h>
#include <TEmu/Platform.h>
#include "screen_size.h"

int main(int argc, char* args[]) {
	std::cout << "Starting TEmu Game Boy Emulator" << std::endl;

	int scale = 1;

	TEmu::Platform platform("TEmu - TEGB - GameBoy",VIDEO_WIDTH,VIDEO_HEIGHT,scale);

	std::cin.get();
	return 0;
}