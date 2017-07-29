#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <iostream>

#include "Loading.h"
#include "Base_Classes.h"
#include "Subpersons.h"

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

bool breakk = false;

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

    Player* player;
    player = new Player();

    new Enemy(50,50,5,100,"Enemy");

    //SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) breakk = true;

			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                player->shoot(e.button.x, e.button.y);
            }
        }

        SDL_SetRenderDrawColor(renderer,155,155,155,255);
        SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            o->update();
            o->render();
        }

        for (Object* o: to_delete) delete o;
        to_delete.clear();

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
