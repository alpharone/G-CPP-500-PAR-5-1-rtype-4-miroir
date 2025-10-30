/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Room
*/

#pragma once

#include "Packets.hpp"
#include <asio.hpp>
#include <chrono>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using asio::ip::udp;

namespace Network {

struct player_input_t {
  uint32_t id;
  float dx;
  float dy;
};

struct endpoint_t {
  std::string address;
  uint16_t port;
  bool operator==(endpoint_t const &o) const {
    return address == o.address && port == o.port;
  }
};

struct room_client_t {
  uint32_t clientId;
  endpoint_t endpoint;
  bool ready{false};
  std::chrono::steady_clock::time_point lastSeen;
  float posX{100};
  float posY{100};
  float lastSnapX{100};
  float lastSnapY{100};
  std::unordered_set<uint8_t> pressedKeys;
  std::string sprite;
  int frame_x{0};
  int frame_y{0};
  int frame_w{0};
  int frame_h{0};
  int frame_count{0};
  float frame_time{0.0f};
};

struct pending_input_t {
  uint32_t clientId;
  std::vector<uint8_t> payload;
};

struct room_t {
  uint32_t id;
  uint32_t nextClientId{1};
  std::unordered_map<uint32_t, room_client_t> clients;
  std::vector<pending_input_t> pendingInputs;
  std::vector<Network::Packet> outgoing;
  bool inGame{false};
  std::mutex mtx;
  std::chrono::steady_clock::time_point lastEmpty;
  double snapshotTimer = 0.0;

  room_t(uint32_t roomId = 0)
      : id(roomId), lastEmpty(std::chrono::steady_clock::now()) {}

  room_t(room_t &&other) noexcept
      : id(other.id), nextClientId(other.nextClientId),
        clients(std::move(other.clients)),
        pendingInputs(std::move(other.pendingInputs)),
        outgoing(std::move(other.outgoing)), inGame(other.inGame) {}

  room_t &operator=(room_t &&other) noexcept {
    if (this != &other) {
      id = other.id;
      nextClientId = other.nextClientId;
      clients = std::move(other.clients);
      pendingInputs = std::move(other.pendingInputs);
      outgoing = std::move(other.outgoing);
      inGame = other.inGame;
    }
    return *this;
  }

  room_t(const room_t &) = delete;
  room_t &operator=(const room_t &) = delete;
};
} // namespace Network
