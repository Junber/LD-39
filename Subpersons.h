#ifndef _SUBPERSONS
#define _SUBPERSONS

#include "Base_Classes.h"

class Enemy: public Person
{
public:
    bool dead;
    std::string dead_tex;

    Enemy(int x, int y, int hitbox, int life, int cooldown, std::string s);
    ~Enemy();
    void update();
    void kill();
};

enum Ages
{
    overpowered=0,laser,life_drain,squaregun,pistol,cane,dead
};

class Player: public Person
{
public:
    Ages age;
    int speed, cur_anim_frame;

    Player();
    void update();
    void shoot(int x, int y);
    void kill();
    void render();
    int get_anim_frame();
    int get_anim_type();
};

class NPC: public Person
{
public:
    bool dead, scared;
    int circle_around[2];
    float real_pos[2];
    std::string dead_tex;

    NPC(int x, int y, int hitbox, std::string s);
    ~NPC();
    void update();
    void kill();
};

extern std::deque<Person*> enemies, dead_enemies, friends, dead_friends;
extern Player* player;

#endif // _SUBPERSONS
