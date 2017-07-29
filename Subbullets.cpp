#include "Subbullets.h"

#include "Subpersons.h"
#include "Loading.h"
#include "SDL2_gfxPrimitives.h"


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

Melee::Melee(Person* shooter) : Base_bullet(shooter) {}

void Melee::move()
{
    accurate_pos[0] = shot_by->pos[0];
    accurate_pos[1] = shot_by->pos[1];
}

void Melee::render()
{
    circleRGBA(renderer,pos[0],pos[1],hitbox_size,255,255,255,255);
}
