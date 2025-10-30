#pragma once

#include <memory>
#include <string>

namespace Network {
struct network_context_t;
}

namespace System {

struct ClientNetworkConfig {
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

} // namespace System
