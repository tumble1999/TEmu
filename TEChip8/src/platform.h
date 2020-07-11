#include <SDL\SDL_video.h>
#include <SDL\SDL_render.h>
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

class Platform {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
public:
	Platform(const char* title, int width, int height, int textureWidth, int textureHeight);
	~Platform();
	bool input(uint8_t* keys);
	void update(const void* buffer, int pitch);
};

#endif // !_PLATFORM_H_
