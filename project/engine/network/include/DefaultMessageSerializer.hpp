/*
** EPITECH PROJECT, 2025
** repo
** File description:
** DefaultSerializerMessage
*/

#pragma once

#include "IMessageSerializer.hpp"
#include "Packets.hpp"

namespace Network {

class DefaultMessageSerializer : public IMessageSerializer {
public:
  DefaultMessageSerializer() = default;

  std::vector<uint8_t> serialize(const Network::Packet &pkt) override;

  Network::Packet deserialize(const uint8_t *data, size_t len) override;
};

} // namespace Network