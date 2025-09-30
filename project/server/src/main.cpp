/*
** EPITECH PROJECT, 2025
** repo
** File description:
** main
*/

#include <iostream>
#include <asio.hpp>
#include "Server.hpp"

int main()
{
    try {
        asio::io_context ctx;
        Server::Server s(ctx, 4242);
        ctx.run();
    } catch (std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
    }
}
