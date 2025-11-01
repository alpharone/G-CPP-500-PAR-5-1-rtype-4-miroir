#include "AISystem.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

System::IaSystem::IaSystem(const std::string& configPath, Ecs::Entity target) : _target(target)
{
    loadConfig(configPath);
}

void System::IaSystem::init(Ecs::Registry& registry)
{
    _entity = registry.spawnEntity();
    float spawnX = 0.f, spawnY = 200.f;

    for (const auto& s : _states) {
        if (s.name == "spawnX")
            spawnX = s.speed;
        if (s.name == "spawnY")
            spawnY = s.amplitude;
    }

    registry.emplaceComponent<Component::position_t>(_entity, Component::position_t{spawnX, spawnY});
    registry.emplaceComponent<Component::velocity_t>(_entity, Component::velocity_t{0, 0});
    registry.emplaceComponent<Component::healt_t>(_entity, Component::healt_t{10});
}

void System::IaSystem::shutdown()
{}

void System::IaSystem::loadConfig(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;
        if (line.find("[state]") != std::string::npos)
            parseState(file);
    }
}

void System::IaSystem::parseState(std::ifstream& file)
{
    State state;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;
        if (line.find("[state]") != std::string::npos) {
            file.seekg(-static_cast<int>(line.size()) - 1, std::ios::cur);
            break;
        }
        std::istringstream iss(line);
        std::string key, value;

        if (!(std::getline(iss, key, '=') && std::getline(iss, value)))
            continue;

        if (key == "name") state.name = value;
        else if (key == "movement") state.movement = value;
        else if (key == "shoot") state.shoot = value;
        else if (key == "speed") state.speed = std::stof(value);
        else if (key == "amplitude") state.amplitude = std::stof(value);
        else if (key == "frequency") state.frequency = std::stof(value);
        else if (key == "duration") state.duration = std::stof(value);
        else if (key == "next") state.next = value;
        else if (key == "condition") state.condition = value;
    }

    _states.push_back(state);
}

void System::IaSystem::update(Ecs::Registry& reg, double dt)
{
    _time += dt;
    _stateTimer += dt;

    if (_states.empty())
        return;
    State& current = _states[_currentStateIndex];

    for (auto& [key, val] : _shootCooldowns)
        if (val > 0.f)
            val -= dt;

    executeState(reg, current, dt);

    if ((current.duration > 0.f && _stateTimer >= current.duration) ||
        checkCondition(reg, current))
    {
        for (size_t i = 0; i < _states.size(); ++i) {
            if (_states[i].name == current.next) {
                _currentStateIndex = i;
                _stateTimer = 0.f;
                break;
            }
        }
    }
}

void System::IaSystem::executeState(Ecs::Registry& reg, State& s, double dt)
{
    if (s.movement == "Chaser")
        moveChaser(reg, dt);
    else if (s.movement == "Sine")
        moveSine(reg, dt);
    else if (s.movement == "Zigzag")
        moveZigzag(reg, dt);
    else if (s.movement == "Charge")
        moveCharge(reg, dt);

    if (s.shoot == "Direct")
        shootDirect(reg);
    else if (s.shoot == "Homing")
        shootHoming(reg);
    else if (s.shoot == "Spread")
        shootSpread(reg);
}

bool System::IaSystem::checkCondition(Ecs::Registry& reg, State& s)
{
    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    auto& playerOpt = reg.getComponents<Component::position_t>()[_target];
    auto& hpOpt = reg.getComponents<Component::healt_t>()[_entity];

    if (!posOpt.has_value() || !playerOpt.has_value() || !hpOpt.has_value())
        return false;

    const Component::position_t& pos = *posOpt;
    const Component::position_t& player = *playerOpt;
    const Component::healt_t& hp = *hpOpt;

    float dx = player.x - pos.x;
    float dy = player.y - pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (s.condition.find("distance<") != std::string::npos) {
        float val = std::stof(s.condition.substr(9));
        return dist < val;
    }
    if (s.condition.find("distance>") != std::string::npos) {
        float val = std::stof(s.condition.substr(9));
        return dist > val;
    }
    if (s.condition.find("hp<") != std::string::npos) {
        int val = std::stoi(s.condition.substr(3));
        return hp.healt < val;
    }
    if (s.condition.find("hp>") != std::string::npos) {
        int val = std::stoi(s.condition.substr(3));
        return hp.healt > val;
    }

    return false;
}

void System::IaSystem::moveChaser(Ecs::Registry& reg, double dt)
{
    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    auto& velOpt = reg.getComponents<Component::velocity_t>()[_entity];
    if (!posOpt || !velOpt)
        return;

    auto& pos = *posOpt;
    auto& vel = *velOpt;

    vel.vx = 50.f * dt;
    pos.x += vel.vx;
}

void System::IaSystem::moveSine(Ecs::Registry& reg, double dt)
{
    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    if (!posOpt)
        return;
    auto& pos = *posOpt;

    pos.x += 50.f * dt;
    pos.y = 200 + 30.f * std::sin(_time * 2);
}

void System::IaSystem::moveZigzag(Ecs::Registry& reg, double dt)
{
    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    if (!posOpt)
        return;
    auto& pos = *posOpt;

    pos.x += 80.f * dt;
    pos.y += std::sin(_time * 8) * 60.f * dt;
}

void System::IaSystem::moveCharge(Ecs::Registry& reg, double dt)
{
    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    auto& playerOpt = reg.getComponents<Component::position_t>()[_target];
    if (!posOpt || !playerOpt)
        return;

    auto& pos = *posOpt;
    auto& player = *playerOpt;

    float dx = player.x - pos.x;
    float dy = player.y - pos.y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len > 0) {
        pos.x += (dx / len) * 100.f * dt;
        pos.y += (dy / len) * 100.f * dt;
    }
}

void System::IaSystem::shootDirect(Ecs::Registry& reg)
{
    if (_shootCooldowns["Direct"] > 0.f)
        return;
    _shootCooldowns["Direct"] = 0.5f;

    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    if (!posOpt)
        return;
    auto& pos = *posOpt;

    Ecs::Entity bullet = reg.spawnEntity();
    reg.emplaceComponent<Component::position_t>(bullet, Component::position_t{pos.x, pos.y});
    reg.emplaceComponent<Component::velocity_t>(bullet, Component::velocity_t{300.f, 0.f});
}

void System::IaSystem::shootHoming(Ecs::Registry& reg)
{
    if (_shootCooldowns["Homing"] > 0.f)
        return;
    _shootCooldowns["Homing"] = 1.f;

    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    auto& playerOpt = reg.getComponents<Component::position_t>()[_target];
    if (!posOpt || !playerOpt)
        return;

    auto& pos = *posOpt;
    auto& player = *playerOpt;

    float dx = player.x - pos.x;
    float dy = player.y - pos.y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len == 0)
        return;
    dx /= len;
    dy /= len;

    Ecs::Entity bullet = reg.spawnEntity();
    reg.emplaceComponent<Component::position_t>(bullet, Component::position_t{pos.x, pos.y});
    reg.emplaceComponent<Component::velocity_t>(bullet, Component::velocity_t{dx * 200.f, dy * 200.f});
}

void System::IaSystem::shootSpread(Ecs::Registry& reg)
{
    if (_shootCooldowns["Spread"] > 0.f)
        return;
    _shootCooldowns["Spread"] = 2.f;

    auto& posOpt = reg.getComponents<Component::position_t>()[_entity];
    if (!posOpt)
        return;
    auto& pos = *posOpt;

    int count = 5;
    float range = 45.f;
    float step = range / (count - 1);
    float base = -range / 2.f;
    float speed = 250.f;

    for (int i = 0; i < count; ++i) {
        float angle = (base + i * step) * 3.14159265f / 180.f;
        Ecs::Entity bullet = reg.spawnEntity();
        reg.emplaceComponent<Component::position_t>(bullet, Component::position_t{pos.x, pos.y});
        reg.emplaceComponent<Component::velocity_t>(bullet, Component::velocity_t{
            std::cos(angle) * speed,
            std::sin(angle) * speed
        });
    }
}

// extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
// {
//     try {
//         return std::make_shared<System::PhysicsSystem>();
//     } catch (const std::exception& e) {
//         Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
//     }
//     return nullptr;
// }
