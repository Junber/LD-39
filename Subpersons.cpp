#include "Subpersons.h"

#include "Subbullets.h"
#include "Loading.h"
#include "Obstacles.h"
#include <iostream>
#include "SDL2_gfxPrimitives.h"

std::deque<Person*> enemies, dead_enemies, friends, dead_friends;
Player* player;
bool show_hitbox = true;

//Enemy

Enemy::Enemy(int x, int y, int hitbox, int life, int cooldown, std::string s): Person(x, y, hitbox, life, cooldown, s)
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
    iframes = 0;
}

void Enemy::update()
{
    if (dead) return;

    if (iframes>0) iframes--;

    for (Obstacle* o: obstacles)
    {
        if (o->collides(this)) o->push_away(this);
    }
}

//Player

const int hitbox_offset = 5;
Player::Player() : Person(0,0, 15, 100,100, "age1")
{
    player = true;
    speed = 1;

    cur_anim_frame = 0;
    cur_cooldown = 0;

    render_size[0] = render_size[1] = 32;

    rotate_center = {render_size[0]/2-hitbox_offset,render_size[1]/2};

    age = overpowered;
    hitbox_size = 4;
}

void Player::update()
{
    auto state = SDL_GetKeyboardState(nullptr);

    bool moving = false;

    int x, y;
    SDL_GetMouseState(&x,&y);
    x /= zoom;
    y /= zoom;

    if (!(age==overpowered && cur_cooldown > 0))
    {
        if (state[SDL_SCANCODE_D])
        {
            pos[0] += speed;
            moving = true;
        }
        else if (state[SDL_SCANCODE_A])
        {
            pos[0] -= speed;
            moving = true;
        }
        if (state[SDL_SCANCODE_S])
        {
            pos[1] += speed;
            moving = true;
        }
        else if (state[SDL_SCANCODE_W])
        {
            pos[1] -= speed;
            moving = true;
        }

        if (age == cane && cur_cooldown == max_cooldown-25/speed) new Melee(this);

        if (age == cane && cur_cooldown <= max_cooldown-25/speed && cur_cooldown > 3*max_cooldown/4) rotation += 360/(max_cooldown/4-25/speed);
        else rotation = std::atan2(y-pos[1],x-pos[0])*180/M_PI;
    }



    if (moving || (age==cane && get_anim_type()==1)) cur_anim_frame++;
    if (cur_cooldown>0) cur_cooldown--;
    if (iframes>0) iframes--;

    for (Obstacle* o: obstacles)
    {
        if (o->collides(this)) o->push_away(this);
    }
}

void Player::shoot(int x, int y)
{
    if (cur_cooldown <= 0)
    {
        cur_cooldown = max_cooldown;

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
            //the Melee is spawned later
            break;
        case dead:
            break;
        }
    }
}

void Player::kill()
{
    age=static_cast<Ages>(age+1);
    lifepower=100;

    if (age>overpowered) max_cooldown=10;
    if (age>laser) bullet_range = 30;
    if (age>=squaregun) life_draining = false;
    if (age>=cane)
    {
        max_cooldown = 240;
        bullet_size = 15;
        bullet_range = max_cooldown/4;
    }

    tex = load_image("age"+std::to_string(age+1));
    itex = load_image("age"+std::to_string(age+1)+(std::string)"_white");

    if (age>=dead)
    {
        pos[0] += hitbox_offset;

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(500);

        SDL_Texture* death = load_image("death");
        for (int i=0;i<80;i++)
        {
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
            filledCircleRGBA(renderer,pos[0],pos[1],25,255,255,255,255);

            SDL_Rect dest={pos[0]-32/2, pos[1]-32/2, 32, 32},
             src ={0,32*(i/20),32, 32};

            SDL_RenderCopy(renderer, death, &src, &dest);

            SDL_RenderPresent(renderer);
            limit_fps();

            SDL_FlushEvents(0,-1);
        }

        SDL_Delay(100);
        for (int i=0;i<=400;i++)
        {
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
            filledCircleRGBA(renderer,pos[0],pos[1],25,255,255,255,255);

            int offset = i/10;

            filledCircleRGBA(renderer,pos[0],pos[1],std::min(offset/2,25),220,220,220,255);

            SDL_Rect dest={pos[0]-32/2-offset/2, pos[1]-32/2-offset/2, 32+offset, 32+offset},
             src ={0,32*3,32, 32};

            SDL_RenderCopy(renderer, death, &src, &dest);

            if (i >= 300)
            {
                int c = (400-i)*2.5;
                SDL_SetRenderDrawColor(renderer,c,c,c,255);
                SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_MOD);
                SDL_Rect r = {0,0,window[0],window[1]};
                SDL_RenderFillRect(renderer,&r);
            }

            SDL_RenderPresent(renderer);
            limit_fps();

            SDL_FlushEvents(0,-1);
        }

        SDL_Event e;
        SDL_FlushEvents(0,-1);
        bool breakkk=false;
        while (!breakkk)
        {
            while(SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT) breakkk = true;
                else if (e.type == SDL_KEYDOWN) breakkk = true;
                else if (e.type == SDL_MOUSEBUTTONDOWN) breakkk = true;
            }

            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            limit_fps();
        }

        breakk = true;
    }
}

void Player::render()
{
    pos[0] += hitbox_offset;
    Person::render();
    pos[0] -= hitbox_offset;

    if (show_hitbox)
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderDrawPoint(renderer,pos[0],pos[1]);
        circleRGBA(renderer,pos[0],pos[1],hitbox_size,255,255,255,255);
        circleRGBA(renderer,pos[0],pos[1],obstacle_hitbox,255,255,255,255);
    }
}

int Player::get_anim_frame()
{
    if (!(age==cane && get_anim_type()==1)) cur_anim_frame %= 50/speed;

    if (age==overpowered && get_anim_type()==1) return 0;
    else if (cur_anim_frame < 15/speed) return 0;
    else if (cur_anim_frame < 25/speed) return 1;
    else if (cur_anim_frame < 40/speed) return 2;
    else if (cur_anim_frame < 50/speed) return (age>=pistol?3:1);
    else if (age==cane && get_anim_type()==1) return 4;

    return 0; //shouldn't happen
}

int Player::get_anim_type()
{
    if (age==overpowered && cur_cooldown > 0) return 1;

    if (age == squaregun || age == pistol || cur_cooldown < 3*max_cooldown/4) return 0;
    else return 1;
}

//NPC

NPC::NPC(int x, int y, int hitbox, std::string s) : Person(x, y, hitbox, 0, 1, s)
{
    player = dead = scared = false;
    friends.push_back(this);

    dead_tex = s+"_dead";

    real_pos[0] = pos[0];
    real_pos[1] = pos[1];
}

NPC::~NPC()
{
    if (dead) remove_it(&dead_friends, (Person*) this);
    else remove_it(&friends, (Person*) this);
}

int sign (int x)
{
    return (x>0?1:(x<0?-1:0));
}

const int circle_radius = 50;
void NPC::update()
{
    if (dead) return;

    Person* run_from = nullptr;
    for (Person* p: enemies)
    {
        if (std::pow(pos[0]-p->pos[0],2)+std::pow(pos[1]-p->pos[1],2) < 2500)
        {
            scared = true;
            run_from = p;
            break;
        }
        else if (scared && std::pow(pos[0]-p->pos[0],2)+std::pow(pos[1]-p->pos[1],2) < 40000)
        {
            run_from = p;
            break;
        }
    }

    if (!run_from)
    {
        scared = false;

        float dx = circle_around[0]-real_pos[0], dy = circle_around[1]-real_pos[1], sum = abs(dx)+abs(dy);

        if (std::pow(pos[0]-circle_around[0],2)+std::pow(pos[1]-circle_around[1],2) < std::pow(circle_radius-2,2))
        {
            real_pos[0] -= 2.*dx/sum;
            real_pos[1] -= 2.*dy/sum;
        }
        else
        {
            double angle = std::atan2(dy,dx);
            angle += 0.02;
            real_pos[0] = circle_around[0]-std::cos(angle)*circle_radius;
            real_pos[1] = circle_around[1]-std::sin(angle)*circle_radius;
        }

        pos[0] = real_pos[0];
        pos[1] = real_pos[1];
    }
    else
    {
        int dx = run_from->pos[0]-pos[0], dy = run_from->pos[1]-pos[1], sum = abs(dx)+abs(dy);

        pos[0] -= 4*dx/sum;
        pos[1] -= 4*dy/sum;

        circle_around[0] = pos[0]+circle_radius*dx/sum;
        circle_around[1] = pos[1]+circle_radius*dy/sum;

        real_pos[0] = pos[0];
        real_pos[1] = pos[1];
    }

    for (Obstacle* o: obstacles)
    {
        if (o->collides(this)) o->push_away(this);
    }
}

void NPC::kill()
{
    remove_it(&friends, (Person*) this);
    dead_friends.push_back(this);
    dead = true;
    tex = itex = load_image(dead_tex);
    iframes = 0;
}
