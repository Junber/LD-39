#ifndef _SUBPERSONS
#define _SUBPERSONS

#include "Base_Classes.h"

class Enemy: public Person
{
    Enemy(int x, int y, int hitbox, int life, std::string s);
    ~Enemy();
};

class Player: public Person
{
public:
    Player();
    void update();
    void shoot(int x, int y);
};

extern std::deque<Enemy*> enemies;
extern Player* player;

#endif // _SUBPERSONS
