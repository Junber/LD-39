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
    void update();
    void shoot(int x, int y);
    void kill();
};

class NPC: public Person
{
public:
    bool dead;
    std::string dead_tex;

    NPC(int x, int y, int hitbox, std::string s);
    ~NPC();
    void update();
    void kill();
};

extern std::deque<Person*> enemies, dead_enemies, friends, dead_friends;
extern Player* player;

#endif // _SUBPERSONS
