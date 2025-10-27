/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ServerNetworkSystem
*/

#pragma once

#include "ISystem.hpp"
#include "Logger.hpp"
#include "NetworkContext.hpp"
#include "ReliableLayerAdapter.hpp"
#include "SnapshotInterpolator.hpp"
#include <any>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace System {

class ClientNetworkSystem : public ISystem {
public:
  ClientNetworkSystem(const std::string &host, unsigned short port,
                      std::shared_ptr<Network::network_context_t> ctx,
                      const std::string &sprite);
  ~ClientNetworkSystem() override = default;

  void init(Ecs::Registry &registry) override;
  void update(Ecs::Registry &registry, double dt) override;
  void shutdown() override;

private:
  void onAppPacket(const Network::Packet &pkt, const Network::endpoint_t &from);
  void handleAcceptClient(const Network::Packet &pkt,
                          const Network::endpoint_t &from);
  void handleEntitySpawn(const Network::Packet &pkt,
                         const Network::endpoint_t &from);
  void handleEntityDespawn(const Network::Packet &pkt,
                           const Network::endpoint_t &from);
  void handleGameStart(const Network::Packet &pkt,
                       const Network::endpoint_t &from);
  void handleServerSnapshot(const Network::Packet &pkt,
                            const Network::endpoint_t &);

  using HandlerFn =
      std::function<void(const Network::Packet &, const Network::endpoint_t &)>;
  std::unordered_map<uint16_t, HandlerFn> _handlers;

  std::shared_ptr<Network::INetworkTransport> _transport;
  std::shared_ptr<Network::IMessageSerializer> _serializer;
  std::shared_ptr<Network::ReliableLayerAdapter> _adapter;
  std::shared_ptr<Network::network_context_t> _ctx;
  Network::endpoint_t _server;
  Network::SnapshotInterpolator _interpolator;
  std::chrono::steady_clock::time_point _startTime =
      std::chrono::steady_clock::now();

  std::unordered_map<uint32_t, size_t> _entityMap;

  std::chrono::steady_clock::time_point _lastPacketTime;
  bool _reconnecting{false};

  std::atomic<bool> _initialized{false};
  std::mutex _mutex;
  std::string _player_sprite;
  bool _timeSynced{false};
};

} // namespace System
