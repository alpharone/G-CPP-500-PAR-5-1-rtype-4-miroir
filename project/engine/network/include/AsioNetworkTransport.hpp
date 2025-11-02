/*
** EPITECH PROJECT, 2025
** repo
** File description:
** AsioNetworkTransport
*/

#pragma once

#include "INetworkTransport.hpp"
#include <asio.hpp>
#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>

using asio::ip::udp;

namespace Network {

class AsioNetworkTransport : public INetworkTransport {
public:
  AsioNetworkTransport(const std::string &bindAddress, uint16_t bindPort);
  ~AsioNetworkTransport() override;

  void start() override;
  void stop() override;
  void setRecvCallback(RecvCallback cb) override;
  void sendTo(const endpoint_t &to, const std::vector<uint8_t> &data) override;

private:
  void doReceive();
  void doSendNext();
  void safePost(std::function<void()> fn);

  std::string _bindAddress;
  uint16_t _bindPort;

  asio::io_context _io;
  udp::socket _socket;
  asio::executor_work_guard<asio::io_context::executor_type> _workGuard;
  std::thread _thread;

  RecvCallback _cb;
  std::atomic<bool> _running{false};

  std::array<uint8_t, 2048> _recvBuf{};
  udp::endpoint _remoteEndpoint;

  std::deque<std::pair<udp::endpoint, std::shared_ptr<std::vector<uint8_t>>>>
      _sendQueue;
  std::mutex _sendQueueMtx;
};

} // namespace Network
