#include <iterator>
#include "CollisionSystem.hpp"

void System::CollisionSystem::update(Ecs::Registry& r, double dt)
{
    std::unordered_map<std::pair<int, int>, std::vector<size_t>, hashmap> col_zone;

    auto& pos_array = r.getComponents<Component::position_t>();
    auto& col_array = r.getComponents<Component::collision_t>();

    for (size_t i = 0; i < pos_array.size(); ++i) {
        if (!pos_array[i].has_value() || !col_array[i].has_value())
            continue;

        const auto& pos = pos_array[i].value();
        int cell_x = static_cast<int>(pos.x / 64);
        int cell_y = static_cast<int>(pos.y / 64);
        col_zone[{cell_x, cell_y}].push_back(i);
    }

    for (auto& [cell, entities] : col_zone) {
        (void)cell;
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                size_t e1 = entities[i];
                size_t e2 = entities[j];

                const auto& pos1 = pos_array[e1].value();
                const auto& pos2 = pos_array[e2].value();
                const auto& col1 = col_array[e1].value();
                const auto& col2 = col_array[e2].value();

                if (check_collision(pos1, col1, pos2, col2)) {
                    collision(r, e1, e2);
                }
            }
        }
    }
}

bool System::CollisionSystem::check_collision(const Component::position_t& p_e1, const Component::collision_t& w_e1,
                                      const Component::position_t& p_e2, const Component::collision_t& w_e2)
{
    return !(p_e1.x + w_e1.width < p_e2.x ||
             p_e1.x > p_e2.x + w_e2.width ||
             p_e1.y + w_e1.height < p_e2.y ||
             p_e1.y > p_e2.y + w_e2.height);
}

void System::CollisionSystem::collision(Ecs::Registry& r, size_t e1, size_t e2)
{
    auto& col_array = r.getComponents<Component::collision_t>();
    auto& type_array = r.getComponents<Component::collision_type_t>();
    auto& health_array = r.getComponents<Component::healt_t>();
    auto& pos_array = r.getComponents<Component::position_t>();
    auto& vel_array = r.getComponents<Component::velocity_t>();
    auto type1 = type_array[e1].has_value() ? type_array[e1].value().type : Component::CollisionType::None;
    auto type2 = type_array[e2].has_value() ? type_array[e2].value().type : Component::CollisionType::None;

    if ((type1 == Component::CollisionType::Player && type2 == Component::CollisionType::Enemy) ||
        (type1 == Component::CollisionType::Enemy && type2 == Component::CollisionType::Player) ||
        (type1 == Component::CollisionType::Player && type2 == Component::CollisionType::Projectile) ||
        (type1 == Component::CollisionType::Projectile && type2 == Component::CollisionType::Player))
    {
        size_t player = (type1 == Component::CollisionType::Player) ? e1 : e2;
        if (health_array[player].has_value())
            health_array[player].value().healt -= 10;
    }
    if ((type1 == Component::CollisionType::Projectile && type2 == Component::CollisionType::Enemy) ||
        (type1 == Component::CollisionType::Enemy && type2 == Component::CollisionType::Projectile))
    {
        size_t enemy = (type1 == Component::CollisionType::Enemy) ? e1 : e2;
        if (health_array[enemy].has_value())
            health_array[enemy].value().healt -= 10;

        size_t projectile = (type1 == Component::CollisionType::Projectile) ? e1 : e2;
        pos_array[projectile].reset();
        vel_array[projectile].reset();
        col_array[projectile].reset();
    }
    if (type1 == Component::CollisionType::Player && type2 == Component::CollisionType::Wall)
        blockPlayer(pos_array[e1].value(), vel_array[e1].value(), pos_array[e2].value(), col_array[e2].value());
    else if (type2 == Component::CollisionType::Player && type1 == Component::CollisionType::Wall)
        blockPlayer(pos_array[e2].value(), vel_array[e2].value(), pos_array[e1].value(), col_array[e1].value());
}

void System::CollisionSystem::blockPlayer(Component::position_t& player_pos, Component::velocity_t& player_vel,
                                  const Component::position_t& wall_pos, const Component::collision_t& wall_col)
{
    float player_left   = player_pos.x;
    float player_right  = player_pos.x + 32;
    float player_top    = player_pos.y;
    float player_bottom = player_pos.y + 32;

    float wall_left   = wall_pos.x;
    float wall_right  = wall_pos.x + wall_col.width;
    float wall_top    = wall_pos.y;
    float wall_bottom = wall_pos.y + wall_col.height;

    if (player_right > wall_left && player_left < wall_right) {
        if (player_vel.vx > 0)
            player_pos.x = wall_left - 32;
        else if (player_vel.vx < 0)
            player_pos.x = wall_right;
        player_vel.vx = 0;
    }

    if (player_bottom > wall_top && player_top < wall_bottom) {
        if (player_vel.vy > 0)
            player_pos.y = wall_top - 32;
        else if (player_vel.vy < 0)
            player_pos.y = wall_bottom;
        player_vel.vy = 0;
    }
}

extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
{
    try {
        return std::make_shared<System::CollisionSystem>();
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
    }
    return nullptr;
}