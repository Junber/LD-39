#ifndef _SUBPERSONS
#define _SUBPERSONS

#include "Base_Classes.h"

class Enemy: public Person
{
public:
    bool dead;
    std::string dead_tex;

    Enemy(int x, int y, int hitbox, int life, std::string s);
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

    Player();
    ~Player();
    void update();
    void shoot(int x, int y);
    void kill();
};

/*class NPC: public Person
{
public:

};*/

extern std::deque<Person*> enemies, dead_enemies, friends;

#endif // _SUBPERSONS
