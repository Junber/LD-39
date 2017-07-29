#include "Subpersons.h"

#include "Subbullets.h"
#include "Loading.h"

std::deque<Enemy*> enemies, dead_enemies;
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
    if (dead) remove_it(&dead_enemies, this);
    else remove_it(&enemies, this);
}

void Enemy::kill()
{
    remove_it(&enemies, this);
    dead_enemies.push_back(this);
    dead = true;
    tex = load_image(dead_tex);
}

//Player

Player::Player() : Person(0,0, 1,100, "Player")
{
    player = true;
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
}

void Player::shoot(int x, int y)
{
    int dx = x-pos[0], dy = y-pos[1], sum = abs(dx)+abs(dy);
    new Melee(this); //new Bullet(this, 5.*dx/sum, 5.*dy/sum);
}
