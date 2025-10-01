/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** Entity
*/

#include "Entity.hpp"

Ecs::Entity::Entity(size_t id) noexcept : _id(id)
{
}

Ecs::Entity::operator size_t() const noexcept
{
    return _id;
}

bool Ecs::Entity::operator==(Entity const& other) const noexcept
{
    return (_id == other._id);
}

bool Ecs::Entity::operator!=(Entity const& other) const noexcept
{
    return (_id != other._id);
}


