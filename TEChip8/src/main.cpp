#include <chrono>
#include <iostream>

#include <SDL/SDL.h>
#include <TEmu\Platform.h>

#include "screen_size.h"
#include "chip8.h"

int main(int argc, char* args[]) {
	std::cout << "Starting TEmu Chip8 Emulator" << std::endl;

	int delay = 1;
	int scale = 10;

	TEmu::Platform platform("TEmu - TEChip8", VIDEO_WIDTH, VIDEO_HEIGHT,scale);
	Chip8 chip8;
	chip8.loadGame("PONG");

	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.input(chip8.key);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > delay)
		{
			lastCycleTime = currentTime;
			chip8.update();
			platform.update(chip8.video, videoPitch);
		}
	}
	std::cin.get();
	return 0;
}