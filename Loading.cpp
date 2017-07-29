#include "Loading.h"

#include <SDL_image.h>
#include <map>

SDL_Window* renderwindow;
SDL_Renderer* renderer;
bool breakk = false;

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

void init_window()
{
    renderwindow = SDL_CreateWindow("LD-39", 50, 50, window[0]*zoom, window[1]*zoom, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}
