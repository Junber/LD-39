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

Enemy::Enemy(int x, int y, int hitbox, std::string s, Enemy_type* typ): Person(x, y, hitbox, typ->life, typ->cooldown, s)
{
    type = typ;
    dead = false;
    enemies.push_back(this);
    bullet_range = type->bullet_range;
    bullet_size = type->bullet_size;

    render_size[0] = render_size[1] = 34;

    cur_anim_frame=0;

    dead_tex = s+"_dead";

    rotate_center = {15,18};
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
    iframes = 0;
    cur_anim_frame = 0;
}

void Enemy::update()
{
    cur_anim_frame++;

    if (dead)
    {
        if (cur_anim_frame > bullet_range*2) to_delete.push_back(this); //*2 actually not required but safety first
        return;
    }

    if (iframes>0) iframes--;

    int last_pos[2] = {pos[0],pos[1]};
    int dx = player->pos[0]-pos[0], dy = player->pos[1]-pos[1], sum = abs(dx)+abs(dy);

    if (sum && (type->movement == walk_towards_player || (type->movement == keep_distance_from_player && dx*dx+dy*dy > bullet_range*bullet_range*2/3)))
    {
        pos[0] += type->speed*dx/sum;
        pos[1] += type->speed*dy/sum;
    }

    cur_cooldown--;
    if (cur_cooldown<=0)
    {
        cur_cooldown = max_cooldown;

        if (type->weapon == alien_pistol)
        {
            new Bullet(this,type->bullet_speed*dx/sum,type->bullet_speed*dy/sum);
        }
        else if (type->weapon == smart_pistol)
        {
            const float v[2] = {float(player->pos[0]-player->last_pos[0])/type->bullet_speed, float(player->pos[1]-player->last_pos[1])/type->bullet_speed},
                d[2] = {float(pos[0]-player->pos[0])/type->bullet_speed, float(pos[1]-player->pos[1])/type->bullet_speed},
                r = hitbox_size+player->hitbox_size-2,
                a =    v[0]*v[0] + v[1]*v[1] - 1,
                b = -2*(v[0]*d[0] + v[1]*d[1] + r),
                c =    d[0]*d[0] + d[1]*d[1] - r*r,
                det = b*b - 4*a*c;

            if (det >= 0)
            {
                const float sqr_det = std::sqrt(det),
                    t1 = (-b + sqr_det)/ (2*a),
                    t2 =   (-b - sqr_det)/ (2*a),
                    t = (t2>0 ? t2 : t1),
                    w[2] = {v[0]*t-d[0], v[1]*t-d[1]},
                    len = std::sqrt(w[0]*w[0] + w[1]*w[1]);

                new Bullet(this,type->bullet_speed*w[0]/len,type->bullet_speed*w[1]/len);
            }
            else
            {
                //This means the bullet is to slow, so we just change the weapon altogether
                type->weapon = alien_pistol;
                cur_cooldown = 0;
            }
        }
        else if (type->weapon == circlegun)
        {
            for (int i=0; i<=63; i+=5)
            {
                new Bullet(this,type->bullet_speed*(dx+std::sin(i/10)*50)/sum,type->bullet_speed*(dy+std::cos(i/10)*50)/sum);
            }
        }
        else if (type->weapon == reverse_circlegun)
        {
            for (int i=0; i<=63; i+=5)
            {
                Bullet* b = new Bullet(this,type->bullet_speed*(dx-std::sin(i/10)*50)/sum,type->bullet_speed*(dy-std::cos(i/10)*50)/sum);
                b->accurate_pos[0] += std::sin(i/10)*50;
                b->accurate_pos[1] += std::cos(i/10)*50;
            }
        }
    }

    for (Obstacle* o: obstacles)
    {
        if (o->collides(this)) o->push_away(this);
    }

    switch(get_anim_type())
    {
    case 0:
        cur_anim_frame %= 120;
        break;
    case 1:
        cur_anim_frame %= 80;
        break;
    case 2:
        cur_anim_frame %= 30;
        break;
    }

    if (last_pos[0] != pos[0] || last_pos[1] != pos[1]) rotation = std::atan2(pos[1]-last_pos[1],pos[0]-last_pos[0])*180/M_PI;
    else rotation = std::atan2(player->pos[1]-pos[1],player->pos[0]-pos[0])*180/M_PI;
}

int Enemy::get_anim_frame()
{
    return (cur_anim_frame/10);
    /*switch(get_anim_type())
    {
    case 0:
        //...
    case 1:

        break;
    case 2:

    }*/
}

int Enemy::get_anim_type()
{
    if (dead) return 0;
    else if (cur_cooldown < 3*max_cooldown/4) return 1;
    else return 2;
}

//Player

Player::Player() : Person(0,0, 15, 100,100, "age1")
{
    is_player = true;
    speed = 2;

    cur_anim_frame = 0;
    cur_cooldown = 0;

    render_size[0] = render_size[1] = 32;

    last_pos[0] = pos[0];
    last_pos[1] = pos[1];

    rotate_center = {render_size[0]/2-hitbox_offset,render_size[1]/2};

    age = overpowered;
    hitbox_size = 4;
}

void Player::update()
{
    last_pos[0] = pos[0];
    last_pos[1] = pos[1];

    auto state = SDL_GetKeyboardState(nullptr);

    bool moving = false;

    int x, y;
    SDL_GetMouseState(&x,&y);
    x = x/zoom+camera[0];
    y = y/zoom+camera[1];

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

    if (moving)
    {
        camera[0] = std::max(0, std::min(map_size[0]-window[0], pos[0]-window[0]/2));
        camera[1] = std::max(0, std::min(map_size[1]-window[1], pos[1]-window[1]/2));
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
    if (cur_cooldown <= 0 && !transition::transition)
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
    cur_cooldown = 0;

    transition::transition = true;
    transition::tex = load_image("age"+std::to_string(age)+"-"+std::to_string(age+1));
    transition::time = 0;
    SDL_QueryTexture(transition::tex, nullptr, nullptr, &transition::size, &transition::h);

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
    if (transition::transition) return;

    pos[0] += hitbox_offset;
    Person::render();
    pos[0] -= hitbox_offset;

    if (show_hitbox)
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderDrawPoint(renderer,pos[0]-camera[0],pos[1]-camera[1]);
        circleRGBA(renderer,pos[0]-camera[0],pos[1]-camera[1],hitbox_size,255,255,255,255);
        circleRGBA(renderer,pos[0]-camera[0],pos[1]-camera[1],obstacle_hitbox,255,255,255,255);
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
    if (age==overpowered || age == laser) return (cur_cooldown > 0);
    else
    {
        if (age == squaregun || age == pistol || cur_cooldown < 3*max_cooldown/4) return 0;
        else return 1;
    }
}

//NPC

NPC::NPC(int x, int y, int hitbox, std::string s) : Person(x, y, hitbox, 0, 1, s)
{
    dead = scared = false;
    friends.push_back(this);

    dead_tex = s+"_dead";

    real_pos[0] = circle_around[0] = pos[0];
    real_pos[1] = circle_around[1] = pos[1];
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
        const int d = std::pow(pos[0]-p->pos[0],2)+std::pow(pos[1]-p->pos[1],2);
        if (d < 2500)
        {
            scared = true;
            run_from = p;
            break;
        }
        else if (scared && d < 40000)
        {
            run_from = p;
            break;
        }
    }

    if (!run_from)
    {
        scared = false;

        if (pos[0] == circle_around[0] && pos[1] == circle_around[1]) real_pos[0] += 1;
        else
        {
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
