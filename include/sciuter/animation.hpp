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

        const std::vector<SDL_Rect>& get_frames() { return m_frames; } 
        const int get_frame_count() { return m_frames.size(); } 
};

typedef std::map<std::string, Animation> AnimationMap;

class TexturePackerAnimationLoader
{
    public:
        static AnimationMap load(const std::string filename);
        static AnimationMap load(std::istream &input);
};

#endif
