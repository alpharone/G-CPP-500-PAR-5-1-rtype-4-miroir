/*
** EPITECH PROJECT, 2025
** repo
** File description:
** IReliableLayer
*/

#pragma once

#include <cstdint>
#include <vector>

#include "INetworkTransport.hpp"
#include "Packets.hpp"

namespace Network {

class IReliableLayer {
public:
  virtual ~IReliableLayer() = default;
  virtual uint32_t sendReliable(const endpoint_t &to,
                                const Network::Packet &p) = 0;
  virtual void sendUnreliable(const endpoint_t &to,
                              const Network::Packet &p) = 0;
  virtual void onRecv(const endpoint_t &from, const Network::Packet &p) = 0;
  virtual void tick(double dt) = 0;
};

} // namespace Network