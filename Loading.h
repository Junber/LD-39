#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <string>

extern SDL_Renderer* renderer;
extern bool breakk;
extern int camera[2];
extern SDL_Texture* bg;

namespace transition
{
    extern bool transition;
    extern SDL_Texture* tex;
    extern int time, size, h;
}

const int zoom = 2;
const int window[2] = {500,500};
const int map_size[2] = {700,700};

SDL_Texture* load_image(std::string s);
SDL_Texture* load_bullet_image(int size);
SDL_Texture* make_background();
void init_window();
void limit_fps();
int random(int x, int y);

#endif // _LOADING
