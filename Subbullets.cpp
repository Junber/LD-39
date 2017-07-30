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
    if (pos[0]-camera[0] < -hitbox_size || pos[0]-camera[0] > window[0]+hitbox_size ||
        pos[1]-camera[1] < -hitbox_size || pos[1]-camera[1] > window[1]+hitbox_size) return;

    filledCircleRGBA(renderer,pos[0]-camera[0],pos[1]-camera[1],hitbox_size,enemy*255,(!enemy)*255,0,255);
    circleRGBA(renderer,pos[0]-camera[0],pos[1]-camera[1],hitbox_size,0,0,0,255);
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
    circleRGBA(renderer,pos[0]-camera[0],pos[1]-camera[1],hitbox_size,255,255,255,255);
}

//Laser

Laser::Laser(Person* shooter, int end_x, int end_y) : Base_bullet(shooter)
{
    end[0] = end_x;
    end[1] = end_y;
    remove_on_impact=false;
    hitbox_size = 3;
}

void Laser::render()
{
    for (int i=2*hitbox_size+1; i>=1; i-=2) thickLineRGBA(renderer,pos[0]-camera[0], pos[1]-camera[1], end[0]-camera[0], end[1]-camera[1],i,enemy*255,(!enemy)*255,0,100);
}

bool Laser::collides(Object* o)
{
    const int diff[2] = {o->pos[0]-pos[0],o->pos[1]-pos[1]},
        n[2] = {-(pos[1]-end[1]), pos[0]-end[0]};
    const float len_n = std::sqrt(n[0]*n[0]+n[1]*n[1]),
        n0[2] = {n[0]/len_n,n[1]/len_n},
        d = diff[0]*n0[0]+diff[1]*n0[1];

    if (abs(d) <= o->hitbox_size+hitbox_size)
    {
        const int proj[2] = {int(o->pos[0]+d*n0[0]), int(o->pos[1]+d*n0[1])};
        return (proj[0]+o->hitbox_size >= std::min(pos[0],end[0]) && proj[0]-o->hitbox_size <= std::max(pos[0],end[0]) &&
                proj[1]+o->hitbox_size >= std::min(pos[1],end[1]) && proj[1]-o->hitbox_size <= std::max(pos[1],end[1]));
    }

    return false;
}

void Laser::move()
{
    accurate_pos[0] = shot_by->pos[0];
    accurate_pos[1] = shot_by->pos[1];
}

//Shockwave

Shockwave::Shockwave(Person* shooter) : Base_bullet(shooter)
{
    remove_on_impact=false;
    instant_kill=true;
}

void Shockwave::render()
{
    circleRGBA(renderer,pos[0]-camera[0],pos[1]-camera[1],hitbox_size,255,255,255,255);
}

void Shockwave::move()
{
    hitbox_size += 5;
    if (hitbox_size<= 1000) lifetime = 10; //arbitrary number; makes shockwave last forever
}
