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
#include "WindowSize.hpp"
#include "Logger.hpp"

namespace System {

struct hashmap
{
    size_t operator()(std::pair<int, int> p) noexcept{
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second)) << 1;
    }
};


class CollisionSystem : public ISystem {
    public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;
    void update(Registry&r, float dt);
    bool check_collision(position_t p_e1, windowSize_t w_e1, position_t p_e2, windowSize_t w_e2);
    void blockPlayer(position_t& player_pos, Velocity& player_vel, const position_t& wall_pos, const collision_t& wall_col);

    private:
};
}