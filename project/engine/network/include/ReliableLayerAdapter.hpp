/*
** EPITECH PROJECT, 2025
** repo
** File description:
** ReliableLayerAdapter
*/

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "IMessageSerializer.hpp"
#include "INetworkTransport.hpp"
#include "Packets.hpp"
#include "ReliableChannel.hpp"

namespace Network {

class ReliableLayerAdapter {
public:
  using AppPacketCallback =
      std::function<void(const Network::Packet &, const endpoint_t &)>;

  ReliableLayerAdapter(std::shared_ptr<INetworkTransport> transport,
                       std::shared_ptr<IMessageSerializer> serializer,
                       size_t mtu = 1200);

  ~ReliableLayerAdapter() = default;

  void tick(double dt);
  uint32_t sendReliable(const endpoint_t &to, const Network::Packet &pkt);
  void sendUnreliable(const endpoint_t &to, const Network::Packet &pkt);
  void setAppPacketCallback(AppPacketCallback cb);

private:
  void onTransportRecv(const std::vector<uint8_t> &data,
                       const endpoint_t &from);

  struct peer_channel_t {
    ReliableChannel channel;
    std::chrono::steady_clock::time_point lastActive;
    endpoint_t endpoint;
    peer_channel_t(size_t mtu = 1200) : channel(mtu) {}
  };

  struct fragment_buffer_t {
    uint16_t total{0};
    std::vector<Network::Packet> fragments;
  };

  std::shared_ptr<INetworkTransport> _transport;
  std::shared_ptr<IMessageSerializer> _serializer;

  std::unordered_map<uint32_t, fragment_buffer_t> _fragments;

  std::unordered_map<std::string, std::shared_ptr<peer_channel_t>> _peers;

  std::mutex _mtx;
  std::mutex _mtxFragments;

  AppPacketCallback _deliverCb;
  size_t _mtu;
};

} // namespace Network
