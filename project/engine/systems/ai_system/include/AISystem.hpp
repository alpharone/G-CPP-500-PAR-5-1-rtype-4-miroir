#pragma once
#include "Animation.hpp"
#include "Drawable.hpp"
#include "EnemyAI.hpp"
#include "Health.hpp"
#include "ISystem.hpp"
#include "Position.hpp"
#include "Registry.hpp"
#include "Velocity.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace System {

class AISystem : public ISystem {
public:
  struct enemy_type_t {
    std::string type;
    std::string sprite_sheet;
    int frame_x = 0, frame_y = 0, frame_w = 64, frame_h = 64;
    Component::animation_t animation;
    std::string pattern;
    int health = 100;
    float speed = 100.0f;
    int damage = 10;
    int points = 10;
  };

  struct enemy_spawn_t {
    std::string type;
    float spawn_x = 0.0f, spawn_y = 0.0f;
    float spawn_time = 0.0f;
    bool spawned = false;
  };

  struct pattern_t {
    std::string name;
    std::string type;
    float velocity_x = 0.0f;
    float velocity_y = 0.0f;
    float amplitude = 0.0f;
    float frequency = 0.0f;
    float chase_speed = 0.0f;
    float max_distance = 0.0f;
    float shoot_interval = 0.0f;
  };

  struct level_t {
    std::string name;
    std::string background;
    float duration = 60.0f;
    std::vector<enemy_spawn_t> enemies;
  };

private:
  nlohmann::json _config;
  std::vector<level_t> _levels;
  std::unordered_map<std::string, enemy_type_t> _enemyTypes;
  std::unordered_map<std::string, pattern_t> _patterns;
  size_t _currentLevelIndex = 0;
  float _levelTimer = 0.0f;
  bool _levelActive = false;

public:
  AISystem();
  ~AISystem() override = default;

  void init(Ecs::Registry &registry) override;
  void update(Ecs::Registry &registry, double dt) override;
  void shutdown() override;

  void loadConfig(const nlohmann::json &config);
  void startLevel(size_t levelIndex);
  void spawnEnemy(Ecs::Registry &registry, const enemy_spawn_t &spawn,
                  const enemy_type_t &type);

private:
  void loadLevels(const nlohmann::json &config);
  void loadEnemyTypes(const nlohmann::json &config);
  void loadPatterns(const nlohmann::json &config);
  void updateLevelTimer(double dt);
  void checkEnemySpawns(Ecs::Registry &registry, double dt);
  void updateEnemyMovements(Ecs::Registry &registry, double dt);
  int getPatternId(const std::string &pattern);
};

} // namespace System
