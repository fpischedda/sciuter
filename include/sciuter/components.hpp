#ifndef __SCIUTER_COMPONENTS_HPP__
#define __SCIUTER_COMPONENTS_HPP__

#include <map>
#include <math.h>
#include <vector>
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

        velocity& normalize()
        {
            float len = sqrt(dx * dx + dy * dy);
            if(len != 0)
            {
                dx /= len;
                dy /= len;
            }
	    return *this;
        }
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

    struct destination_rect
    {
        SDL_Rect rect;
    };

    struct screen_boundaries
    {
        SDL_Rect rect;
    };

    struct image
    {
        SDL_Texture* texture;
    };

    typedef std::map<Uint8, std::string> KeyActionMap;
    typedef std::map<std::string, Uint8> ActionStatusMap;

    struct gamepad
    {
        KeyActionMap key_action_mapping;
        ActionStatusMap previous_status;
        ActionStatusMap current_status;

        gamepad(const KeyActionMap& _key_action_mapping)
            : key_action_mapping(_key_action_mapping)
        {
            for(auto& [_key, val] : _key_action_mapping)
            {
                previous_status[val] = 0;
                current_status[val] = 0;
            }
        }

        void update()
        {
            SDL_PumpEvents();
            const Uint8* keys = SDL_GetKeyboardState(NULL);
            // keep track of previos status
            for(auto& [key, val] : current_status)
            {
                previous_status[key] = val;
            }

            for(auto& [key, val] : key_action_mapping)
            {
                current_status[val] = keys[key];
            }
        }

        bool down(const std::string& action)
        {
            return current_status[action] == 1;
        }

        bool up(const std::string& action)
        {
            return current_status[action] == 1;
        }

        bool pressed(const std::string& action)
        {
            return (current_status[action] == 1 &&
                    previous_status[action] == 0);
        }

        bool released(const std::string& action)
        {
            return (current_status[action] == 0 &&
                    previous_status[action] == 1);
        }
    };

    struct energy
    {
        int value;
    };

    struct damage
    {
        int value;
    };

    struct collision_mask
    {
	unsigned int value;
    };

    struct target
    {
	entt::entity entity;
    };

    struct timer
    {
	float timeout;
	float reset_time;

	timer(const float time) : timeout(time), reset_time(time) {}
	timer(const float time, const float start_offset)
	    : timeout(time * start_offset), reset_time(time) {}

	const float update(const float dt) {
	    if(timeout <= 0.f) {
		timeout += reset_time;
	    }
	    timeout -= dt;
	    return timeout;
	}

	const bool timed_out() const { return timeout <= 0.f; }
    };
} //components


#endif
