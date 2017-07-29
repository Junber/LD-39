#include "Obstacles.h"

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
    int dx = o->pos[0]-pos[0], dy = o->pos[1]-pos[1], sum = abs(dx)+abs(dy);

    o->pos[0] = pos[0]+(obstacle_hitbox+o->obstacle_hitbox)*dx/sum;
    o->pos[1] = pos[1]+(obstacle_hitbox+o->obstacle_hitbox)*dy/sum;
}
