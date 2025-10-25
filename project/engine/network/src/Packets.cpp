/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Packet
*/

#include "Packets.hpp"

#include "Utils.hpp"

std::vector<uint8_t> Network::Packet::serialize() const {
  std::vector<uint8_t> out;
  out.reserve(8 + payload.size());

  write_u8(out, header.type);
  write_u16_le(out, static_cast<uint16_t>(payload.size()));
  write_u32_le(out, header.seq);
  write_u8(out, header.flags);

  out.insert(out.end(), payload.begin(), payload.end());
  return out;
}

Network::Packet Network::Packet::deserialize(const uint8_t *data, size_t len) {
  const size_t HEADER_SIZE = 8;
  if (len < HEADER_SIZE) {
    throw std::runtime_error("deserialize: packet too small");
  }
  Packet p;
  size_t offset = 0;

  p.header.type = read_u8(data, len, offset);
  offset += 1;

  p.header.length = read_u16_le(data, len, offset);
  offset += 2;

  p.header.seq = read_u32_le(data, len, offset);
  offset += 4;

  p.header.flags = read_u8(data, len, offset);
  offset += 1;

  if (len - offset < p.header.length)
    throw std::runtime_error("deserialize: invalid payload length");

  p.payload.assign(data + offset, data + offset + p.header.length);
  return p;
}
