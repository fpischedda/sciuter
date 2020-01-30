/**
 * Here is defined a simple data structure to work with
 * animations; an Animation is just a collection of frames (rects)
 * and a name.
 * AnimationMap is used to keep multiple animations together, like
 * "player_jump", "run" and so on
 */
#ifndef __SCIUTER__ANIMATION_HPP__
#define __SCIUTER__ANIMATION_HPP__

#include <map>
#include <string>
#include <vector>
#include <sciuter/sdl.hpp>

class Animation
{
    private:
        std::vector<SDL_Rect> m_frames;
        std::string m_name;

    public:
        Animation() {}
        Animation(const std::vector<SDL_Rect> &frames, const std::string name="")
            : m_frames(frames), m_name(name){}

        const std::vector<SDL_Rect>& get_frames() const { return m_frames; }
        const int get_frame_count() const { return m_frames.size(); }
};

typedef std::map<std::string, Animation> AnimationMap;

class TexturePackerAnimationLoader
{
    public:
        static AnimationMap load(const std::string filename);
        static AnimationMap load(std::istream &input);
};

#endif
