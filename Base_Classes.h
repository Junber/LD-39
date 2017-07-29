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

class Object;
extern std::deque<Object*> objects;
class Object
{
public:
    int pos[2], render_size[2], hitbox_size;
    double rotation;
    SDL_Texture* tex;

    Object(int x, int y, int hitbox, std::string s);
    virtual ~Object();
    virtual void update() {};
    virtual void render();
};

class Person: public Object
{
public:
    int lifepower, bullet_size;
    bool player;

    Person(int x, int y, int hitbox, int life, std::string s);
    void attack(Person* attacker);
};

#endif // _BASE_CLASSES
