#ifndef __SCIUTER_BEHAVIORS_HPP__
#define __SCIUTER_BEHAVIORS_HPP__

#include <sciuter/components.hpp>

class BossBehavior : public components::IEntityBehavior {
public:
    virtual const bool has_finished() const {return false;}
    virtual void update(const float dt, entt::entity &entity,
                        entt::registry &registry) {
	auto& position = registry.get<components::position>(entity);
	auto& direction = registry.get<components::velocity>(entity);

        if (direction.dx < 0 && position.x < 100) {
          direction.dx = -direction.dx;
        }

        if (direction.dx > 0 && position.x > 540) {
          direction.dx = -direction.dx;
        }
    }
};

class EnemySpawnerBehavior : components::IEntityBehavior {
private:
    int enemy_count;
    float delay;
    float next_delay;
    entt::entity camera;
public:
  EnemySpawnerBehavior(const int enemy_count_,
		       const float delay_,
                       const entt::entity &camera_) {
      enemy_count = enemy_count_;
      delay = delay_;
      next_delay = delay_;
      camera = camera_;
  }
  virtual const bool has_finished() const { return true; }
  virtual void update(const float dt, entt::entity &entity,
                      entt::registry &registry) {

      delay -= dt;
      if(delay > 0) return; // still no time to spawn

      delay += next_delay;

      if(enemy_count < 0) return;

      enemy_count -= 1;

      auto position = registry.get<components::position>(entity);
      auto enemy = registry.create();
      registry.assign<components::position>(enemy, position);
  }
};

#endif
