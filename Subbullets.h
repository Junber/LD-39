#ifndef _BULLET
#define _BULLET

#include "Base_Classes.h"

class Bullet: public Base_bullet
{
public:
    float speed[2];

    Bullet(Person* shooter, float s_x, float s_y);
    void move();
    void render();
};

class Melee: public Base_bullet
{
public:
    Melee(Person* shooter);
    void move();
    void render();
};

#endif // _BULLET
