/*
** EPITECH PROJECT, 2025
** repo
** File description:
** AsioNetworkTransport
*/

#include "AsioNetworkTransport.hpp"
#include "Logger.hpp"

using asio::ip::udp;

namespace Network {

    AsioNetworkTransport::AsioNetworkTransport(const std::string& bindAddress, uint16_t bindPort)
        : _bindAddress(bindAddress),
          _bindPort(bindPort),
          _socket(_io),
          _workGuard(asio::make_work_guard(_io))
    {
        try {
            udp::endpoint ep(asio::ip::make_address(bindAddress), bindPort);
            _socket.open(ep.protocol());
            _socket.bind(ep);
            Logger::info("[AsioTransport] Bound to " + bindAddress + ":" + std::to_string(bindPort));
        } catch (const std::exception& e) {
            Logger::error(std::string("[AsioTransport] Failed to bind socket: ") + e.what());
            throw;
        }
    }

    AsioNetworkTransport::~AsioNetworkTransport()
    {
        stop();
    }

    void AsioNetworkTransport::start()
    {
        if (_running.exchange(true))
            return;

        Logger::info("[AsioTransport] Starting IO thread...");
        doReceive();

        _thread = std::thread([this]() {
            try {
                _io.run();
            } catch (const std::exception& e) {
                Logger::error(std::string("[AsioTransport] IO thread crashed: ") + e.what());
            }
        });
    }

    void AsioNetworkTransport::stop()
    {
        if (!_running.exchange(false))
            return;

        Logger::info("[AsioTransport] Stopping transport...");

        try {
            _io.post([this]() {
                std::lock_guard<std::mutex> lk(_sendQueueMtx);
                _sendQueue.clear();
            });

            _workGuard.reset();

            asio::error_code ec;
            _socket.close(ec);
            if (ec) {
                Logger::warn("[AsioTransport] Socket close error: " + ec.message());
            }

            _io.stop();

            if (_thread.joinable())
                _thread.join();

            Logger::info("[AsioTransport] Stopped cleanly.");
        } catch (const std::exception& e) {
            Logger::error(std::string("[AsioTransport] Exception in stop(): ") + e.what());
        }
    }

    void AsioNetworkTransport::setRecvCallback(RecvCallback cb)
    {
        _cb = std::move(cb);
    }

    void AsioNetworkTransport::safePost(std::function<void()> fn)
    {
        if (_running)
            asio::post(_io, std::move(fn));
    }

    void AsioNetworkTransport::sendTo(const endpoint_t& to, const std::vector<uint8_t>& data)
    {
        if (!_running)
            return;

        auto bufPtr = std::make_shared<std::vector<uint8_t>>(data);
        udp::endpoint ep(asio::ip::make_address(to.address), to.port);

        safePost([this, bufPtr, ep]() {
            bool startSend = false;
            {
                std::lock_guard<std::mutex> lk(_sendQueueMtx);
                startSend = _sendQueue.empty();
                _sendQueue.emplace_back(ep, bufPtr);
            }

            if (startSend)
                doSendNext();
        });
    }

    void AsioNetworkTransport::doSendNext()
    {
        std::pair<udp::endpoint, std::shared_ptr<std::vector<uint8_t>>> item;
        {
            std::lock_guard<std::mutex> lk(_sendQueueMtx);
            if (_sendQueue.empty())
                return;
            item = _sendQueue.front();
        }

        auto ep = item.first;
        auto bufPtr = item.second;

        _socket.async_send_to(
            asio::buffer(*bufPtr), ep,
            [this, bufPtr](std::error_code ec, std::size_t bytes_sent) {
                bool hasNext = false;
                {
                    std::lock_guard<std::mutex> lk(_sendQueueMtx);
                    if (!_sendQueue.empty())
                        _sendQueue.pop_front();
                    hasNext = !_sendQueue.empty();
                }

                if (hasNext)
                    safePost([this]() { doSendNext(); });
            });
    }

    void AsioNetworkTransport::doReceive()
    {
        _socket.async_receive_from(
            asio::buffer(_recvBuf), _remoteEndpoint,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if (!_running)
                    return;

                if (!ec && bytes_recvd > 0) {
                    endpoint_t e{_remoteEndpoint.address().to_string(),
                                 static_cast<uint16_t>(_remoteEndpoint.port())};

                    if (_cb) {
                        std::vector<uint8_t> data(_recvBuf.data(), _recvBuf.data() + bytes_recvd);
                        try {
                            _cb(data, e);
                        } catch (const std::exception& ex) {
                            Logger::warn(std::string("[AsioTransport] recv callback exception: ") + ex.what());
                        }
                    }
                } else if (ec != asio::error::operation_aborted) {
                    Logger::warn("[AsioTransport] Receive failed: " + ec.message());
                }

                if (_running)
                    doReceive();
            }
        );
    }

}
