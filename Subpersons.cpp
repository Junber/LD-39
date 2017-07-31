#include "Subpersons.h"

#include "Subbullets.h"
#include "Loading.h"
#include "Obstacles.h"
#include <iostream>
#include "SDL2_gfxPrimitives.h"
#include "Ammo_Drops.h"

std::deque<Person*> enemies, dead_enemies, friends, dead_friends;
Player* player;
bool show_hitbox = false;

//Enemy

Enemy::Enemy(int x, int y, Enemy_type* typ): Person(x, y, typ->hitbox, typ->life, typ->cooldown, typ->texture)
{
    type = typ;
    dead = false;
    enemies.push_back(this);
    bullet_range = type->bullet_range;
    bullet_size = type->bullet_size;

    render_size[0] = render_size[1] = 34;

    cur_cooldown = random(0,max_cooldown);

    rotate_center = {15,18};
}

Enemy::~Enemy()
{
    if (dead) remove_it(&dead_enemies, (Person*) this);
    else remove_it(&enemies, (Person*) this);
}

void Enemy::kill()
{
    if (type->younger && lifepower <= 0) change_type(type->younger);
    else
    {
        remove_it(&enemies, (Person*) this);
        dead_enemies.push_back(this);
        dead = true;
        iframes = 0;
        cur_anim_frame = 0;
    }
}

void Enemy::update()
{
    cur_anim_frame++;

    if (dead)
    {
        iframes = 0;
        if (cur_anim_frame == 70)
        {
            SDL_Texture* t = load_image("alien_dead");
            int x,y;
            SDL_QueryTexture(t,nullptr,nullptr,&x,&y);
            SDL_Rect r = {pos[0]-x/2,pos[1]-y/2,x,y};
            SDL_SetRenderTarget(renderer, bg);
            SDL_RenderCopyEx(renderer,t,nullptr,&r,rotation,nullptr,SDL_FLIP_NONE);
            SDL_SetRenderTarget(renderer, nullptr);

            if (player->age == pistol || player->age == squaregun) new Ammo(pos[0],pos[1],6);
        }

        if (cur_anim_frame > bullet_range+10 && cur_anim_frame >= 120 && !std::count(to_delete.begin(),to_delete.end(),this)) to_delete.push_back(this); //+10 actually not required but safety first
        return;
    }

    if (iframes>0) iframes--;

    int last_pos[2] = {pos[0],pos[1]};
    int dx = player->pos[0]-pos[0], dy = player->pos[1]-pos[1], sum = abs(dx)+abs(dy);

    if (sum && (type->movement == walk_towards_player ||
                ((type->movement == keep_distance_from_player || type->movement == circle_player) && dx*dx+dy*dy > bullet_range*bullet_range*2/3)))
    {
        pos[0] += type->speed*dx/sum;
        pos[1] += type->speed*dy/sum;
    }
    else if (type->movement == circle_player)
    {
        double angle = std::atan2(dy,dx),
            dist = std::sqrt(dx*dx+dy*dy);

        angle += 2*std::asin(type->speed/(2*dist));
        pos[0] = player->pos[0]-std::cos(angle)*dist;
        pos[1] = player->pos[1]-std::sin(angle)*dist;
    }

    cur_cooldown--;
    if (cur_cooldown<=0)
    {
        cur_cooldown = max_cooldown;

        if (type->weapon == melee)
        {
            if (dx*dx+dy*dy < bullet_size) new Melee(this);
            else cur_cooldown = 0;
        }
        else if (type->weapon == alien_pistol)
        {
            if (!sum) new Bullet(this,type->bullet_speed,0);
            else new Bullet(this,type->bullet_speed*dx/sum,type->bullet_speed*dy/sum);
        }
        else if (type->weapon == smart_pistol)
        {
            float v[2] = {float(player->pos[0]-player->last_pos[0])/type->bullet_speed, float(player->pos[1]-player->last_pos[1])/type->bullet_speed},
                d[2] = {float(pos[0]-player->pos[0])/type->bullet_speed, float(pos[1]-player->pos[1])/type->bullet_speed},
                r = hitbox_size+player->hitbox_size-2,
                a =    v[0]*v[0] + v[1]*v[1] - 1,
                b = -2*(v[0]*d[0] + v[1]*d[1] + r),
                c =    d[0]*d[0] + d[1]*d[1] - r*r,
                det = b*b - 4*a*c;

            if (det >= 0)
            {
                float sqr_det = std::sqrt(det),
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

    for (Person* e: enemies)
    {
        if (e == this) break;
        else if (e->pos[0] == pos[0] || e->pos[1] == pos[1]) pos[0]++;
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

    if (pos[0] < obstacle_hitbox) pos[0] = obstacle_hitbox;
    else if (pos[0] > map_size[0]-obstacle_hitbox) pos[0] = map_size[0]-obstacle_hitbox;
    if (pos[1] < obstacle_hitbox) pos[1] = obstacle_hitbox;
    else if (pos[1] > map_size[1]-obstacle_hitbox) pos[1] = map_size[1]-obstacle_hitbox;

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

void Enemy::change_type(Enemy_type* new_type)
{
    type = new_type;
    cur_cooldown = 0;
    bullet_range = type->bullet_range;
    bullet_size = type->bullet_size;
    lifepower = type->life;
    max_cooldown = type->cooldown;
    tex = load_image(type->texture);
}

void Enemy::get_life()
{
    if (type->older) change_type(type->older);
}

//Player

Player::Player() : Person(map_size[0]/2, map_size[1]/2, 15, 100,100, "age1")
{
    is_player = true;
    speed = 2;
    ammo = 6;

    cur_cooldown = 0;

    render_size[0] = render_size[1] = 32;

    last_pos[0] = pos[0];
    last_pos[1] = pos[1];

    rotate_center = {render_size[0]/2-hitbox_offset,render_size[1]/2};

    life_draining = false;
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

        if (age == cane && cur_cooldown <= max_cooldown-25/speed && cur_cooldown > max_cooldown/2) rotation += 360/(max_cooldown/2-25/speed);
        else rotation = std::atan2(y-pos[1],x-pos[0])*180/M_PI;
    }

    if (moving || (age==cane && get_anim_type()==1)) cur_anim_frame++;
    if (cur_cooldown>0) cur_cooldown--;
    if (iframes>0) iframes--;

    for (Obstacle* o: obstacles)
    {
        if (o->collides(this)) o->push_away(this);
    }

    if (pos[0] < obstacle_hitbox) pos[0] = obstacle_hitbox;
    else if (pos[0] > map_size[0]-obstacle_hitbox) pos[0] = map_size[0]-obstacle_hitbox;
    if (pos[1] < obstacle_hitbox) pos[1] = obstacle_hitbox;
    else if (pos[1] > map_size[1]-obstacle_hitbox) pos[1] = map_size[1]-obstacle_hitbox;
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
            if (this == player) screen_shake+=50;
            //play_sound(load_sound("shockwave"));
            new Shockwave(this);
            break;

        case laser:
            //play_sound(load_sound("laser"));
            new Laser(this,x,y);
            break;

        case life_drain:
            if (lifepower > 5)
            {
                lifepower -= 5;
                new Bullet(this, 15.*dx/sum, 15.*dy/sum);
            }
            break;

        case squaregun:
            if (ammo > 0)
            {
                screen_shake+=10;
                for (int i=-1;i<=1;i++)
                {
                    for (int u=-1;u<=1;u++) new Bullet(this, 10.*dx/sum+i, 10.*dy/sum+u);
                }
                ammo--;
            }
            break;

        case pistol:
            if (ammo > 0)
            {
                screen_shake+=5;
                new Bullet(this, 10.*dx/sum, 10.*dy/sum);
                ammo--;
            }
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

    if (age>=laser) max_cooldown=20;

    if (age>=squaregun)
    {
        max_cooldown = 15;
        life_draining = false;
        bullet_size = 5;
        bullet_range = 30;

        ammo = 6;
    }
    else if (age==life_drain)
    {
        max_cooldown = 10;
        life_draining = true;
        bullet_range = 30;
        bullet_size = 10;
    }

    if (age>=cane)
    {
        max_cooldown = 120;
        bullet_size = 15;
        bullet_range = max_cooldown/4;
    }

    tex = load_image("age"+std::to_string(age+1));
    itex = load_image("age"+std::to_string(age+1)+(std::string)"_white");

    if (age>=dead)
    {
        pos[0] += hitbox_offset-camera[0];
        pos[1] += -camera[1];

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
    if (age == squaregun || age == pistol) return 0;
    else if (age==cane) return (cur_cooldown > max_cooldown/2);
    else return (cur_cooldown > 0);
}

//NPC

NPC::NPC(int x, int y, int hitbox, std::string s) : Person(x, y, hitbox, 1, 1, s)
{
    dead = scared = false;
    friends.push_back(this);

    real_pos[0] = circle_around[0] = pos[0];
    real_pos[1] = circle_around[1] = pos[1];

    render_size[0] = render_size[1] = 32;

    rotate_center = {17,17};
}

NPC::~NPC()
{
    if (dead) remove_it(&dead_friends, (Person*) this);
    else remove_it(&friends, (Person*) this);
}

int sign(int x)
{
    return (x>0?1:(x<0?-1:0));
}

const int circle_radius = 50;
void NPC::update()
{
    cur_anim_frame++;

    if (dead)
    {
        iframes = 0;
        if (cur_anim_frame > 25/(2+2*scared)-1)
        {
            if (player->age == pistol || player->age == squaregun) new Ammo(pos[0],pos[1],1);

            SDL_SetRenderTarget(renderer,bg);
            pos[0] += camera[0];
            pos[1] += camera[1];
            render();
            SDL_SetRenderTarget(renderer,nullptr);
            to_delete.push_back(this);
        }
        return;
    }

    float last_pos[2] = {real_pos[0],real_pos[1]};

    Person* run_from = nullptr;
    for (Person* p: enemies)
    {
        const int d = std::pow(pos[0]-p->pos[0],2)+std::pow(pos[1]-p->pos[1],2);
        if (d < 5000)
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

        if (!sum)
        {
            pos[0] += 4;

            circle_around[0] = pos[0]+circle_radius;
            circle_around[1] = pos[1];
        }
        else
        {
            pos[0] -= 4*dx/sum;
            pos[1] -= 4*dy/sum;

            circle_around[0] = pos[0]+circle_radius*dx/sum;
            circle_around[1] = pos[1]+circle_radius*dy/sum;
        }

        real_pos[0] = pos[0];
        real_pos[1] = pos[1];
    }

    for (Obstacle* o: obstacles)
    {
        if (o->collides(this))
        {
            o->push_away(this);

            int dx = pos[0]-o->pos[0], dy = pos[1]-o->pos[1], sum = abs(dx)+abs(dy);

            circle_around[0] = pos[0]+circle_radius*dx/sum;
            circle_around[1] = pos[1]+circle_radius*dy/sum;
        }
    }

    if (pos[0] < obstacle_hitbox)
    {
        pos[0] = obstacle_hitbox;
        circle_around[0] = circle_radius+obstacle_hitbox;
    }
    else if (pos[0] > map_size[0]-obstacle_hitbox)
    {
        pos[0] = map_size[0]-obstacle_hitbox;
        circle_around[0] = map_size[0]-circle_radius-obstacle_hitbox;
    }
    if (pos[1] < obstacle_hitbox)
    {
        pos[1] = obstacle_hitbox;
        circle_around[1] = circle_radius+obstacle_hitbox;
    }
    else if (pos[1] > map_size[1]-obstacle_hitbox)
    {
        pos[1] = map_size[1]-obstacle_hitbox;
        circle_around[1] = map_size[1]-circle_radius-obstacle_hitbox;
    }

    rotation = std::atan2(real_pos[1]-last_pos[1],real_pos[0]-last_pos[0])*180/M_PI;

    cur_anim_frame %= 50/(2+2*scared);
}

void NPC::kill()
{
    remove_it(&friends, (Person*) this);
    dead_friends.push_back(this);
    dead = true;
    scared = false;
    iframes = 0;
    cur_anim_frame = 0;
}

int NPC::get_anim_frame()
{
    if (cur_anim_frame < 15/(2+2*scared)) return 0;
    else if (cur_anim_frame < 25/(2+2*scared)) return 1;
    else if (cur_anim_frame < 40/(2+2*scared)) return 2;
    else if (cur_anim_frame < 50/(2+2*scared)) return 3;

    return 0; //shouldn't happen
}

int NPC::get_anim_type()
{
    return 1+dead;
}
