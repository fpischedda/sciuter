#ifndef __SCIUTER_COMPONENTS_HPP__
#define __SCIUTER_COMPONENTS_HPP__

#include <entt/entt.hpp>
#include <sciuter/animation.hpp>

namespace components
{
    struct position
    {
        float x;
        float y;
    };

    struct velocity
    {
        float dx;
        float dy;
        float speed;
    };

    struct animation
    {
        int frame_index;
        float frame_time;
        float next_frame_time;
        Animation* animation_data;
        float speed;

        animation(Animation* _animation, const float _speed)
            : frame_index(0), animation_data(_animation), speed(_speed)
        {
            frame_time = _speed / _animation->get_frame_count();
            next_frame_time = frame_time;
        }

        void update(float dt)
        {
            next_frame_time -= dt;

            if(next_frame_time <= 0.f)
            {
                frame_index += 1;
                if(frame_index >= animation_data->get_frame_count())
                {
                    frame_index = 0;
                }
                next_frame_time += frame_time;
            }
        }

        const SDL_Rect& get_current_frame()
        {
            return animation_data->get_frames()[frame_index];
        };
    };

    struct source_rect
    {
        SDL_Rect rect;

        static const SDL_Rect from_texture(SDL_Texture* texture)
        {
            Uint32 format;
            int access, width, height;
            SDL_QueryTexture(texture, &format, &access, &width, &height);
            const SDL_Rect rect = {0, 0, width, height};
            return rect;
        }
    };

    struct screen_boundaries
    {
        SDL_Rect rect;
    };

    struct image
    {
        SDL_Texture* texture;
    };
} //components


#endif
