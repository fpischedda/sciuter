#include <sciuter/systems.hpp>

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

void update_destination_rect(entt::registry& registry)
{
    auto view = registry.view<
        components::position,
        components::source_rect,
        components::destination_rect>();

    for(auto entity: view) {
        auto &position = view.get<components::position>(entity);
        auto &frame_rect = view.get<components::source_rect>(entity);
        auto &dest = view.get<components::destination_rect>(entity);

        dest.rect = center_position(position.x, position.y, frame_rect.rect);
    }
}

void check_boundaries(entt::registry& registry)
{
    auto view = registry.view<
        components::destination_rect,
        components::screen_boundaries>();

    for(auto entity: view) {
        auto &dest_rect = view.get<components::destination_rect>(entity);
        auto &boundaries = view.get<components::screen_boundaries>(entity);

        if(!SDL_HasIntersection(&dest_rect.rect, &boundaries.rect))
        {
            registry.destroy(entity);
        }
    }
}

void resolve_collisions(entt::registry& registry)
{
    auto view_bullets = registry.view<
        components::destination_rect,
        components::damage>();
    auto view_enemies = registry.view<
        components::destination_rect,
        components::energy>();

    for(auto bullet: view_bullets) {
        auto &bullet_rect = view_bullets.get<components::destination_rect>(bullet);
        auto &damage = view_bullets.get<components::damage>(bullet);

        for(auto enemy: view_enemies) {
            auto &enemy_rect = view_enemies.get<components::destination_rect>(enemy);
            auto &energy = view_enemies.get<components::energy>(enemy);

            if(SDL_HasIntersection(&bullet_rect.rect, &enemy_rect.rect))
            {
                registry.destroy(bullet);
                energy.value -= damage.value;

                if(energy.value <= 0)
                {
                    registry.destroy(enemy);
                }
            }
        }
    }
}

void render_sprites(SDL_Renderer* renderer, entt::registry& registry)
{
    auto view = registry.view<
        components::image,
        components::source_rect,
        components::destination_rect>();

    for(auto entity: view) {
        auto &image = view.get<components::image>(entity);
        auto &frame_rect = view.get<components::source_rect>(entity);
        auto &dest_rect = view.get<components::destination_rect>(entity);

        SDL_RenderCopy(
                renderer, image.texture,
                &frame_rect.rect, &dest_rect.rect);
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
    registry.assign<components::destination_rect>(bullet);
    registry.assign<components::velocity>(bullet, 0.f, -1.f, 100.f);
    registry.assign<components::screen_boundaries>(bullet, boundaries);
    registry.assign<components::damage>(bullet, 10);
    registry.assign<components::image>(
            bullet,
            texture);
    return bullet;
}
