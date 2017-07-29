#include "Subpersons.h"

#include "Bullet.h"

std::deque<Enemy*> enemies;
Player* player;

//Enemy

Enemy::Enemy(int x, int y, int hitbox, int life, std::string s): Person(x, y, hitbox, life, s)
{
    player = false;
    enemies.push_back(this);
}

Enemy::~Enemy()
{
    remove_it(&enemies, this);
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
    new Bullet(this, 5*dx/sum, 5*dy/sum);
}
