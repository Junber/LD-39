#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <string>

extern SDL_Renderer* renderer;

SDL_Texture* load_image(std::string s);
void init_window();

#endif // _LOADING
