#include "Ammo_Drops.h"

#include "Subpersons.h"
#include "Loading.h"

Ammo::Ammo(int x, int y, int ammo_amount) : Object(x,y,15,"Ammo")
{
    render_size[0] = render_size[1] = 32;
    amount = ammo_amount;
}

void Ammo::update()
{
    if (player->age >= cane && !transition::transition) to_delete.push_back(this);

    if (player->collides(this))
    {
        player->ammo += amount;
        to_delete.push_back(this);
    }
}

int Ammo::get_anim_frame()
{
    return player->age-squaregun-transition::transition;
}
