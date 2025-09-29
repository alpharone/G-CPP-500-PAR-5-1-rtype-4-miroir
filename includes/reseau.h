#pragma once
#include <sys/epoll.h>
#include <queue>
#include <unordered_map>
#include <functional>

namespace client {
    typedef struct header_s {
        uint8_t type;
        uint16_t length;
        uint32_t seq;
    }header_t __attribute__((packed));

    enum MessageType : uint8_t {
        NEW_CLIENT      = 0x01,
        ACCEPT_CLIENT   = 0x02,
        CLIENT_INPUT    = 0x03,
        SERVER_SNAPSHOT = 0x04,
        ENTITY_SPAWN    = 0x05,
        ENTITY_DESPAWN  = 0x06,
        GAME_EVENT      = 0x07,
        ACK             = 0x08,
        PING            = 0x09,
        PONG            = 0x0A,
        RELIABLE_FRAGMENT   = 0x0B,
        PLAYER_READY    = 0x0C,
        GAME_START      = 0x0D,
        GAME_OVER       = 0x0E,
        ADMIN_TEXT      = 0xFF
    };
    class MessageHeader {
    uint8_t type;
    uint16_t length;
    uint32_t seq;
    uint8_t flags;
  }__attribute__((packed));

  class Packet {
    MessageHeader header;
    std::vector<uint8_t> payload;
    std::vector<uint8_t> serialize() const;
    static Packet deserialize(const std::vector<uint8_t>& data);
  };
    template<typename T>
    class network {
        public:
            network();
            ~network() = default;
            epoll_event setnetword();
            uint32_t new_player();
            ssize_t read_soket(T* buffer, size_t size);
            void send(const Packet& p);
            void rec();
        private:
            int connection;
            int socket_;
            int epoll_file_;
            std::queue<T> queue;
            inline static const std::unordered_map<MessageType, std::function<void()>> code_function {
            { MessageType::NEW_CLIENT,         [](){  } },
            { MessageType::ACCEPT_CLIENT,      [](){ } },
            { MessageType::CLIENT_INPUT,       [](){ } },
            { MessageType::SERVER_SNAPSHOT,   [](){  } },
            { MessageType::ENTITY_SPAWN,       [](){ } },
            { MessageType::ENTITY_DESPAWN,    [](){  } },
            { MessageType::GAME_EVENT,     [](){  } },
            { MessageType::ACK,        [](){  } },
            { MessageType::PING,       [](){  } },
            { MessageType::PONG,       [](){  } },
            { MessageType::RELIABLE_FRAGMENT, [](){  } },
            { MessageType::PLAYER_READY,       [](){ } },
            { MessageType::GAME_START,     [](){  } },
            { MessageType::GAME_OVER,      [](){  } },
            { MessageType::ADMIN_TEXT,     [](){  } },
    };
    };
}