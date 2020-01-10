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

//game dimension constants
const int AREA_WIDTH = 640;
const int AREA_HEIGHT = 480;

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
    registry.assign<components::timer>(entity, 0.05);
    registry.assign<components::energy>(entity, 100000);
    registry.assign<components::collision_mask>(entity, COLLISION_MASK_PLAYER);
    registry.assign<components::gamepad>(
            entity,
            key_map);

    return entity;
}

entt::entity create_enemy_entity(
        const float x, const float y,
        AnimationMap& animations,
        entt::registry& registry)
{

    auto enemy = registry.create();
    registry.assign<components::position>(enemy, x, y);
    registry.assign<components::source_rect>(enemy);
    registry.assign<components::destination_rect>(enemy);
    registry.assign<components::energy>(enemy, 100);
    registry.assign<components::animation>(enemy, &animations["ufo"], .5f);
    registry.assign<components::collision_mask>(enemy, COLLISION_MASK_ENEMIES);
    registry.assign<components::image>(
            enemy,
            Resources::get("resources/images/ufo.png"));
    return enemy;
}

entt::entity create_boss_entity(
        const float x, const float y,
	entt::entity& target,
        entt::registry& registry)
{
    auto texture = Resources::get("resources/images/boss.png");
    auto enemy = registry.create();
    registry.assign<components::position>(enemy, x, y);
    registry.assign<components::velocity>(enemy, 0.f, 0.f, 50.f);
    registry.assign<components::source_rect>(
            enemy,
            components::source_rect::from_texture(texture));
    registry.assign<components::destination_rect>(enemy);
    registry.assign<components::energy>(enemy, 300);
    registry.assign<components::timer>(enemy, 0.5f);
    registry.assign<components::target>(enemy, target);
    registry.assign<components::image>(enemy, texture);
    registry.assign<components::collision_mask>(enemy, COLLISION_MASK_ENEMIES);
    return enemy;
}

entt::entity create_camera(const SDL_Rect& viewport,
			   const SDL_Rect& bounds,
			   entt::registry& registry)
{
    entt::entity camera = registry.create();
    return camera;
}

void main_loop(SDL_Window* window, const int fixed_scale)
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
    Resources::load("resources/images/boss.png", renderer);
    Resources::load("resources/images/bullet.png", renderer);
    Resources::load("resources/images/bullet-enemy.png", renderer);

    SDL_Texture* background = Resources::get("resources/images/background.png");

    entt::registry registry;

    AnimationMap player_animations = TexturePackerAnimationLoader::load(
            "resources/images/player.json");
    auto player = create_player_entity(player_animations, registry);

    AnimationMap enemy_animations = TexturePackerAnimationLoader::load(
            "resources/images/ufo.json");
    create_enemy_entity(200.f, 50.f, enemy_animations, registry);
    create_enemy_entity(300.f, 100.f, enemy_animations, registry);
    create_enemy_entity(100.f, 180.f, enemy_animations, registry);
    create_enemy_entity(500.f, 80.f, enemy_animations, registry);

    create_boss_entity(320.f, 200.f, player, registry);

    SDL_Rect screen_rect = {0, 0, AREA_WIDTH, AREA_HEIGHT};
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


	update_timers(dt, registry);
        handle_gamepad(screen_rect, registry);

        update_animations(dt, registry);
        update_linear_velocity(dt, registry);
        update_destination_rect(registry);
        resolve_collisions(registry);
        check_boundaries(registry);
        update_shot_to_target_behaviour(screen_rect, registry);

        //Clear screen
        SDL_RenderClear( renderer );

        //Render texture to screen
        SDL_RenderCopy( renderer, background, NULL, NULL );

        render_sprites(renderer, fixed_scale, registry);

        //Update screen
        SDL_RenderPresent( renderer );
    }
    SDL_DestroyRenderer( renderer );
}

int main( int argc, char* args[] )
{
    SDL_Window* window = sdl_init(AREA_WIDTH, AREA_HEIGHT);

    if( NULL == window)
    {
	SDL_Log("window creation failed: %s", SDL_GetError());
	return 1;
    }

    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
    {
	SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	return 1;
    }

    cout << "width: " << dm.w << " height: " << dm.h;

    int scale = dm.w / AREA_WIDTH;

    // if upscaling I want the window to be a bit smaller than the screen
    // (in future enable this check only for windowed mode)
    if(scale > 1 && AREA_WIDTH * scale == dm.w) {
	scale = scale - 1;
    }

    SDL_SetWindowSize(window, AREA_WIDTH * scale, AREA_HEIGHT * scale);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, 10);

    main_loop(window, scale);

    //Quit SDL subsystems
    sdl_quit(window);

    return 0;
}
