/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "ISystem.hpp"
#include "Logger.hpp"
#include "ReliableLayerAdapter.hpp"
#include "RoomManager.hpp"

namespace System {

class ServerNetworkSystem : public ISystem {
public:
  ServerNetworkSystem(unsigned short port);
  ~ServerNetworkSystem() override = default;

  void init(Ecs::Registry &registry) override;
  void update(Ecs::Registry &registry, double dt) override;
  void shutdown() override;

private:
  void onAppPacket(const Network::Packet &pkt, const Network::endpoint_t &from);
  void handleNewClient(const Network::Packet &pkt,
                       const Network::endpoint_t &from);
  void handleDisconnect(const Network::Packet &pkt,
                        const Network::endpoint_t &from);

  using HandlerFn =
      std::function<void(const Network::Packet &, const Network::endpoint_t &)>;
  std::unordered_map<uint16_t, HandlerFn> _handlers;

  std::shared_ptr<Network::INetworkTransport> _transport;
  std::shared_ptr<Network::IMessageSerializer> _serializer;
  std::unique_ptr<Network::ReliableLayerAdapter> _adapter;
  std::unique_ptr<Network::RoomManager> _rooms;

  std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> _knownClients;
  std::mutex _clientsMtx;

  uint32_t _mainRoomId = 0;
  unsigned short _port;
};

} // namespace System
