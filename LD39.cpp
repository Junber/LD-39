#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <iostream>

#include "Loading.h"
#include "Base_Classes.h"
#include "Subpersons.h"
#include "Obstacles.h"

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);

    init_window();
    SDL_RenderSetScale(renderer,zoom,zoom);

    player = new Player();

    new Obstacle(50,50,5,"Enemy");
    new Enemy(100,50,5,100,60,"Enemy");
    new Enemy(50,300,5,100,60,"Enemy");
    new NPC(100,60,5,"Enemy");

    //SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) breakk = true;

			#ifdef DEBUG
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F1) player->kill();
			#endif // DEBUG

			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                player->shoot(e.button.x/zoom, e.button.y/zoom);
            }
        }

        bool bad_vision = (player->age >= cane && player->lifepower < 25);

        SDL_SetRenderDrawColor(renderer,155,155,155,255);
        if (!bad_vision) SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            o->update();
            o->render();
        }

        if (bad_vision)
        {
            SDL_SetRenderDrawColor(renderer,100,100,100,10);
            SDL_Rect r = {0,0,window[0],window[1]};
            SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer,&r);
        }

        for (Object* o: to_delete) delete o;
        to_delete.clear();

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
