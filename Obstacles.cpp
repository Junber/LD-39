#include "Obstacles.h"

#include <iostream>

std::deque<Obstacle*> obstacles;

Obstacle::Obstacle(int x, int y, int hitbox, std::string s) : Object(x,y,hitbox,s)
{
    obstacles.push_back(this);
}

Obstacle::~Obstacle()
{
    remove_it(&obstacles,this);
}

void Obstacle::push_away(Object* o)
{
    float dx = o->pos[0]-pos[0], dy = o->pos[1]-pos[1], dist = std::sqrt(dx*dx+dy*dy);

    o->pos[0] = pos[0]+(obstacle_hitbox+o->obstacle_hitbox)*dx/dist;
    o->pos[1] = pos[1]+(obstacle_hitbox+o->obstacle_hitbox)*dy/dist;
}

bool Obstacle::collides(Object* with)
{
    const int dx = abs(pos[0]-with->pos[0]),
            dy = abs(pos[1]-with->pos[1]),
            r = obstacle_hitbox+with->obstacle_hitbox;

    if (dx > r || dy > r) return false;
    return (dx*dx + dy*dy < r*r);
}
