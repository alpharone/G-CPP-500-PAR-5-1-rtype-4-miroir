/*
** EPITECH PROJECT, 2025
** repo
** File description:
** NetworkContext
*/

#pragma once

#include <asio.hpp>
#include <memory>

struct NetworkContext {
    std::shared_ptr<asio::ip::udp::socket> socket;
    asio::ip::udp::endpoint serverEndpoint;
    uint32_t playerId = UINT32_MAX;
};
