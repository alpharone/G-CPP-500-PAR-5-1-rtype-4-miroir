#include "AISystem.hpp"
#include <cmath>
#include <iostream>

namespace System {

AISystem::AISystem() = default;

void AISystem::init(Ecs::Registry &registry) {
  if (!_levels.empty()) {
    startLevel(0);
  }
}

void AISystem::update(Ecs::Registry &registry, double dt) {
  if (!_levelActive)
    return;

  updateLevelTimer(dt);
  checkEnemySpawns(registry, dt);
  updateEnemyMovements(registry, dt);
}

void AISystem::shutdown() {}

void AISystem::loadConfig(const nlohmann::json &config) {
  _config = config;
  loadLevels(config);
  loadEnemyTypes(config);
  loadPatterns(config);
}

void AISystem::startLevel(size_t levelIndex) {
  if (levelIndex >= _levels.size())
    return;

  _currentLevelIndex = levelIndex;
  _levelTimer = 0.0f;
  _levelActive = true;

  for (auto &spawn : _levels[_currentLevelIndex].enemies) {
    spawn.spawned = false;
  }

  std::cout << "Starting level: " << _levels[_currentLevelIndex].name
            << std::endl;
}

void AISystem::spawnEnemy(Ecs::Registry &registry, const enemy_spawn_t &spawn,
                          const enemy_type_t &type) {
  Ecs::Entity enemy = registry.spawnEntity();

  registry.emplaceComponent<Component::position_t>(
      enemy, Component::position_t{spawn.spawn_x, spawn.spawn_y});

  registry.emplaceComponent<Component::velocity_t>(
      enemy, Component::velocity_t{-type.speed, 0.0f});

  registry.emplaceComponent<Component::health_t>(
      enemy, Component::health_t{type.health});

  Component::drawable_t drawable{"sprite", 0, 1};
  drawable.meta["sprite_path"] = type.sprite_sheet;
  registry.emplaceComponent<Component::drawable_t>(enemy, drawable);

  registry.emplaceComponent<Component::animation_t>(enemy, type.animation);

  int patternId = getPatternId(type.pattern);
  registry.emplaceComponent<Component::enemy_ai_t>(
      enemy, Component::enemy_ai_t{patternId, 0.0f});

  std::cout << "Spawned enemy of type: " << type.type << " at ("
            << spawn.spawn_x << ", " << spawn.spawn_y << ")" << std::endl;
}

void AISystem::loadLevels(const nlohmann::json &config) {
  if (!config.contains("levels"))
    return;

  for (const auto &levelJson : config["levels"]) {
    level_t level;
    level.name = levelJson.value("name", "unnamed");
    level.background = levelJson.value("background", "");
    level.duration = levelJson.value("duration", 60.0f);

    if (levelJson.contains("enemies")) {
      for (const auto &enemyJson : levelJson["enemies"]) {
        enemy_spawn_t spawn;
        spawn.type = enemyJson.value("type", "chaser");
        spawn.spawn_x = enemyJson.value("spawn_x", 800.0f);
        spawn.spawn_y = enemyJson.value("spawn_y", 300.0f);
        spawn.spawn_time = enemyJson.value("spawn_time", 0.0f);
        level.enemies.push_back(spawn);
      }
    }

    _levels.push_back(level);
  }
}

void AISystem::loadEnemyTypes(const nlohmann::json &config) {
  if (!config.contains("enemies"))
    return;

  for (const auto &enemyJson : config["enemies"]) {
    enemy_type_t type;
    type.type = enemyJson.value("type", "unknown");
    type.sprite_sheet = enemyJson.value("sprite_sheet", "");
    type.frame_x = enemyJson.value("frame_x", 0);
    type.frame_y = enemyJson.value("frame_y", 0);
    type.frame_w = enemyJson.value("frame_w", 64);
    type.frame_h = enemyJson.value("frame_h", 64);

    if (enemyJson.contains("animation")) {
      const auto &animJson = enemyJson["animation"];
      type.animation.frameW = type.frame_w;
      type.animation.frameH = type.frame_h;
      type.animation.frameCount = animJson.value("frame_count", 1);
      type.animation.fps = animJson.value("frame_time", 0.1f) > 0
                               ? 1.0f / animJson.value("frame_time", 0.1f)
                               : 12;
      type.animation.startX = type.frame_x;
      type.animation.startY = type.frame_y;
    }

    type.pattern = enemyJson.value("pattern", "chase_player");
    type.health = enemyJson.value("health", 100);
    type.speed = enemyJson.value("speed", 100.0f);
    type.damage = enemyJson.value("damage", 10);
    type.points = enemyJson.value("points", 10);

    _enemyTypes[type.type] = type;
  }
}

void AISystem::updateLevelTimer(double dt) {
  _levelTimer += dt;

  if (_levelTimer >= _levels[_currentLevelIndex].duration) {
    _levelActive = false;
    std::cout << "level_t " << _levels[_currentLevelIndex].name << " completed!"
              << std::endl;

    if (_currentLevelIndex + 1 < _levels.size()) {
      startLevel(_currentLevelIndex + 1);
    }
  }
}

void AISystem::checkEnemySpawns(Ecs::Registry &registry, double dt) {
  if (_currentLevelIndex >= _levels.size())
    return;

  level_t &currentLevel = _levels[_currentLevelIndex];

  for (auto &spawn : currentLevel.enemies) {
    if (!spawn.spawned && _levelTimer >= spawn.spawn_time) {
      auto it = _enemyTypes.find(spawn.type);
      if (it != _enemyTypes.end()) {
        spawnEnemy(registry, spawn, it->second);
        spawn.spawned = true;
      }
    }
  }
}

void AISystem::loadPatterns(const nlohmann::json &config) {
  if (!config.contains("patterns"))
    return;

  for (const auto &patternJson : config["patterns"]) {
    pattern_t pattern;
    pattern.name = patternJson.value("name", "unknown");
    pattern.type = patternJson.value("type", "linear");
    pattern.velocity_x = patternJson.value("velocity_x", 0.0f);
    pattern.velocity_y = patternJson.value("velocity_y", 0.0f);
    pattern.amplitude = patternJson.value("amplitude", 0.0f);
    pattern.frequency = patternJson.value("frequency", 0.0f);
    pattern.chase_speed = patternJson.value("chase_speed", 0.0f);
    pattern.max_distance = patternJson.value("max_distance", 0.0f);
    pattern.shoot_interval = patternJson.value("shoot_interval", 0.0f);

    _patterns[pattern.name] = pattern;
  }
}

void AISystem::updateEnemyMovements(Ecs::Registry &registry, double dt) {
  auto &positions = registry.getComponents<Component::position_t>();
  auto &velocities = registry.getComponents<Component::velocity_t>();
  auto &enemyAIs = registry.getComponents<Component::enemy_ai_t>();

  for (size_t i = 0;
       i < positions.size() && i < velocities.size() && i < enemyAIs.size();
       ++i) {
    if (!positions[i].has_value() || !velocities[i].has_value() ||
        !enemyAIs[i].has_value()) {
      continue;
    }

    auto &pos = positions[i].value();
    auto &vel = velocities[i].value();
    auto &ai = enemyAIs[i].value();

    switch (ai.pattern) {
    case 0:
      vel.vx = -100.0f;
      vel.vy = std::sin(ai.cooldown * 2.0f) * 20.0f;
      break;

    case 1:
      vel.vx = -90.0f;
      vel.vy = std::sin(ai.cooldown * 3.0f) * 60.0f;
      break;

    case 2:
      vel.vx = -80.0f;
      vel.vy = std::sin(ai.cooldown * 1.5f) * 40.0f;
      break;

    case 3:
      vel.vx = -50.0f;
      vel.vy = std::sin(ai.cooldown * 0.8f) * 30.0f;
      break;

    default:
      vel.vx = -100.0f;
      vel.vy = 0.0f;
      break;
    }

    ai.cooldown += dt;
  }
}

int AISystem::getPatternId(const std::string &pattern) {
  static std::unordered_map<std::string, int> patternMap = {
      {"chase_player", 0}, {"zigzag", 1}, {"sine", 2}, {"boss_pattern", 3}};

  auto it = patternMap.find(pattern);
  return it != patternMap.end() ? it->second : 0;
}

extern "C" std::shared_ptr<System::ISystem> createAISystem(std::any params) {
  try {
    auto vec = std::any_cast<std::vector<std::any>>(params);
    auto aiSystem = std::make_shared<System::AISystem>();

    if (!vec.empty()) {
      try {
        auto config = std::any_cast<nlohmann::json>(vec[0]);
        aiSystem->loadConfig(config);
      } catch (const std::exception &e) {
        std::cerr << "Failed to load config in AISystem: " << e.what()
                  << std::endl;
      }
    }

    return aiSystem;
  } catch (const std::exception &e) {
    std::cerr << "Failed to create AISystem: " << e.what() << std::endl;
  }
  return nullptr;
}

} // namespace System
