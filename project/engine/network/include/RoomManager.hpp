/*
** EPITECH PROJECT, 2025
** repo
** File description:
** RoomManager
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Packets.hpp"
#include "Room.hpp"

namespace std {
template <> struct hash<Network::endpoint_t> {
  std::size_t operator()(Network::endpoint_t const &e) const noexcept {
    return std::hash<std::string>()(e.address) ^
           (std::hash<uint16_t>()(e.port) << 1);
  }
};
} // namespace std

namespace Network {

class RoomManager {
public:
  using PacketVec = std::vector<Network::Packet>;
  static constexpr int MAX_PLAYERS_PER_ROOM = 4;

  RoomManager();
  ~RoomManager();

  uint32_t createRoom();
  std::optional<std::pair<uint32_t, uint32_t>>
  joinAuto(const endpoint_t &endpoint);
  void leave(uint32_t roomId, uint32_t clientId);
  std::vector<uint32_t> listRooms();

  room_t &getRoom(uint32_t id);
  PacketVec collectOutgoing(uint32_t roomId);
  void onPacket(uint32_t roomId, const endpoint_t &from,
                const Network::Packet &p);
  void tick(double dt);

private:
  std::unordered_map<uint32_t, room_t> _rooms;
  std::mutex _roomsMtx;
  uint32_t _nextRoomId{0};
};

} // namespace Network
