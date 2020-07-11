#include <chrono>
#include <iostream>

#include <SDL/SDL.h>

#include "screen_size.h"
#include "platform.h"
#include "chip8.h"



int main(int argc, char* args[]) {
	std::cout << "Hello" << std::endl;

	int delay = 1;
	int scale = 10;

	Platform platform("TEmu - TEChip8", scale*VIDEO_WIDTH, scale*VIDEO_HEIGHT, VIDEO_WIDTH, VIDEO_HEIGHT);
	Chip8 chip8;
	chip8.loadGame("pong");

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