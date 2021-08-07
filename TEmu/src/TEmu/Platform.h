#ifndef _TE_PLATFORM_H_
#define _TE_PLATFORM_H_

#include <SDL\SDL_video.h>
#include <SDL\SDL_render.h>

namespace TEmu {
	class Platform {
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
	public:
		Platform(const char* title, int width, int height, int textureScale);
		~Platform();
		bool input(uint8_t* keys);
		void update(const void* buffer, int pitch);

	};
}

#endif // !_TE_PLATFORM_H_
