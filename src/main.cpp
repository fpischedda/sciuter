//Using SDL and standard IO
#include <stdio.h>
#include <string>
#include <sciuter/sdl.hpp>
#include <sciuter/components.hpp>
#include <sciuter/animation.hpp>

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
    auto view = registry.view<components::animation>();

    for(auto entity: view) {
        auto &animation = view.get<components::animation>(entity);
        animation.update(dt);
    }
}

void render_animated_sprites(SDL_Renderer* renderer, entt::registry& registry)
{
    auto view = registry.view<
        components::position,
        components::animation,
        components::image>();

    for(auto entity: view) {
        auto &position = view.get<components::position>(entity);
        auto &animation = view.get<components::animation>(entity);
        auto &image = view.get<components::image>(entity);

        const SDL_Rect& frame_rect = animation.get_current_frame();
        SDL_Rect dest_rect = center_position(position.x, position.y, frame_rect);
        SDL_RenderCopy(renderer, image.texture, &frame_rect, &dest_rect);
    }
}

void main_loop(SDL_Window* window)
{
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

    SDL_Texture* background = load_texture("resources/images/background.png", renderer);
    SDL_Texture* player_texture = load_texture("resources/images/player.png", renderer);
    AnimationMap animations = TexturePackerAnimationLoader::load("resources/images/player.json");

    entt::registry registry;
    auto player_entity = registry.create();
    registry.assign<components::position>(player_entity, 100.f, 100.f);
    registry.assign<components::animation>(player_entity, &animations["player"], .3f);
    registry.assign<components::image>(player_entity, player_texture);

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
                    quit = true;
                    break;
            }
        }

        update_animation(16.f / 1000.f, registry);
        //Clear screen
        SDL_RenderClear( renderer );

        //Render texture to screen
        SDL_RenderCopy( renderer, background, NULL, NULL );

        render_animated_sprites(renderer, registry);

        //Update screen
        SDL_RenderPresent( renderer );
    }
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(player_texture);
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
