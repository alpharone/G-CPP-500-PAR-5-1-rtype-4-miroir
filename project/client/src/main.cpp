/*
** EPITECH PROJECT, 2025
** Client
** File description:
** main.cpp
*/

#include "GameRunner.hpp"
#include "Logger.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    Logger::info("Usage:\t./r-type_client <config_file>.json");
    return 84;
  }

  std::string configFile = argv[1];

  Logger::init("logs/client.log");
  std::cout << "Starting R-Type client with config: " << configFile
            << std::endl;

  try {
    GameRunner game(configFile);
    game.run();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    Logger::shutdown();
    return 1;
  }

  Logger::shutdown();
  return 0;
}
