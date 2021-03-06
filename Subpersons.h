#ifndef _SUBPERSONS
#define _SUBPERSONS

#include "Base_Classes.h"

extern bool show_hitbox;

enum Movements
{
    none=0,walk_towards_player,keep_distance_from_player,circle_player,MOVEMENTS_NUM
};

enum Weapons
{
    alien_pistol=0,smart_pistol,circlegun,reverse_circlegun,melee,WEAPON_NUM
};

struct Enemy_type
{
    int life, cooldown, speed, hitbox=14;
    int bullet_range, bullet_size, bullet_speed;
    std::string texture="alien_brown";
    Movements movement;
    Weapons weapon;
    Enemy_type *younger=nullptr, *older=nullptr;
};

class Enemy: public Person
{
public:
    bool dead;
    Enemy_type* type;

    Enemy(int x, int y, Enemy_type* typ);
    ~Enemy();
    void update();
    void kill();
    void get_life();
    void change_type(Enemy_type* new_type);
    int get_anim_frame();
    int get_anim_type();
};

enum Ages
{
    overpowered=0,laser,life_drain,squaregun,pistol,cane,dead
};

class Player: public Person
{
public:
    Ages age;
    int speed, last_pos[2], ammo;
    const int hitbox_offset = 5;

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

    NPC(int x, int y, int hitbox, std::string s);
    ~NPC();
    void update();
    void kill();
    int get_anim_frame();
    int get_anim_type();
};

extern std::deque<Person*> enemies, dead_enemies, friends, dead_friends;
extern Player* player;

#endif // _SUBPERSONS
