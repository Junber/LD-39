#ifndef _AMMO_DROPS
#define _AMMO_DROPS

#include "Base_Classes.h"

class Ammo: public Object
{
public:
    Ammo(int x, int y);
    void update();
    int get_anim_frame();
};

#endif // _AMMO_DROPS
