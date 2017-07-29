#ifndef _BASE_CLASSES
#define _BASE_CLASSES

#include <deque>
#include <string>
#include <SDL.h>
#include <algorithm>

template<class t> void remove_it(std::deque<t>* base, t thing)
{
    base->erase( std::remove( std::begin(*base), std::end(*base), thing ), std::end(*base) );
}

class Object
{
public:
    int pos[2], render_size[2], hitbox_size;
    double rotation;
    SDL_Texture* tex;

    Object(int x, int y, int hitbox, std::string s);
    virtual bool collides(Object* with);
    virtual ~Object();
    virtual void update() {};
    virtual void render();
};

class Person: public Object
{
public:
    int lifepower, bullet_size, bullet_range;
    bool player;

    Person(int x, int y, int hitbox, int life, std::string s);
    void attack(Person* attacker);
    virtual void kill() {};
};

class Base_bullet: public Object
{
public:
    float accurate_pos[2];
    bool enemy;
    int lifetime;
    Person* shot_by;

    Base_bullet(Person* shooter);
    void update();
    virtual void move() {};
};

extern std::deque<Object*> objects, to_delete;
#endif // _BASE_CLASSES
