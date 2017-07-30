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
    e->weapon = static_cast<Weapons>(random(0,WEAPON_NUM-1));
    e->speed = 2;//random(2,10);

    return e;
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);
    random_init();

    init_window();
    SDL_RenderSetScale(renderer,zoom,zoom);

    transition::transition = false;

    player = new Player();

    new Obstacle(50,50,25,"House");
    for (int i=0; i<5; i++) new Enemy(random(50,window[0]),random(50,window[1]),5,"alien_brown",random_enemy_type());
    new NPC(200,200,5,"NPC");

    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) breakk = true;

			#ifdef DEBUG
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F1) player->kill();
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F3 && player->age >= 2)
			{
			    player->age = static_cast<Ages>(player->age-2);
			    player->kill();
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F2) show_hitbox = !show_hitbox;
			#endif // DEBUG

			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                player->shoot(e.button.x/zoom+camera[0], e.button.y/zoom+camera[1]);
            }
        }

        bool bad_vision = (player->age >= cane && player->lifepower < 25);

        if (!bad_vision)
        {
            SDL_SetRenderDrawColor(renderer,155,155,155,255);
            SDL_RenderClear(renderer);
        }

        if (transition::transition)
        {
            for (Object* o: objects) o->render();
            SDL_SetRenderDrawColor(renderer,0,0,0,100);
            SDL_Rect r = {0,0,window[0],window[1]};
            SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer,&r);

            SDL_Rect src = {0,int(transition::time/15)*transition::size,transition::size,transition::size},
                    dest = {player->pos[0]-transition::size/2-camera[0], player->pos[1]-transition::size/2-camera[1], transition::size, transition::size};

            SDL_Point p;
            if (player->age == laser)
            {
                p = {17,18};
                dest.x += 1;
                dest.y += 1;
            }
            else if (player->age == life_drain)
            {
                p = {14,18};
                dest.x += 3;
                dest.y += 1;
            }
            else if (player->age == squaregun)
            {
                p = {12,35};
                dest.x += 12;
                dest.y -= 10;
            }
            else if (player->age == pistol)
            {
                p = {8,17};
                dest.x += 8;
                dest.y += 1;
            }
            else if (player->age == cane)
            {
                p = {10,17};
                dest.x += 6;
                dest.y += 1;
            }

            SDL_RenderCopyEx(renderer,transition::tex,&src,&dest,player->rotation,&p,SDL_FLIP_NONE);
            transition::time++;
            if (transition::time >= transition::h*15/transition::size) transition::transition = false;
        }
        else
        {
            for (Object* o: objects)
            {
                o->update();
                o->render();
            }
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
