/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Logger
*/

#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>

class Logger {
public:
  enum class Level { DEBUG, INFO, WARN, ERROR };
  static void init(const std::string &logFile = "logs/debug.log");
  static void shutdown();
  static void debug(const std::string &msg);
  static void info(const std::string &msg);
  static void warn(const std::string &msg);
  static void error(const std::string &msg);

private:
  static std::mutex &getMutex();
  static std::ofstream &getFile();
  static bool &isInitialized();
  static void log(Level level, const std::string &msg);
  static std::string timestamp();
  static std::string color(Level level, const std::string &text);
};
