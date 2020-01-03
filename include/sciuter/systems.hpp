#ifndef __SCIUTER_SYSTEMS_HPP__
#define __SCIUTER_SYSTEMS_HPP__

#include <string>
#include <sciuter/sdl.hpp>
#include <sciuter/components.hpp>
#include <sciuter/animation.hpp>
#include <sciuter/resources.hpp>

void handle_gamepad(
        SDL_Rect& boundaries,
        entt::registry& registry);

SDL_Rect center_position(const int x, const int y, const SDL_Rect& frame_rect);
void update_animation(float dt, entt::registry &registry);
void update_linear_velocity(float dt, entt::registry& registry);
void update_destination_rect(entt::registry& registry);
void update_shot_to_target_behaviour(
    SDL_Rect& boundaries,
    entt::registry& registry);
void resolve_collisions(entt::registry& registry);
void check_boundaries(entt::registry& registry);
void render_sprites(SDL_Renderer* renderer, entt::registry& registry);

entt::entity spawn_bullet(
        const float x, const float y,
	const float speed,
        const SDL_Rect& boundaries,
        entt::registry& registry);

#endif
