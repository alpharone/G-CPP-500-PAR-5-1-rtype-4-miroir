/*
** EPITECH PROJECT, 2025
** repo
** File description:
** RoomManager
*/

#pragma once

#include "Packets.hpp"
#include "Room.hpp"
#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

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

  RoomManager(float start_x, float start_y, float speed, int max_players,
              int max_rooms, int timeout, int screen_width, int screen_height,
              double snapshot_interval,
              std::chrono::steady_clock::time_point start_time);
  ~RoomManager();

  uint32_t createRoom();
  std::optional<std::pair<uint32_t, uint32_t>>
  joinAuto(const endpoint_t &endpoint, const std::string &sprite);
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
  float _start_x;
  float _start_y;
  float _speed;
  int _max_players;
  int _max_rooms;
  int _timeout;
  int _screen_width;
  int _screen_height;
  double _snapshot_interval;
  std::chrono::steady_clock::time_point _start_time;
};

} // namespace Network
