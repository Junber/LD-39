#include "Subbullets.h"

#include "Subpersons.h"
#include "Loading.h"
#include "SDL2_gfxPrimitives.h"
#include <iostream>


//Bullet

Bullet::Bullet(Person* shooter, float s_x, float s_y) : Base_bullet(shooter)
{
    speed[0] = s_x;
    speed[1] = s_y;
}

void Bullet::move()
{
    accurate_pos[0] += speed[0];
    accurate_pos[1] += speed[1];
}

void Bullet::render()
{
    filledCircleRGBA(renderer,pos[0],pos[1],hitbox_size,enemy*255,(!enemy)*255,0,255);
    circleRGBA(renderer,pos[0],pos[1],hitbox_size,0,0,0,255);
}

//Melee

Melee::Melee(Person* shooter) : Base_bullet(shooter)
{
    remove_on_impact=false;
}

void Melee::move()
{
    accurate_pos[0] = shot_by->pos[0];
    accurate_pos[1] = shot_by->pos[1];
}

void Melee::render()
{
    circleRGBA(renderer,pos[0],pos[1],hitbox_size,255,255,255,255);
}

//Laser

Laser::Laser(Person* shooter, int end_x, int end_y) : Base_bullet(shooter)
{
    end[0] = end_x;
    end[1] = end_y;
    remove_on_impact=false;
}

void Laser::render()
{
    SDL_SetRenderDrawColor(renderer,enemy*255,(!enemy)*255,0,255);
    SDL_RenderDrawLine(renderer, pos[0], pos[1], end[0], end[1]);
}

bool Laser::collides(Object* o)
{
    int a =  std::pow(pos[0]-end[0],2)         +  std::pow(pos[1]-end[1],2), //make constant if lasers dont move
    b = 2* ((pos[0]-end[0])*(pos[0]-o->pos[0]) + (pos[1]-end[1])*(pos[1]-o->pos[1])),
    c =      std::pow(pos[0]-o->pos[0],2)      +  std::pow(pos[1]-o->pos[1],2) - std::pow(o->hitbox_size,2); //add hitbox_size to o->hitbox_size if laser is thicker

    if ((b*b-4*a*c) >= 0) std::cout << o->pos[0] << " " << o->pos[1] << "\n";
    return (b*b-4*a*c) >= 0;
}

//Shockwave

Shockwave::Shockwave(Person* shooter) : Base_bullet(shooter)
{
    remove_on_impact=false;
    instant_kill=true;
}

void Shockwave::render()
{
    circleRGBA(renderer,pos[0],pos[1],hitbox_size,255,255,255,255);
}

void Shockwave::move()
{
    hitbox_size += 5;
    lifetime = 10; //arbitrary number; makes shockwave last forever
}
