#include "Ammo_Drops.h"

#include "Subpersons.h"

Ammo::Ammo(int x, int y) : Object(x,y,15,"Ammo")
{

}

void Ammo::update()
{
    if (player->collides(this))
    {
        player->ammo += 6;
        to_delete.push_back(this);
    }
}
