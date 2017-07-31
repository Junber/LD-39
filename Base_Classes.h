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
    int pos[2], render_size[2], hitbox_size, obstacle_hitbox, iframes;
    bool deflectable_bullet;
    SDL_Point rotate_center;
    double rotation;
    SDL_Texture *tex, *itex;

    Object(int x, int y, int hitbox, std::string s);
    virtual bool collides(Object* with);
    virtual ~Object();
    virtual void update() {};
    virtual void render();
    virtual int get_anim_frame();
    virtual int get_anim_type();
    virtual void deflect() {};
};

class Person: public Object
{
public:
    int lifepower, bullet_size, bullet_range, cur_cooldown, max_cooldown, cur_anim_frame;
    bool life_draining, is_player;

    Person(int x, int y, int hitbox, int life, int cooldown, std::string s);
    void attack(Person* attacker);
    virtual void get_life() {};
    virtual void kill() {};
};

class Obstacle;

class Base_bullet: public Object
{
public:
    float accurate_pos[2];
    bool enemy, remove_on_impact, instant_kill;
    int lifetime;
    Person* shot_by;

    Base_bullet(Person* shooter);
    void update();
    virtual void move() {};
    bool exec_hit(Person* p); //returns true if the scan for further hits should be stopped
    virtual void hit_obstacle(Obstacle* o) {};
};

extern std::deque<Object*> objects, to_delete;
#endif // _BASE_CLASSES
