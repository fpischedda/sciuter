//Using SDL and standard IO
#include <iostream>
#include <string>
#include <sciuter/sdl.hpp>
#include <sciuter/components.hpp>
#include <sciuter/animation.hpp>
#include <sciuter/resources.hpp>
#include <sciuter/systems.hpp>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

using namespace std;

void main_loop(SDL_Window* window)
{
    unsigned int old_time = SDL_GetTicks();
    Resources resources;
    bool quit = false;
    SDL_Event e;

    //Create renderer for window
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if( renderer == NULL )
    {
        cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl;
        return;
    }

    //Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    resources.load("resources/images/background.png", renderer);
    resources.load("resources/images/player.png", renderer);
    resources.load("resources/images/bullet.png", renderer);

    SDL_Texture* background = resources.get("resources/images/background.png");
    AnimationMap animations = TexturePackerAnimationLoader::load("resources/images/player.json");

    entt::registry registry;

    const components::KeyActionMap key_map = {
        {SDL_SCANCODE_LEFT, "move_left"},
        {SDL_SCANCODE_RIGHT, "move_right"},
        {SDL_SCANCODE_UP, "move_up"},
        {SDL_SCANCODE_DOWN, "move_down"},
        {SDL_SCANCODE_Z, "fire"}
    };

    auto player_entity = registry.create();
    registry.assign<components::position>(player_entity, 100.f, 100.f);
    registry.assign<components::velocity>(player_entity, 0.f, 0.f, 150.f);
    registry.assign<components::source_rect>(player_entity);
    registry.assign<components::animation>(player_entity, &animations["player"], .6f);
    registry.assign<components::image>(
            player_entity,
            resources.get("resources/images/player.png"));
    registry.assign<components::gamepad>(
            player_entity,
            key_map);

    auto enemy = registry.create();
    registry.assign<components::position>(enemy, 300.f, 300.f);
    registry.assign<components::source_rect>(enemy);
    registry.assign<components::animation>(enemy, &animations["player"], .5f);
    registry.assign<components::image>(
            enemy,
            resources.get("resources/images/player.png"));

    SDL_Rect screen_rect = {0, 0, 640, 480};
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            switch( e.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            quit = true;
                            break;
                    }
                    break;
            }
        }

        unsigned int now_time = SDL_GetTicks();
        float dt = (now_time - old_time) * 0.001f;
        old_time = now_time;


        handle_gamepad(screen_rect, resources, registry);

        update_animation(dt, registry);
        update_linear_velocity(dt, registry);
        check_boundaries(registry);

        //Clear screen
        SDL_RenderClear( renderer );

        //Render texture to screen
        SDL_RenderCopy( renderer, background, NULL, NULL );

        render_sprites(renderer, registry);

        //Update screen
        SDL_RenderPresent( renderer );
    }
    SDL_DestroyRenderer( renderer );
}

int main( int argc, char* args[] )
{
    SDL_Window* window = sdl_init(SCREEN_WIDTH, SCREEN_HEIGHT);

    if( NULL != window)
    {
        main_loop(window);
    }

    //Quit SDL subsystems
    sdl_quit(window);

    return 0;
}
