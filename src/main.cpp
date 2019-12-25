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

entt::entity spawn_bullet(const float x, const float y,
        Resources& resources,
        entt::registry& registry)
{
    auto bullet = registry.create();
    auto texture = resources.get("resources/images/bullet.png");
    registry.assign<components::position>(bullet, x, y);
    registry.assign<components::source_rect>(
            bullet,
            components::source_rect::from_texture(texture));
    registry.assign<components::velocity>(bullet, 0.f, 1.f, 50.f);
    registry.assign<components::image>(
            bullet,
            texture);
    return bullet;
}

void main_loop(SDL_Window* window)
{
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

    auto player_entity = registry.create();
    registry.assign<components::position>(player_entity, 100.f, 100.f);
    registry.assign<components::source_rect>(player_entity);
    registry.assign<components::animation>(player_entity, &animations["player"], .3f);
    registry.assign<components::image>(
            player_entity,
            resources.get("resources/images/player.png"));

    auto enemy = registry.create();
    registry.assign<components::position>(enemy, 300.f, 300.f);
    registry.assign<components::source_rect>(enemy);
    registry.assign<components::animation>(enemy, &animations["player"], .2f);
    registry.assign<components::image>(
            enemy,
            resources.get("resources/images/player.png"));

    while( !quit )
    {
        if( SDL_PollEvent( &e ) != 0 )
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
                        case SDLK_z:
                            spawn_bullet(300.f, 300.f, resources, registry);
                            break;
                        default:
                            quit = true;
                            break;
                    }
                    break;
            }
        }

        update_animation(16.f / 1000.f, registry);
        update_linear_velocity(16.f / 1000.f, registry);
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
