/**
 * Testing EnTT ECS library using SDL2 as the media library
 * Implementing a shmup to have fun while I experiment
 */
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

entt::entity create_player_entity(
        AnimationMap& animations,
        entt::registry& registry)
{
    // KeyActionMap holds an association between "low level" key code
    // to high level action which is represented by a string
    // the action could be a more efficient compile time hashed string but
    // for now a plain old string is enough
    const components::KeyActionMap key_map = {
        {SDL_SCANCODE_LEFT, "move_left"},
        {SDL_SCANCODE_RIGHT, "move_right"},
        {SDL_SCANCODE_UP, "move_up"},
        {SDL_SCANCODE_DOWN, "move_down"},
        {SDL_SCANCODE_Z, "fire"}
    };

    auto entity = registry.create();
    registry.assign<components::position>(entity, 100.f, 300.f);
    registry.assign<components::velocity>(entity, 0.f, 0.f, 150.f);
    registry.assign<components::source_rect>(entity);
    registry.assign<components::destination_rect>(entity);
    registry.assign<components::animation>(entity, &animations["player"], .6f);
    registry.assign<components::image>(
            entity,
            Resources::get("resources/images/player.png"));
    registry.assign<components::gamepad>(
            entity,
            key_map);

    return entity;
}

void create_enemy_entity(
        const float x, const float y,
        AnimationMap& animations,
	entt::entity& target,
        entt::registry& registry)
{

    auto enemy = registry.create();
    registry.assign<components::position>(enemy, x, y);
    registry.assign<components::source_rect>(enemy);
    registry.assign<components::destination_rect>(enemy);
    registry.assign<components::energy>(enemy, 100);
    registry.assign<components::animation>(enemy, &animations["ufo"], .5f);
    registry.assign<components::target>(enemy, target);
    registry.assign<components::image>(
            enemy,
            Resources::get("resources/images/ufo.png"));
}

void main_loop(SDL_Window* window)
{
    unsigned int old_time = SDL_GetTicks();
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

    Resources::load("resources/images/background.png", renderer);
    Resources::load("resources/images/player.png", renderer);
    Resources::load("resources/images/ufo.png", renderer);
    Resources::load("resources/images/bullet.png", renderer);

    SDL_Texture* background = Resources::get("resources/images/background.png");

    entt::registry registry;

    AnimationMap player_animations = TexturePackerAnimationLoader::load(
            "resources/images/player.json");
    auto player = create_player_entity(player_animations, registry);

    AnimationMap enemy_animations = TexturePackerAnimationLoader::load(
            "resources/images/ufo.json");
    create_enemy_entity(200.f, 50.f, enemy_animations, player, registry);
    create_enemy_entity(300.f, 100.f, enemy_animations, player, registry);
    create_enemy_entity(100.f, 180.f, enemy_animations, player, registry);
    create_enemy_entity(500.f, 80.f, enemy_animations, player, registry);

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


        handle_gamepad(screen_rect, registry);

        update_animation(dt, registry);
        update_linear_velocity(dt, registry);
        update_destination_rect(registry);
        update_shot_to_target_behaviour(screen_rect, registry);
        resolve_collisions(registry);
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
