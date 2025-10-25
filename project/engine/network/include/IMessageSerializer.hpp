/*
** EPITECH PROJECT, 2025
** repo
** File description:
** IMessageSerializer
*/

#pragma once

#include <optional>
#include <vector>

#include "Packets.hpp"

namespace Network {

class IMessageSerializer {
public:
  virtual ~IMessageSerializer() = default;
  virtual std::vector<uint8_t> serialize(const Network::Packet &p) = 0;
  virtual Network::Packet deserialize(const uint8_t *data, size_t size) = 0;
};

} // namespace Network