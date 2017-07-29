#include "Base_Classes.h"
#include "Loading.h"

std::deque<Object*> objects;

//Object

Object::Object(int x, int y, int hitbox, std::string s)
{
    pos[0] = x;
    pos[1] = y;

    rotation = 0;

    hitbox_size = hitbox;

    if (s.empty())
    {
        tex = nullptr;
        render_size[0] = render_size[1] = 0;
    }
    else
    {
        tex = load_image(s);
        SDL_QueryTexture(tex, nullptr, nullptr, &render_size[0], &render_size[1]);
    }

    objects.push_back(this);
}

Object::~Object()
{
    remove_it(&objects, this);
}

void Object::render()
{
    SDL_Rect r={pos[0]-render_size[0]/2, pos[1]-render_size[1]/2, render_size[0], render_size[1]};

    SDL_RenderCopyEx(renderer, tex, nullptr, &r, rotation, nullptr, SDL_FLIP_NONE);
}

//Person

Person::Person(int x, int y, int hitbox, int life, std::string s) : Object(x, y, hitbox, s)
{
    lifepower = life;
    bullet_size=5;
}

void Person::attack(Person* attacker)
{
    lifepower -= 10;
    attacker->lifepower += 5;
}
