/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** Registry
*/

#pragma once

#include <unordered_map>
#include <typeindex>
#include <any>
#include <functional>
#include <vector>
#include <stdexcept>
#include "Entity.hpp"
#include "SparseArray.hpp"

namespace Ecs {
    
    class Registry {
        public:
            Registry() = default;
            
            Entity spawnEntity();
            void killEntity(Entity const& Entity);
            void runSystems();

            template <typename Component>
            SparseArray<Component>& registerComponent()
            {
                std::type_index key(typeid(Component));
                if (_components.find(key) == _components.end()) {
                    _components.emplace(key, SparseArray<Component>{});
                    _erase_callbacks.push_back([](Registry& r, Entity const& e){
                        r.removeComponent<Component>(e);
                    });
                }
                return std::any_cast<SparseArray<Component>&>(_components.at(key));
            }
        
            template <typename Component>
            SparseArray<Component>& getComponents()
            {
                std::type_index key(typeid(Component));
                if (_components.find(key) == _components.end())
                    return registerComponent<Component>();
                return std::any_cast<SparseArray<Component>&>(_components.at(key));
            }
        
            template <typename Component>
            SparseArray<Component> const& getComponents() const
            {
                std::type_index key(typeid(Component));
                return std::any_cast<SparseArray<Component> const&>(_components.at(key));
            }
        
            template <typename Component, typename... Params>
            auto emplaceComponent(Entity const& e, Params&&... params)
            {
                return getComponents<Component>().emplace_at(static_cast<size_t>(e), std::forward<Params>(params)...);
            }
        
            template <typename Component>
            void removeComponent(Entity const& e)
            {
                getComponents<Component>().erase(static_cast<size_t>(e));
            }
        
            template <class... Components, typename Function>
            void addSystem(Function&& f)
            {
                _systems.emplace_back([fn = std::forward<Function>(f)](Registry& r){
                    fn(r, r.template getComponents<Components>()...);
                });
            }

        private:
            size_t _next_id{0};
            std::vector<size_t> _free_ids;
            std::unordered_map<std::type_index, std::any> _components;
            std::vector<std::function<void(Registry&, Entity const&)>> _erase_callbacks;
            std::vector<std::function<void(Registry&)>> _systems;
    };

}