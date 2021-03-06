#ifndef _LOADING
#define _LOADING

#include <SDL.h>
#include <string>
#include <SDL_mixer.h>

extern SDL_Renderer* renderer;
extern bool breakk, fullscreen, vsync, screen_shake_enabled;
extern int camera[2], zoom, sfx_volume, music_volume, screen_shake;
extern SDL_Texture* bg;

namespace transition
{
    extern bool transition;
    extern SDL_Texture* tex;
    extern int time, size, h;
}

const int window[2] = {960,540};
const int map_size[2] = {1000,1000};

SDL_Texture* load_image(std::string s);
SDL_Texture* load_bullet_image(int size);
SDL_Texture* make_background();
void remake_target_textures();

Mix_Chunk* load_sound(std::string s);
Mix_Music* load_music(std::string s);

void sound_init();
void play_sound(Mix_Chunk* s, int selfcut=0);
void play_music(Mix_Music* s);
void set_loop(Mix_Music* s);

void init_window();
void limit_fps();
int random(int x, int y);
void show_image(SDL_Texture* tex);
void help_screen();
//int sign(int x);

#endif // _LOADING
