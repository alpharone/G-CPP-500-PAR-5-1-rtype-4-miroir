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
#include "WIndowSize.hpp"
#include "Logger.hpp"
#include "Collision.hpp"
#include "Healt.hpp"

namespace System {

struct hashmap {
    size_t operator()(const std::pair<int,int>& p) const noexcept {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class CollisionSystem : public ISystem {
    public:
        CollisionSystem() = default;
        ~CollisionSystem() override= default;

        void init(Ecs::Registry& registry) override {};
        void update(Ecs::Registry& r, double dt) override;
        bool check_collision(const Component::position_t& p_e1, const Component::collision_t& w_e1,
                             const Component::position_t& p_e2, const Component::collision_t& w_e2);
        void collision(Ecs::Registry& r, size_t e1, size_t e2);
        void blockPlayer(Component::position_t& player_pos, Component::velocity_t& player_vel,
                         const Component::position_t& wall_pos, const Component::collision_t& wall_col);
        void shutdown() override {};
    private:
};
}