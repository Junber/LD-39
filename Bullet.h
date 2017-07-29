#ifndef _BULLET
#define _BULLET

#include "Base_Classes.h"

class Bullet: public Object
{
public:
    int speed[2];
    bool enemy;
    Person* shot_by;

    Bullet(Person* shooter, int s_x, int s_y);
    void update();
    bool collides(Object* with);
    void render();
};

#endif // _BULLET
