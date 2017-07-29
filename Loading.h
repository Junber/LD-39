#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <string>

extern SDL_Renderer* renderer;
extern bool breakk;
const int zoom = 2;
const int window[2] = {500,500};

SDL_Texture* load_image(std::string s);
void init_window();

#endif // _LOADING
