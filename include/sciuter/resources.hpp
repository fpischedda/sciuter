#ifndef __SCIUTER_RESOURCES_HPP
#define __SCIUTER_RESOURCES_HPP

#include <map>
#include <string>
#include <sciuter/sdl.hpp>

typedef std::map<std::string, SDL_Texture*> ResourceMap;

class Resources
{
    private:
        ResourceMap m_resources;

    public:
        Resources() { }
        ~Resources()
        {
            for(auto& [_path, texture] : m_resources)
            {
                SDL_DestroyTexture(texture);
            }
        }

        SDL_Texture* load(const std::string path, SDL_Renderer* renderer)
        {
            auto texture = load_texture(path, renderer);
            if( nullptr != texture )
            {
                m_resources[path] = texture;
            }
            return texture;
        }

        SDL_Texture* get(const std::string path)
        {
            return m_resources[path];
        }
};
#endif
