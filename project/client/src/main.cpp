/*
** EPITECH PROJECT, 2025
** Client
** File description:
** main.cpp
*/

#include "ClientApp.hpp"

int main()
{
    ClientApp app;

    if (!app.init())
        return -1;
    app.mainLoop();
    app.shutdown();
    return 0;
}
