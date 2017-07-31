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

std::deque<Enemy_type*> enemy_types;
void gen_enemy_types()
{
    { //circlelover, circlegunner, better soldier
        Enemy_type* e = new Enemy_type(); //circlelover
        e->bullet_range = 150;
        e->bullet_size = 5;
        e->bullet_speed = 2;
        e->cooldown = 90;
        e->life = 20;
        e->movement = circle_player;
        e->weapon = reverse_circlegun;
        e->speed = 2;
        e->texture = "alien_grey";

        Enemy_type* f = new Enemy_type(); //circlegunner
        f->bullet_range = 250;
        f->bullet_size = 5;
        f->bullet_speed = 2;
        f->cooldown = 80;
        f->life = 20;
        f->movement = circle_player;
        f->weapon = circlegun;
        f->speed = 3;
        f->texture = "alien_brown";

        Enemy_type* g = new Enemy_type(); //better soldier
        g->bullet_range = 250;
        g->bullet_size = 15;
        g->bullet_speed = 3;
        g->cooldown = 50;
        g->life = 20;
        g->movement = keep_distance_from_player;
        g->weapon = smart_pistol;
        g->speed = 5;
        g->texture = "alien_green";

        f->younger = e;
        e->older = f;

        g->younger = f;
        f->older = g;

        enemy_types.push_back(e);
        enemy_types.push_back(f);
        enemy_types.push_back(g);
    }

    { //runner, worse soldier, sniper
        Enemy_type* e = new Enemy_type(); //runner
        e->cooldown = 50;
        e->life = 10;
        e->movement = walk_towards_player;
        e->weapon = melee;
        e->speed = 5;
        e->texture = "alien_red";

        Enemy_type* f = new Enemy_type(); //worse soldier
        f->bullet_range = 250;
        f->bullet_size = 10;
        f->bullet_speed = 4;
        f->cooldown = 40;
        f->life = 20;
        f->movement = keep_distance_from_player;
        f->weapon = alien_pistol;
        f->speed = 2;
        f->texture = "alien_orange";

        Enemy_type* g = new Enemy_type(); //sniper
        g->bullet_range = 500;
        g->bullet_size = 20;
        g->bullet_speed = 7;
        g->cooldown = 80;
        g->life = 10;
        g->movement = none;
        g->weapon = smart_pistol;
        g->texture = "alien_yellow";

        f->younger = e;
        e->older = f;

        g->younger = f;
        f->older = g;

        enemy_types.push_back(e);
        enemy_types.push_back(f);
        enemy_types.push_back(g);
    }
}

Enemy_type* random_enemy_type()
{
    return enemy_types[random(0,enemy_types.size()-1)];
}

int main(int argc, char* args[])
{
    SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);
    random_init();

    init_window();
    SDL_RenderSetScale(renderer,zoom,zoom);
    bg = make_background();
    gen_enemy_types();

    transition::transition = false;

    player = new Player();

    new Obstacle(100,100,64,"house"+std::to_string(random(1,4)));
    for (int i=0;i<=10;i++) new Enemy(random(50,map_size[0]),random(50,map_size[1]),random_enemy_type());
    for (int i=0;i<=15;i++) new NPC(random(50,map_size[0]),random(50,map_size[1]),5,"npc_"+std::to_string(random(1,3)));

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

        camera[0] = std::max(0, std::min(map_size[0]-window[0], player->pos[0]-window[0]/2));
        camera[1] = std::max(0, std::min(map_size[1]-window[1], player->pos[1]-window[1]/2));

        SDL_Rect r = {camera[0],camera[1],window[0],window[1]};
        SDL_RenderCopy(renderer,bg,&r,nullptr);

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

        for (Object* o: to_delete) delete o;
        to_delete.clear();

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
