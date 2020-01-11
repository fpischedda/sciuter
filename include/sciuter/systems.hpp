#ifndef __SCIUTER_SYSTEMS_HPP__
#define __SCIUTER_SYSTEMS_HPP__

#include <string>
#include <sciuter/sdl.hpp>
#include <sciuter/components.hpp>
#include <sciuter/animation.hpp>
#include <sciuter/resources.hpp>

const unsigned int COLLISION_MASK_ENEMIES = 1;
const unsigned int COLLISION_MASK_PLAYER = 2;

void update_timers(float dt, entt::registry &registry);
void handle_gamepad(
    const SDL_Rect& boundaries,
    entt::registry& registry);

SDL_Rect center_position(const int x, const int y, const SDL_Rect& frame_rect);
void update_animations(const float dt, entt::registry &registry);
void update_linear_velocity(const float dt, entt::registry& registry);
void update_destination_rect(entt::registry& registry);
void apply_camera_transformation(const entt::entity& camera,
				 entt::registry& registry);
void update_shot_to_target_behaviour(
    const SDL_Rect& boundaries,
    entt::registry& registry);
void resolve_collisions(entt::registry& registry);
void check_boundaries(entt::registry& registry);

void render_entity(SDL_Renderer* renderer,
		   const int scale,
		   entt::entity& background,
		   entt::registry& registry);
void render_sprites(SDL_Renderer* renderer,
		    const int scale,
		    entt::entity& background,
		    entt::registry& registry);

entt::entity spawn_bullet(
    const components::position& position,
    const components::velocity& velocity,
    const unsigned int collision_mask,
    const SDL_Rect& boundaries,
    entt::registry& registry);

#endif
