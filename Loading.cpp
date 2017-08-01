#include "Loading.h"

#include <SDL_image.h>
#include <map>
#include <SDL2_gfxPrimitives.h>
#include <iostream>

SDL_Window* renderwindow;
SDL_Renderer* renderer;
bool breakk = false, fullscreen=true, vsync=false, screen_shake_enabled=true;
int camera[2] = {0,0}, zoom=2, sfx_volume=64, music_volume=64, screen_shake=0;
SDL_Texture* bg;

namespace transition
{
    bool transition;
    SDL_Texture* tex;
    int time, size, h;
}

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s))
    {
        loaded_textures[s] = IMG_LoadTexture(renderer,("Data\\Graphics\\"+s+".png").c_str());
        Uint32 format;
        int w, h;
        SDL_QueryTexture(loaded_textures[s],&format,nullptr,&w,&h);
        loaded_textures[s+"_white"] = SDL_CreateTexture(renderer,format,SDL_TEXTUREACCESS_TARGET,w,h);

        SDL_SetRenderTarget(renderer,loaded_textures[s+"_white"]);
        SDL_SetTextureBlendMode(loaded_textures[s+"_white"], SDL_BLENDMODE_NONE);
        SDL_RenderCopy(renderer,loaded_textures[s],nullptr,nullptr);

        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_Rect r = {0,0,w,h};
        SDL_RenderFillRect(renderer,&r);

        SDL_SetRenderTarget(renderer,nullptr);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(loaded_textures[s+"_white"], SDL_BLENDMODE_BLEND);
    }

    return loaded_textures[s];
}

std::map<int,SDL_Texture*> bullet_textures;
SDL_Texture* load_bullet_image(int size)
{
    if (!bullet_textures.count(size))
    {
        bullet_textures[size] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,size*2+1,size*2+1);

        SDL_SetRenderTarget(renderer,bullet_textures[size]);
        SDL_SetTextureBlendMode(bullet_textures[size], SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_Rect r = {0,0,size*2+1,size*2+1};
        SDL_RenderFillRect(renderer,&r);

        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(bullet_textures[size], SDL_BLENDMODE_BLEND);

        filledCircleRGBA(renderer,size,size,size,255,0,0,255);
        circleRGBA(renderer,size,size,size,0,0,0,255);

        SDL_SetRenderTarget(renderer,nullptr);
    }

    return bullet_textures[size];
}

void remake_target_textures()
{
    for (auto a: loaded_textures)
    {
        if (a.first.size() >= 6 && a.first.substr(a.first.size()-6,6) == "_white")
        {
            int w, h;
            SDL_QueryTexture(a.second,nullptr,nullptr,&w,&h);

            SDL_SetRenderTarget(renderer,a.second);
            SDL_SetTextureBlendMode(a.second, SDL_BLENDMODE_NONE);
            SDL_RenderCopy(renderer,loaded_textures[a.first.substr(0,a.first.size()-6)],nullptr,nullptr);

            SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_ADD);
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
            SDL_Rect r = {0,0,w,h};
            SDL_RenderFillRect(renderer,&r);

            SDL_SetRenderTarget(renderer,nullptr);
            SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
            SDL_SetTextureBlendMode(a.second, SDL_BLENDMODE_BLEND);
        }
    }

    for (auto a: bullet_textures)
    {
        int size = a.first;

        SDL_SetRenderTarget(renderer,bullet_textures[size]);
        SDL_SetTextureBlendMode(bullet_textures[size], SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_Rect r = {0,0,size*2+1,size*2+1};
        SDL_RenderFillRect(renderer,&r);

        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(bullet_textures[size], SDL_BLENDMODE_BLEND);

        filledCircleRGBA(renderer,size,size,size,255,0,0,255);
        circleRGBA(renderer,size,size,size,0,0,0,255);

        SDL_SetRenderTarget(renderer,nullptr);
    }
}

SDL_Texture* make_background()
{
    SDL_Texture* bg = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,map_size[0],map_size[1]);

    for (int i=1;i<=3;i++) load_image("dirt"+std::to_string(i));

    SDL_SetRenderTarget(renderer,bg);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(bg, SDL_BLENDMODE_BLEND);

    for (int i=0; i<map_size[0]; i+=64)
    {
        for (int u=0; u<map_size[1]; u+=64)
        {
            SDL_Rect r = {i,u,64,64};
            SDL_RenderCopy(renderer,load_image("dirt"+std::to_string(random(1,3))),nullptr,&r);
        }
    }

    SDL_SetRenderTarget(renderer,nullptr);

    return bg;
}

void init_window()
{
    renderwindow = SDL_CreateWindow("LD-39", 50, 30, window[0]*zoom, window[1]*zoom, SDL_WINDOW_SHOWN | (fullscreen*SDL_WINDOW_FULLSCREEN));
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | (vsync*SDL_RENDERER_PRESENTVSYNC));
}

void show_image(SDL_Texture* tex)
{
    SDL_Event e;
    int frames_passed = 0;
	while (true)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT)
			{
			    breakk = true;
			    return;
			}
			else if (e.type == SDL_KEYDOWN)
            {
			    if (e.key.keysym.sym == SDLK_ESCAPE)
			    {
			        breakk = true;
			        return;
			    }
			    if (frames_passed > 30) return;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN && frames_passed > 30) return;
			else if (e.type == SDL_RENDER_TARGETS_RESET)
            {
                bg = make_background();
                remake_target_textures();
            }
        }

        SDL_RenderCopy(renderer,tex,nullptr,nullptr);
        SDL_RenderPresent(renderer);
        frames_passed++;

        limit_fps();
    }
}

void help_screen()
{
    SDL_Event e;
    int frames_passed = 0;
	while (true)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT)
			{
			    breakk = true;
			    return;
			}
			else if (e.type == SDL_KEYDOWN)
            {
			    if (e.key.keysym.sym == SDLK_ESCAPE)
			    {
			        breakk = true;
			        return;
			    }
			    if (frames_passed > 30) return;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN && frames_passed > 30) return;
            else if (e.type == SDL_RENDER_TARGETS_RESET)
            {
                bg = make_background();
                remake_target_textures();
            }
        }

        SDL_RenderCopy(renderer,load_image("help"),nullptr,nullptr);

        SDL_Rect dest = {20,90,34,34},
                src = {34,34*((frames_passed/10)%8),34,34};
        SDL_RenderCopy(renderer,load_image("alien_yellow"),&src,&dest);

        dest.y = 160;
        SDL_RenderCopy(renderer,load_image("alien_orange"),&src,&dest);

        dest.y = 220;
        SDL_RenderCopy(renderer,load_image("alien_red"),&src,&dest);

        dest.y = 325;
        SDL_RenderCopy(renderer,load_image("alien_green"),&src,&dest);

        dest.y = 385;
        SDL_RenderCopy(renderer,load_image("alien_brown"),&src,&dest);

        dest.y = 440;
        SDL_RenderCopy(renderer,load_image("alien_grey"),&src,&dest);

        SDL_RenderPresent(renderer);
        frames_passed++;

        limit_fps();
    }
}

std::map<std::string,Mix_Chunk*> loaded_sounds;
Mix_Chunk* load_sound(std::string s)
{
    if (!loaded_sounds.count(s))
    {
        loaded_sounds[s] = Mix_LoadWAV(std::string("Data\\Sounds\\"+s+".wav").c_str());
        Mix_VolumeChunk(loaded_sounds[s], sfx_volume);
    }
    return loaded_sounds[s];
}

std::map<std::string,Mix_Music*> loaded_music;
Mix_Music* load_music(std::string s)
{
    if (!loaded_music.count(s)) loaded_music[s] = Mix_LoadMUS(std::string("Data\\Music\\"+s+".mp3").c_str());
    return loaded_music[s];
}

void sound_init()
{
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_VolumeMusic(music_volume);

    play_music(load_music("Lasershow Superhero"));
    set_loop(load_music("Lasershow Superhero (Loop)"));
}

int selfcut_channel[3]={-1,-1};
void play_sound(Mix_Chunk* s, int selfcut)
{
    selfcut--;

    if (s!=nullptr)
    {
        int channel = Mix_PlayChannel((selfcut&&(selfcut_channel[selfcut]!=-1))?selfcut_channel[selfcut]:-1, s, 0);

        for (int i=0;i<=2;i++)
        {
            if (i == selfcut) selfcut_channel[i] = channel;
            else if (channel == selfcut_channel[selfcut]) selfcut_channel[selfcut] = -1;
        }
    }
}

Mix_Music* looping;

void hook()
{
    Mix_PlayMusic(looping,-1);
}

void play_music(Mix_Music* s)
{
    if (s!= nullptr) Mix_PlayMusic(s,-1);
    Mix_HookMusicFinished(hook);
}

void set_loop(Mix_Music* s)
{
    looping = s;
}

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}
