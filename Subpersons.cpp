#include "Subpersons.h"

#include "Subbullets.h"
#include "Loading.h"
#include "Obstacles.h"

std::deque<Person*> enemies, dead_enemies, friends, dead_friends;
Player* player;

//Enemy

Enemy::Enemy(int x, int y, int hitbox, int life, std::string s): Person(x, y, hitbox, life, s)
{
    player = false;
    dead = false;
    enemies.push_back(this);

    dead_tex = s+"_dead";
}

Enemy::~Enemy()
{
    if (dead) remove_it(&dead_enemies, (Person*) this);
    else remove_it(&enemies, (Person*) this);
}

void Enemy::kill()
{
    remove_it(&enemies, (Person*) this);
    dead_enemies.push_back(this);
    dead = true;
    tex = load_image(dead_tex);
}

void Enemy::update()
{
    for (Obstacle* o: obstacles)
    {
        if (o->collides(this)) o->push_away(this);
    }
}

//Player

Player::Player() : Person(0,0, 3,100, "Player")
{
    player = true;

    age = squaregun;//overpowered;
    hitbox_size = 1;
}

void Player::update()
{
    auto state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_D]) pos[0]++;
    else if (state[SDL_SCANCODE_A]) pos[0]--;
    if (state[SDL_SCANCODE_S]) pos[1]++;
    else if (state[SDL_SCANCODE_W]) pos[1]--;

    int x, y;
    SDL_GetMouseState(&x,&y);

    rotation = std::atan2(y-pos[1],x-pos[0])*180/M_PI+90;

    for (Obstacle* o: obstacles)
    {
        if (o->collides(this)) o->push_away(this);
    }
}

void Player::shoot(int x, int y)
{
    int dx = x-pos[0], dy = y-pos[1], sum = abs(dx)+abs(dy);

    switch (age)
    {
    case overpowered:
        new Shockwave(this);
        break;
    case laser:
        new Laser(this,x,y);
        break;
    case life_drain:
        new Bullet(this, 5.*dx/sum, 5.*dy/sum);
        break;
    case squaregun:
        new Bullet(this, 10.*dx/sum, 10.*dy/sum);
        for (int i=-1;i<=1;i++)
        {
            for (int u=-1;u<=1;u++) new Bullet(this, 10.*dx/sum+i, 10.*dy/sum+u);
        }
        break;
    case pistol:
        new Bullet(this, 10.*dx/sum, 10.*dy/sum);
        break;
    case cane:
        new Melee(this);
        break;
    case dead:
        break;
    }
}

void Player::kill()
{
    age=static_cast<Ages>(age+1);
    lifepower=100;

    if (age>=squaregun) life_draining = false;

    if (age>=dead)
    {
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderClear(renderer);

        SDL_Event e;
        bool breakkk=false;
        while (!breakkk)
        {
            while(SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT) breakkk = true;
                else if (e.type == SDL_KEYDOWN) breakkk = true;
                else if (e.type == SDL_MOUSEBUTTONDOWN) breakkk = true;
            }
        }

        breakk = true;
    }
}

//NPC

NPC::NPC(int x, int y, int hitbox, std::string s) : Person(x, y, hitbox, 1, s)
{
    player = false;
    dead = false;
    friends.push_back(this);

    dead_tex = s+"_dead";
}

NPC::~NPC()
{
    if (dead) remove_it(&dead_friends, (Person*) this);
    else remove_it(&friends, (Person*) this);
}

void NPC::update()
{

}

void NPC::kill()
{
    remove_it(&friends, (Person*) this);
    dead_friends.push_back(this);
    dead = true;
    tex = load_image(dead_tex);
}
