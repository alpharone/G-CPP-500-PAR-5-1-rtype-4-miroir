#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include "ISystem.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Registry.hpp"
#include "Friction.hpp"
#include "Logger.hpp"



namespace System {

class PositionSystem : public ISystem {
    public:
    PositionSystem() = default;
    ~PositionSystem() override = default;
    void init(Ecs::Registry& registry) override;
    void update(Ecs::Registry& registry, double dt);
    void shutdown() override;
    private:
};
}