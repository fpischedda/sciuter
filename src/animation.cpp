#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <sciuter/animation.hpp>

using json = nlohmann::json;

AnimationMap TexturePackerAnimationLoader::load(std::istream &input)
{
    json j;
    input >> j;
    std::vector<SDL_Rect> rects;
    std::map<std::string, SDL_Rect> all_frames;
    AnimationMap animations;

    // setup frame rects for all frames defined in the json
    for( auto& [frame_name, frame] : j["frames"].items() )
    {
        auto& source_frame = frame["frame"];
        SDL_Rect rect = {
            source_frame["x"], source_frame["y"],
            source_frame["w"], source_frame["h"]};

        all_frames[frame_name] = rect;
    }
    
    for( auto& [anim_name, anim_frame_names] : j["animations"].items() )
    {
        std::vector<SDL_Rect> frames;
        for( auto& frame_name : anim_frame_names )
        {
            frames.push_back(all_frames[frame_name]);
        }
        animations[anim_name] = Animation(frames, anim_name);
    }
    return animations;
}

AnimationMap TexturePackerAnimationLoader::load(const std::string filename)
{
    std::ifstream input(filename);
    auto animation = load(input);
    return animation;
}
