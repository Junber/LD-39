#include <SDL.h>
#include <SDL_image.h>
#include <SDL2_gfxPrimitives.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <math.h>
#include <iostream>

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

const int window[2] = {500,500};

bool breakk = false;
SDL_Window* renderwindow;
SDL_Renderer* renderer;

class Player;
Player* player;
class Enemy;
std::deque<Enemy*> enemies;

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

template<class t> void remove_it(std::deque<t>* base, t thing)
{
    base->erase( std::remove( std::begin(*base), std::end(*base), thing ), std::end(*base) );
}

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,("Data\\Graphics\\"+s+".png").c_str());

    return loaded_textures[s];
}

class Object;
std::deque<Object*> objects;
class Object
{
public:
    int pos[2], render_size[2], hitbox_size;
    double rotation;
    SDL_Texture* tex;

    Object(int x, int y, int hitbox, std::string s)
    {
        pos[0] = x;
        pos[1] = y;

        rotation = 0;

        hitbox_size = hitbox;

        if (s.empty())
        {
            tex = nullptr;
            render_size[0] = render_size[1] = 0;
        }
        else
        {
            tex = load_image(s);
            SDL_QueryTexture(tex, nullptr, nullptr, &render_size[0], &render_size[1]);
        }

        objects.push_back(this);
    }

    virtual ~Object()
    {
        remove_it(&objects, this);
    }

    virtual void update()
    {

    }

    virtual void render()
    {
        SDL_Rect r={pos[0]-render_size[0]/2, pos[1]-render_size[1]/2, render_size[0], render_size[1]};

        SDL_RenderCopyEx(renderer, tex, nullptr, &r, rotation, nullptr, SDL_FLIP_NONE);
    }
};

class Person: public Object
{
public:
    int lifepower, bullet_size;
    bool player;

    Person(int x, int y, int hitbox, int life, std::string s) : Object(x, y, hitbox, s)
    {
        lifepower = life;
        bullet_size=5;
    }

    void attack(Person* attacker)
    {
        lifepower -= 10;
        attacker->lifepower += 5;
    }
};

class Enemy: public Person
{
    Enemy(int x, int y, int hitbox, int life, std::string s): Person(x, y, hitbox, life, s)
    {
        player = false;
        enemies.push_back(this);
    }

    ~Enemy()
    {
        remove_it(&enemies, this);
    }
};

class Player: public Person
{
public:
    Player() : Person(0,0, 1,100, "Player")
    {
        player = true;
    }

    void update()
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

    void shoot(int x, int y);
};

class Bullet: public Object
{
public:
    int speed[2];
    bool enemy;
    Person* shot_by;

    Bullet(Person* shooter, int s_x, int s_y) : Object(shooter->pos[0], shooter->pos[1], shooter->bullet_size, "")
    {
        shot_by = shooter;

        enemy = !shot_by->player;
        speed[0] = s_x;
        speed[1] = s_y;
    }

    void update()
    {
        pos[0] += speed[0];
        pos[1] += speed[1];

        if (enemy)
        {
            if (collides(player)) player->attack(shot_by);
        }
        else
        {
            for (Enemy* e: enemies)
            {
                if (collides(e)) e->attack(shot_by);
            }
        }
    }

    bool collides(Object* with)
    {
        return (std::pow(pos[0]-with->pos[0],2) + std::pow(pos[1]-with->pos[1],2) < std::pow(hitbox_size+with->hitbox_size,2));
    }

    void render()
    {
        filledCircleRGBA(renderer,pos[0],pos[1],hitbox_size,enemy*255,(!enemy)*255,0,255);
        circleRGBA(renderer,pos[0],pos[1],hitbox_size,0,0,0,255);
    }
};

void Player::shoot(int x, int y)
{
    int dx = x-pos[0], dy = y-pos[1], sum = abs(dx)+abs(dy);
    new Bullet(this, 5*dx/sum, 5*dy/sum);
}

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);

    renderwindow = SDL_CreateWindow("LD-39", 50, 50, window[0], window[1], SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    player = new Player();

    //SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) breakk = true;

			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                player->shoot(e.button.x, e.button.y);
            }
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            o->update();
            o->render();
        }

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
