#ifndef _SUBPERSONS
#define _SUBPERSONS

#include "Base_Classes.h"

class Enemy: public Person
{
    bool dead;
    std::string dead_tex;

    Enemy(int x, int y, int hitbox, int life, std::string s);
    ~Enemy();

    void kill();
};

class Player: public Person
{
public:
    Player();
    void update();
    void shoot(int x, int y);
};

extern std::deque<Enemy*> enemies, dead_enemies;
extern Player* player;

#endif // _SUBPERSONS
