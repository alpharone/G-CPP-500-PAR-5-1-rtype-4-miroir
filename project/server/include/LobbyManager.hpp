/*
** EPITECH PROJECT, 2025
** repo
** File description:
** LobbyManager
*/

#pragma once
#include <map>
#include <vector>
#include "Connection.hpp"

namespace Server {

    struct Room {
        std::vector<Connection*> players;
        bool running{false};

        void addPlayer(Connection* c);
    };

    class LobbyManager {
        public:
            LobbyManager() = default;
            ~LobbyManager() = default;

            Room& createRoom();
            Room& getRoom(int id);

        private:
            int _nextId{1};
            std::map<int, Room> _rooms;
    };

}