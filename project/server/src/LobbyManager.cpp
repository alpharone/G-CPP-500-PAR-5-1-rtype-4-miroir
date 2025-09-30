/*
** EPITECH PROJECT, 2025
** repo
** File description:
** LobbyManager
*/

#include "LobbyManager.hpp"

void Server::Room::addPlayer(Server::Connection* c)
{
    players.push_back(c);
}

Server::Room& Server::LobbyManager::createRoom()
{
    int id = _nextId;

    _nextId += 1;
    return _rooms[id];
}

Server::Room& Server::LobbyManager::getRoom(int id)
{
    return _rooms.at(id);
}