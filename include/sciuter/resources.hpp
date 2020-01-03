#ifndef __SCIUTER_RESOURCES_HPP
#define __SCIUTER_RESOURCES_HPP

#include <map>
#include <string>
#include <sciuter/sdl.hpp>

typedef std::map<std::string, SDL_Texture*> ResourceMap;

/**
 * A pretty dumb resource manager, right now used only to load, store
 * and cleanup textures
 */
class Resources
{
    private:
	ResourceMap m_resources;
	static Resources s_instance;
	Resources() { }

	SDL_Texture* _load(const std::string path, SDL_Renderer* renderer)
	{
	    auto texture = load_texture(path, renderer);
	    if( nullptr != texture )
	    {
		m_resources[path] = texture;
	    }
	    return texture;
	}

	SDL_Texture* _get(const std::string path)
	{
	    return m_resources[path];
	}

	static Resources& get_instance() { return Resources::s_instance; }

    public:
	~Resources()
	{
	    for(auto& [_path, texture] : m_resources)
	    {
		SDL_DestroyTexture(texture);
	    }
	}

	static SDL_Texture* load(const std::string path, SDL_Renderer* renderer)
	{
	    auto& instance = Resources::get_instance();
	    return instance._load(path, renderer);
	}

	static SDL_Texture* get(const std::string path)
	{
	    auto& instance = Resources::get_instance();
	    return instance._get(path);
	}
};
#endif
