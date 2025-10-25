/*
** EPITECH PROJECT, 2025
** repo
** File description:
** DefaultMessageSerializer.cpp
*/

#include "DefaultMessageSerializer.hpp"

std::vector<uint8_t>
Network::DefaultMessageSerializer::serialize(const Network::Packet &pkt) {
  return pkt.serialize();
}

Network::Packet
Network::DefaultMessageSerializer::deserialize(const uint8_t *data,
                                               size_t len) {
  return Network::Packet::deserialize(data, len);
}
