#include "Base_Classes.h"

#include "Loading.h"
#include "Subpersons.h"
#include "Obstacles.h"
#include <iostream>

std::deque<Object*> objects, to_delete;

//Object

Object::Object(int x, int y, int hitbox, std::string s)
{
    pos[0] = x;
    pos[1] = y;

    rotation = 0;
    iframes=0;

    obstacle_hitbox = hitbox_size = hitbox;

    if (s.empty())
    {
        tex = nullptr;
        render_size[0] = render_size[1] = 0;
    }
    else
    {
        tex = load_image(s);
        //SDL_SetTextureColorMod(tex,255,0,0);
        itex = load_image(s+"_white");
        SDL_QueryTexture(tex, nullptr, nullptr, &render_size[0], &render_size[1]);
    }

    rotate_center = {render_size[0]/2,render_size[1]/2};

    objects.push_back(this);
}

Object::~Object()
{
    remove_it(&objects, this);
}

void Object::render()
{
    SDL_Rect dest={pos[0]-render_size[0]/2-camera[0], pos[1]-render_size[1]/2-camera[1], render_size[0], render_size[1]},
             src ={get_anim_type()*render_size[0],get_anim_frame()*render_size[1],render_size[0], render_size[1]};

    SDL_RenderCopyEx(renderer, iframes>0?itex:tex, &src, &dest, rotation, &rotate_center, SDL_FLIP_NONE);
}

bool Object::collides(Object* with)
{
    return (std::pow(pos[0]-with->pos[0],2) + std::pow(pos[1]-with->pos[1],2) < std::pow(hitbox_size+with->hitbox_size,2));
}

int Object::get_anim_frame()
{
    return 0;
}

int Object::get_anim_type()
{
    return 0;
}

//Person

Person::Person(int x, int y, int hitbox, int life, int cooldown, std::string s) : Object(x, y, hitbox, s)
{
    lifepower = life;
    bullet_size=5;
    bullet_range=10;

    cur_cooldown = max_cooldown = cooldown;

    life_draining = true;
    is_player = false;
}

void Person::attack(Person* attacker)
{
    if (iframes<=0)
    {
        lifepower -= 10;
        if (attacker->life_draining) attacker->lifepower += 5;

        if (lifepower < 0) kill();
        std::cout << "ow" << lifepower << "\n";

        iframes = 10;
    }
}

//Base_bullet

Base_bullet::Base_bullet(Person* shooter) : Object(shooter->pos[0], shooter->pos[1], shooter->bullet_size, "")
{
    shot_by = shooter;
    remove_on_impact = true;
    instant_kill = false;

    enemy = !shot_by->is_player;
    lifetime = shot_by->bullet_range;

    accurate_pos[0] = pos[0];
    accurate_pos[1] = pos[1];
}

bool Base_bullet::check_hit(Person* p)
{
    if (collides(p))
    {
        if (instant_kill) p->kill();
        else p->attack(shot_by);

        if(remove_on_impact)
        {
            to_delete.push_back(this);
            return true;
        }
    }

    return false;
}

void Base_bullet::update()
{
    move();

    pos[0] = accurate_pos[0];
    pos[1] = accurate_pos[1];

    if (remove_on_impact)
    {
        for (Obstacle* o: obstacles)
        {
            if (o->collides(this)) to_delete.push_back(this);
        }
    }

    bool stop = false;
    for (Person* f: friends)
    {
        stop = check_hit(f);
        if (stop) break;
    }

    if (!stop)
    {
        if (enemy) check_hit(player);
        else
        {
            for (Person* e: enemies)
            {
                if (check_hit(e)) break;
            }
        }
    }

    lifetime--;
    if (lifetime <= 0) to_delete.push_back(this);
}
