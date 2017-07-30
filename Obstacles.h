#ifndef _OBSTACLES
#define _OBSTACLES

#include "Base_Classes.h"

class Obstacle : public Object
{
public:
    Obstacle(int x, int y, int hitbox, std::string s);
    ~Obstacle();
    void push_away(Object* o);
    bool collides(Object* with);
};

extern std::deque<Obstacle*> obstacles;

#endif // _OBSTACLES
