#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <string>

extern SDL_Renderer* renderer;
extern bool breakk;

SDL_Texture* load_image(std::string s);
void init_window();

#endif // _LOADING
