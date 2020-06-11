/**
 * Testing EnTT ECS library using SDL2 as the media library
 * Implementing a shmup to have fun while I experiment
 */
#include <random>
#include <string>
#include <sciuter/game.hpp>
#include <sciuter/components.hpp>
#include <sciuter/behaviors.hpp>
#include <sciuter/animation.hpp>
#include <sciuter/resources.hpp>
#include <sciuter/systems.hpp>

//game dimension constants
const int AREA_WIDTH = 640;
const int AREA_HEIGHT = 480;

using namespace std;

entt::entity create_player_entity(
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

    const AnimationMap& animations = Resources::get_animations("player-animations"_hs)->value;

    auto entity = registry.create();
    registry.assign<components::position>(entity, 100.f, 300.f);
    registry.assign<components::velocity>(entity, 0.f, 0.f, 150.f);
    registry.assign<components::source_rect>(entity);
    registry.assign<components::destination_rect>(entity);
    registry.assign<components::animation>(entity, animations.at("player"), .6f);
    registry.assign<components::image>(
            entity,
            Resources::get_texture("player"_hs)->value);
    registry.assign<components::transformation>(entity, 2.f, 0.f);
    registry.assign<components::timer>(entity, 0.10);
    registry.assign<components::energy>(entity, 100000);
    registry.assign<components::collision_mask>(entity, COLLISION_MASK_PLAYER);
    registry.assign<components::gamepad>(
            entity,
            key_map);
    registry.assign<components::draw_order>(entity, 2);

    return entity;
}

entt::entity create_enemy_entity(
        const float x, const float y,
        entt::registry& registry)
{
    const AnimationMap& animations = Resources::get_animations("ufo-animations"_hs)->value;

    auto enemy = registry.create();
    registry.assign<components::position>(enemy, x, y);
    registry.assign<components::velocity>(enemy, 1.f, 0.f, 50.f);
    registry.assign<components::world_position>(enemy);
    registry.assign<components::source_rect>(enemy);
    registry.assign<components::destination_rect>(enemy);
    registry.assign<components::energy>(enemy, 100);
    registry.assign<components::animation>(enemy, animations.at("ufo"), .5f);
    registry.assign<components::collision_mask>(enemy, COLLISION_MASK_ENEMIES);
    registry.assign<components::image>(
            enemy,
            Resources::get_texture("ufo"_hs)->value);
    registry.assign<components::draw_order>(enemy, 1);
    registry.assign<components::entity_behavior>(enemy, new BossBehavior());
    return enemy;
}

entt::entity create_boss_entity(const float x, const float y,
                                  entt::entity &target,
                                  entt::registry &registry) {
  auto texture = Resources::get_texture("boss"_hs)->value;
  auto enemy = registry.create();
  registry.assign<components::position>(enemy, x, y, true);
  registry.assign<components::velocity>(enemy, 1.f, 0.f, 50.f);
  registry.assign<components::world_position>(enemy);
  registry.assign<components::source_rect>(
      enemy, components::source_rect::from_texture(texture));
  registry.assign<components::destination_rect>(enemy);
  registry.assign<components::energy>(enemy, 1000);
  registry.assign<components::timer>(enemy, 0.5f);
  registry.assign<components::target>(enemy, target);
  registry.assign<components::image>(enemy, texture);
  registry.assign<components::collision_mask>(enemy, COLLISION_MASK_ENEMIES);
  registry.assign<components::draw_order>(enemy, 1);
  registry.assign<components::entity_behavior>(enemy, new BossBehavior());
  return enemy;
}

void create_random_enemies(const float end_y,
			   entt::registry& registry)
{
    std::random_device rd;
    std::mt19937 rand_engine(rd());
    std::uniform_real_distribution<> dist_x(0.f, 640.f);
    std::uniform_real_distribution<> dist_y(30.f, 100.f);

    float y = 100;

    while(y < end_y)
    {
	int x = dist_x(rand_engine);
	create_enemy_entity(x, y, registry);
	y += dist_y(rand_engine);
    }
}

entt::entity create_background(entt::registry& registry)
{
    auto bg = registry.create();
    SDL_Texture* texture = Resources::get_texture("background"_hs)->value;

    registry.assign<components::position>(bg, 320.f, 600.f);
    registry.assign<components::world_position>(bg);
    registry.assign<components::source_rect>(
            bg,
            components::source_rect::from_texture(texture));
    registry.assign<components::destination_rect>(bg);
    registry.assign<components::image>(bg, texture);
    registry.assign<components::draw_order>(bg, 0);

    return bg;
}

entt::entity create_camera(const components::position position,
			   entt::registry& registry)
{
    auto camera = registry.create();
    registry.assign<components::position>(camera, position);
    registry.assign<components::velocity>(camera, 0.f, -1.f, 30.f);
    return camera;
}

void load_resources(SDL_Renderer* renderer)
{

    Resources::load_texture(
	"background"_hs,
	"resources/images/background.png",
	renderer);
    Resources::load_texture("player"_hs, "resources/images/player.png", renderer);
    Resources::load_texture("ufo"_hs, "resources/images/ufo.png", renderer);
    Resources::load_texture("boss"_hs, "resources/images/boss.png", renderer);
    Resources::load_texture("bullet"_hs, "resources/images/bullet.png", renderer);
    Resources::load_texture("bullet-enemy"_hs, "resources/images/bullet-enemy.png", renderer);
    Resources::load_texture("bullet-enemy-small"_hs, "resources/images/bullet-enemy-small.png", renderer);

    Resources::load_animations("player-animations"_hs,
			       "resources/images/player.json");
    Resources::load_animations("ufo-animations"_hs,
			       "resources/images/ufo.json");
}

void main_loop(SDL_Window* window, const int scale)
{
    unsigned int old_time = SDL_GetTicks();
    bool quit = false;
    SDL_Event e;

    //Create renderer for window
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if( renderer == NULL )
    {
        SDL_Log("Renderer could not be created! SDL Error: %s", SDL_GetError());
        return;
    }

    load_resources(renderer);

    //Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    entt::registry registry;

    auto player = create_player_entity(registry);

    create_boss_entity(320.f, 50.f, player, registry);
    create_random_enemies(1300.f, registry);

    create_background(registry);

    SDL_Rect screen_rect = {0, 0, AREA_WIDTH, AREA_HEIGHT};
    auto camera = create_camera({0, 1200 - 480}, registry);

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
	update_behaviors(dt, registry);

        update_animations(dt, registry);
        update_linear_velocity(dt, registry);
        update_destination_rect(registry);
        apply_camera_transformation(camera, registry);
        resolve_collisions(registry);
        check_boundaries(registry);
        update_shot_to_target_behaviour(screen_rect, registry);
        update_transformations(registry);

        //Clear screen
        SDL_RenderClear( renderer );

        render_sprites(renderer, scale, registry);

        //Update screen
        SDL_RenderPresent( renderer );
    }
    SDL_DestroyRenderer( renderer );
}
