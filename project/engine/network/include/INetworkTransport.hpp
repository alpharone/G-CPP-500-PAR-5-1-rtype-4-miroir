/*
** EPITECH PROJECT, 2025
** repo
** File description:
** INetworkTransport
*/

#pragma once
#include "Room.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace Network {

class INetworkTransport {
public:
  using RecvCallback = std::function<void(const std::vector<uint8_t> &data,
                                          const endpoint_t &from)>;

  virtual ~INetworkTransport() = default;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void setRecvCallback(RecvCallback cb) = 0;
  virtual void sendTo(const endpoint_t &to,
                      const std::vector<uint8_t> &data) = 0;
};

} // namespace Network