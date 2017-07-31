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

class Laser: public Base_bullet
{
public:
    int end[2];

    Laser(Person* shooter, int end_x, int end_y);
    void render();
    bool collides(Object* o);
    void move();
    void hit_obstacle(Obstacle* o);
};

class Shockwave: public Base_bullet
{
public:
    Shockwave(Person* shooter);
    void render();
    void move();
};

#endif // _BULLET
