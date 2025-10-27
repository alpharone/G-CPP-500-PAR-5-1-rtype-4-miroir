#include <iterator>
#include "CollisionSystem.hpp"

void System::CollisionSystem::update(Registry& r, float dt)
{

    std::unordered_map<std::pair<int, int>, std::vector<Registry&>, hashmap> col_zone;

    auto& pos_array = r.getComponents<position_t>();

    for (auto& it : pos_array) {
        auto cell_x = (int)(it.x / 64);
        auto cell_y = (int)(it.y / 64);
        col_zone[{cell_x, cell_y}].push_back(r(std::distance(pos_array.begin(), it)));
    }

    for (auto& [cell, entity] : col_zone) {
        for (int first_e = 0; first_e < entity.size(); first_e++) {
            for (int seconde_e = first_e + 1; seconde_e < entity.size(); seconde_e++) {
                auto entity1 = entity.at(first_e);
                auto entity2 = entity.at(seconde_e);
                if (check_collision(pos_array[first_e], r.getComponents<WindowSize_t>()[entity1], pos_array[seconde_e], r.getComponents<WindowSize_t>()[entity2]))
                    collision(r, entity1, entity2);
            }
        }
    }
}

bool System::CollisionSystem::check_collision(position_t p_e1, windowSize_t w_e1, position_t p_e2, windowSize_t w_e2)
{
    return !(p_e1.x + w_e1.width < p_e2.x || 
            p_e1.x > p_e2.x + w_e2.width ||
            p_e1.y + w_e1.height < p_e2.y ||
            p_e1.y > p_e2.y + w_e2.height);
}

void System::CollisionSystem::collision(Registry& r, size_t e1, size_t e2)
{
    auto& col_array = r.getComponents<collision_t>();
    auto& health_array = r.getComponents<health_t>();
    auto& pos_array = r.getComponents<position_t>();
    auto& vel_array = r.getComponents<velocity_t>();

    auto type1 = col_array[e1].value().type;
    auto type2 = col_array[e2].value().type;

    if ((type1 == CollisionType::Player && type2 == CollisionType::Enemy) ||
        (type1 == CollisionType::Enemy && type2 == CollisionType::Player) ||
        (type1 == CollisionType::Player && type2 == CollisionType::Projectile) ||
        (type1 == CollisionType::Projectile && type2 == CollisionType::Player))
    {
        size_t player = (type1 == CollisionType::Player) ? e1 : e2;
        if (health_array[player].has_value())
            health_array[player].value().hp -= 10;
    }

    if ((type1 == CollisionType::Projectile && type2 == CollisionType::Enemy) ||
        (type1 == CollisionType::Enemy && type2 == CollisionType::Projectile))
    {
        size_t enemy = (type1 == CollisionType::Enemy) ? e1 : e2;
        if (health_array[enemy].has_value())
            health_array[enemy].value().hp -= 10;
        
        size_t projectile = (type1 == CollisionType::Projectile) ? e1 : e2;
        pos_array[projectile].reset();
        vel_array[projectile].reset();
        col_array[projectile].reset();
    }

    if (type1 == CollisionType::Player && type2 == CollisionType::Wall)
        blockPlayer(pos_array[e1].value(), vel_array[e1].value(), pos_array[e2].value(), col_array[e2].value());
    else if (type2 == CollisionType::Player && type1 == CollisionType::Wall)
        blockPlayer(pos_array[e2].value(), vel_array[e2].value(), pos_array[e1].value(), col_array[e1].value());
}

void System::CollisionSystem::blockPlayer(position_t& player_pos, Velocity& player_vel, const position_t& wall_pos, const collision_t& wall_col)
{
    float player_left = player_pos.x;
    float player_right = player_pos.x + player_vel.width;
    float player_top = player_pos.y;
    float player_bottom = player_pos.y + player_vel.height;

    float wall_left = wall_pos.x;
    float wall_right = wall_pos.x + wall_col.width;
    float wall_top = wall_pos.y;
    float wall_bottom = wall_pos.y + wall_col.height;

    if (player_right > wall_left && player_left < wall_right) {
        if (player_vel.vx > 0)
            player_pos.x = wall_left - (player_vel.width);
        else if (player_vel.vx < 0)
            player_pos.x = wall_right;
        player_vel.vx = 0;
    }

    if (player_bottom > wall_top && player_top < wall_bottom) {
        if (player_vel.vy > 0)
            player_pos.y = wall_top - player_vel.height;
        else if (player_vel.vy < 0)
            player_pos.y = wallBottom;
        player_vel.vy = 0;
    }
}

extern "C" std::shared_ptr<System::ISystem> createInputSystem(std::any params)
{
    try {
        return std::make_shared<System::CollisionSystem::>();
    } catch (const std::exception& e) {
        Logger::error(std::string("[Factory]: Failed to create InputSystem: ") + e.what());
    }
    return nullptr;
}