#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace Network {
struct network_context_t;
}

namespace System {

struct client_network_config_t {
  std::string host;
  unsigned short port;
  std::shared_ptr<Network::network_context_t> ctx;
  std::string sprite;
  int frameW;
  int frameH;
  int frameCount;
  float frameTime;
  int frameX;
  int frameY;
};

struct client_network_factory_params_t {
  std::string host;
  int port;
  std::shared_ptr<Network::network_context_t> ctx;
  std::string sprite;
  int frameW;
  int frameH;
  int frameCount;
  float frameTime;
  int frameX;
  int frameY;
};

} // namespace System
