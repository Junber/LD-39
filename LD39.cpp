#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <random>
#include <time.h>

#include "Loading.h"
#include "Base_Classes.h"
#include "Subpersons.h"
#include "Obstacles.h"

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

std::default_random_engine generator;
int random(int x, int y)
{
    std::uniform_int_distribution<int> distribution(x,y);
    return distribution(generator);
}

void random_init()
{
    generator.seed(time(nullptr));
    random(0,1);
}

Enemy_type* random_enemy_type()
{
    Enemy_type* e = new Enemy_type();
    e->bullet_range = random(50,300);
    e->bullet_size = random(5,30);
    e->bullet_speed = random(1,5);
    e->cooldown = random(30,60);
    e->life = random(10,40);
    e->movement = static_cast<Movements>(random(0,MOVEMENTS_NUM-1));
    e->weapon = reverse_circlegun;//static_cast<Weapons>(random(0,WEAPON_NUM-1));
    e->speed = 2;//random(2,10);

    return e;
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);
    random_init();

    init_window();
    SDL_RenderSetScale(renderer,zoom,zoom);

    player = new Player();

    new Obstacle(50,50,25,"House");
    for (int i=0; i<5; i++) new Enemy(random(50,window[0]),random(50,window[1]),5,"Enemy",random_enemy_type());
    new NPC(200,200,5,"NPC");

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
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F2) show_hitbox = !show_hitbox;
			#endif // DEBUG

			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                player->shoot(e.button.x/zoom+camera[0], e.button.y/zoom+camera[1]);
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
            SDL_SetRenderDrawColor(renderer,100,100,100,player->lifepower*5);
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
