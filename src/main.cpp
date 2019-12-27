//Using SDL and standard IO
#include <stdio.h>
#include <string>
#include <sciuter/sdl.hpp>
#include <sciuter/components.hpp>
#include <sciuter/animation.hpp>
#include <sciuter/resources.hpp>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


entt::entity spawn_bullet(
        const float x, const float y,
        const SDL_Rect& boundaries,
        Resources& resources,
        entt::registry& registry);

void handle_gamepad(
        SDL_Rect& boundaries,
        Resources& resources,
        entt::registry& registry)
{
    auto view = registry.view<
        components::position,
        components::velocity,
        components::gamepad>();

    for(auto entity: view) {
        auto &position = view.get<components::position>(entity);
        auto &velocity = view.get<components::velocity>(entity);
        auto &gamepad = view.get<components::gamepad>(entity);

        gamepad.update();

        if(gamepad.down("move_left"))
        {
            velocity.dx = -1;
        }
        else if(gamepad.down("move_right"))
        {
            velocity.dx = 1;
        }
        else
        {
            velocity.dx = 0;
        }

        if(gamepad.down("move_up"))
        {
            velocity.dy = -1;
        }
        else if(gamepad.down("move_down"))
        {
            velocity.dy = 1;
        }
        else
        {
            velocity.dy = 0;
        }

        velocity.normalize();

        if(gamepad.pressed("fire"))
        {
            spawn_bullet(
                    position.x, position.y,
                    boundaries, resources, registry);
        }
    }
}

SDL_Rect center_position(const int x, const int y, const SDL_Rect& frame_rect)
{
    SDL_Rect position =
    {
        x - frame_rect.w / 2, y - frame_rect.h / 2,
        frame_rect.w, frame_rect.h
    };
    return position;
}

void update_animation(float dt, entt::registry &registry)
{
    auto view = registry.view<
        components::animation,
        components::source_rect>();

    for(auto entity: view) {
        auto &animation = view.get<components::animation>(entity);
        auto &frame_rect = view.get<components::source_rect>(entity);
        animation.update(dt);
        frame_rect.rect = animation.get_current_frame();
    }
}

void update_linear_velocity(float dt, entt::registry& registry)
{
    auto view = registry.view<
        components::position,
        components::velocity>();

    for(auto entity: view) {
        auto &position = view.get<components::position>(entity);
        auto &velocity = view.get<components::velocity>(entity);
        
        position.x += velocity.dx * velocity.speed * dt;
        position.y += velocity.dy * velocity.speed * dt;
    }
}

void check_boundaries(entt::registry& registry)
{
    auto view = registry.view<
        components::position,
        components::source_rect,
        components::screen_boundaries>();

    for(auto entity: view) {
        auto &position = view.get<components::position>(entity);
        auto &frame_rect = view.get<components::source_rect>(entity);
        auto &boundaries = view.get<components::screen_boundaries>(entity);

        SDL_Rect dest_rect = center_position(position.x, position.y, frame_rect.rect);
        if(!SDL_HasIntersection(&dest_rect, &boundaries.rect))
        {
            registry.destroy(entity);
        }
    }
}

void render_sprites(SDL_Renderer* renderer, entt::registry& registry)
{
    auto view = registry.view<
        components::position,
        components::image,
        components::source_rect>();

    for(auto entity: view) {
        auto &position = view.get<components::position>(entity);
        auto &image = view.get<components::image>(entity);
        auto &frame_rect = view.get<components::source_rect>(entity);

        SDL_Rect dest_rect = center_position(position.x, position.y, frame_rect.rect);
        SDL_RenderCopy(renderer, image.texture, &frame_rect.rect, &dest_rect);
    }
}

entt::entity spawn_bullet(
        const float x, const float y,
        const SDL_Rect& boundaries,
        Resources& resources,
        entt::registry& registry)
{
    auto bullet = registry.create();
    auto texture = resources.get("resources/images/bullet.png");
    registry.assign<components::position>(bullet, x, y);
    registry.assign<components::source_rect>(
            bullet,
            components::source_rect::from_texture(texture));
    registry.assign<components::velocity>(bullet, 0.f, -1.f, 100.f);
    registry.assign<components::screen_boundaries>(bullet, boundaries);
    registry.assign<components::image>(
            bullet,
            texture);
    return bullet;
}

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
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
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
