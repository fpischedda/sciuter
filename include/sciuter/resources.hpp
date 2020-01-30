#ifndef __SCIUTER_RESOURCES_HPP
#define __SCIUTER_RESOURCES_HPP

#include <map>
#include <string>
#include <entt/entt.hpp>
#include <sciuter/sdl.hpp>
#include <sciuter/animation.hpp>

struct texture_resource
{
    SDL_Texture* value;
    ~texture_resource() { if(nullptr != value) SDL_DestroyTexture(value); }
};

struct texture_loader: entt::loader<texture_loader, texture_resource> {
    std::shared_ptr<texture_resource> load(const std::string path, SDL_Renderer* renderer) const {
	auto texture = load_texture(path, renderer);
	return std::shared_ptr<texture_resource>(new texture_resource{texture});
    }
};
using texture_cache = entt::cache<texture_resource>;
using texture_id_type = texture_cache::id_type;

struct animation_resource
{
    const AnimationMap value;
};

using animation_cache = entt::cache<animation_resource>;
using animation_id_type = animation_cache::id_type;

struct animation_loader final: entt::loader<animation_loader, animation_resource> {
    std::shared_ptr<animation_resource> load(const std::string path) const {
	auto animations = TexturePackerAnimationLoader::load(path);
	return std::shared_ptr<animation_resource>(new animation_resource{ animations });
    }
};

typedef std::map<std::string, SDL_Texture*> ResourceMap;

/**
 * A pretty dumb resource manager, right now used only to load, store
 * and cleanup textures
 */
class Resources
{
private:
    ResourceMap m_resources{};
    animation_cache animations_{};
    texture_cache textures_{};

    static Resources s_instance;

    Resources() { }

    SDL_Texture* _load(const std::string path, SDL_Renderer* renderer)
	{
	    auto texture = ::load_texture(path, renderer);
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

    const entt::handle<texture_resource> _load_texture(
	const std::string path, SDL_Renderer* renderer) {
	return _load_texture(entt::hashed_string::to_value(path.c_str()),
		      path,
		      renderer);
    }

    const entt::handle<texture_resource> _load_texture(
	animation_id_type id,
	const std::string path,
	SDL_Renderer* renderer) {
	return textures_.load<texture_loader>(id, path, renderer);
    }

    const entt::handle<texture_resource> _get_texture(texture_id_type id) const {
	return textures_.handle(id);
    }

    void _load_animations(const std::string path) {
	_load_animations(entt::hashed_string::to_value(path.c_str()), path);
    }

    void _load_animations(animation_id_type id,
			  const std::string path) {
	animations_.load<animation_loader>(id, path);
    }

    const entt::handle<animation_resource> _get_animations(animation_id_type id) const {
	return animations_.handle(id);
    }
public:

    ~Resources() {
	for (auto& [_path, texture] : m_resources) {
	    SDL_DestroyTexture(texture);
	}
    }

    static Resources& get_instance() { return s_instance; }

    static void load_animations(const std::string path) {
	s_instance._load_animations(path);
    }

    static void load_animations(animation_id_type id,
				const std::string path) {
	s_instance._load_animations(id, path);
    }

    static const entt::handle<animation_resource> get_animations(
	animation_id_type id) {
	return s_instance._get_animations(id);
    }

    static const entt::handle<texture_resource> load_texture(
	const std::string path,
	SDL_Renderer* renderer) {
	return s_instance._load_texture(path, renderer);
    }

    static const entt::handle<texture_resource> load_texture(
	texture_id_type id,
	const std::string path,
	SDL_Renderer* renderer) {
	return s_instance._load_texture(id, path, renderer);
    }

    static const entt::handle<texture_resource> get_texture(
	texture_id_type id) {
	return s_instance._get_texture(id);
    }

    static SDL_Texture* load(const std::string path, SDL_Renderer* renderer)
	{
	    auto& instance = get_instance();
	    return instance._load(path, renderer);
	}

    static SDL_Texture* get(const std::string path)
	{
	    auto& instance = get_instance();
	    return instance._get(path);
	}
};
#endif
