#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include "ISystem.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Healt.hpp"
#include "Registry.hpp"

// struct Position { float x = 0.f, y = 0.f; };
// struct Velocity { float vx = 0.f, vy = 0.f; };
// struct Health { int hp = 10; };

namespace System {

class IaSystem {
public:
    struct State {
        std::string name;
        std::string movement;
        std::string shoot;
        float speed = 50.f;
        float amplitude = 0.f;
        float frequency = 0.f;
        float duration = -1.f;
        std::string next;
        std::string condition;
    };

private:
    Ecs::Entity _entity;
    Ecs::Entity _target;
    std::vector<State> _states;
    size_t _currentStateIndex = 0;
    float _stateTimer = 0.f;
    float _time = 0.f;
    std::unordered_map<std::string, float> _shootCooldowns;

public:
    IaSystem(const std::string& configPath, Ecs::Entity target);

    void init(Ecs::Registry& registry);
    void update(Ecs::Registry& registry, double dt);
    void shutdown();

private:
    void loadConfig(const std::string& path);
    void parseState(std::ifstream& file);
    void executeState(Ecs::Registry& reg, State& s, double dt);
    bool checkCondition(Ecs::Registry& reg, State& s);
    void moveChaser(Ecs::Registry& reg, double dt);
    void moveSine(Ecs::Registry& reg, double dt);
    void moveZigzag(Ecs::Registry& reg, double dt);
    void moveCharge(Ecs::Registry& reg, double dt);
    void shootDirect(Ecs::Registry& reg);
    void shootHoming(Ecs::Registry& reg);
    void shootSpread(Ecs::Registry& reg);
};
}