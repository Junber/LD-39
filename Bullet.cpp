#include "Bullet.h"

#include "Subpersons.h"
#include "Loading.h"
#include "SDL2_gfxPrimitives.h"

Bullet::Bullet(Person* shooter, int s_x, int s_y) : Object(shooter->pos[0], shooter->pos[1], shooter->bullet_size, "")
{
    shot_by = shooter;

    enemy = !shot_by->player;
    speed[0] = s_x;
    speed[1] = s_y;
}

void Bullet::update()
{
    pos[0] += speed[0];
    pos[1] += speed[1];

    if (enemy)
    {
        if (collides(player)) player->attack(shot_by);
    }
    else
    {
        for (Enemy* e: enemies)
        {
            if (collides(e)) e->attack(shot_by);
        }
    }
}

bool Bullet::collides(Object* with)
{
    return (std::pow(pos[0]-with->pos[0],2) + std::pow(pos[1]-with->pos[1],2) < std::pow(hitbox_size+with->hitbox_size,2));
}

void Bullet::render()
{
    filledCircleRGBA(renderer,pos[0],pos[1],hitbox_size,enemy*255,(!enemy)*255,0,255);
    circleRGBA(renderer,pos[0],pos[1],hitbox_size,0,0,0,255);
}
