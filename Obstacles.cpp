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
    return (std::pow(pos[0]-with->pos[0],2) + std::pow(pos[1]-with->pos[1],2) < std::pow(obstacle_hitbox+with->obstacle_hitbox,2));
}
