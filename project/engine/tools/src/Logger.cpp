/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Logger
*/

#include <unordered_map>
#include <filesystem>
#include "Logger.hpp"

std::mutex& Logger::getMutex()
{
    static std::mutex mutex;
    return mutex;
}

std::ofstream& Logger::getFile()
{
    static std::ofstream file;
    return file;
}

bool& Logger::isInitialized()
{
    static bool initialized = false;
    return initialized;
}

void Logger::init(const std::string& logFile)
{
    std::lock_guard<std::mutex> lock(getMutex());

    if (isInitialized())
        return;

    std::filesystem::create_directories(std::filesystem::path(logFile).parent_path());

    auto& file = getFile();
    file.open(logFile, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[Logger] Failed to open log file: " << logFile << std::endl;
    } else {
        std::cout << "[Logger] Logging to " << logFile << std::endl;
    }

    isInitialized() = true;
}

void Logger::shutdown()
{
    std::lock_guard<std::mutex> lock(getMutex());
    auto& file = getFile();
    if (file.is_open())
        file.close();
    isInitialized() = false;
}

void Logger::debug(const std::string& msg)
{
    log(Level::DEBUG, msg);
}

void Logger::info(const std::string& msg)
{
    log(Level::INFO,  msg);
}

void Logger::warn(const std::string& msg)
{
    log(Level::WARN,  msg);
}

void Logger::error(const std::string& msg)
{
    log(Level::ERROR, msg);
}


void Logger::log(Level level, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(getMutex());

    static const std::unordered_map<Level, std::string> levelNames = {
        { Level::DEBUG, "DEBUG" },
        { Level::INFO,  "INFO" },
        { Level::WARN,  "WARN" },
        { Level::ERROR, "ERROR" }
    };

    const std::string& levelStr = levelNames.at(level);
    std::string formatted = "[" + timestamp() + "] [" + levelStr + "] " + msg;

    std::cout << color(level, formatted) << std::endl;

    auto& file = getFile();
    if (file.is_open())
        file << formatted << std::endl;
}

std::string Logger::timestamp()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

std::string Logger::color(Level level, const std::string& text)
{
    static const std::unordered_map<Level, const char*> colors = {
        { Level::DEBUG, "\033[36m" },
        { Level::INFO,  "\033[32m" },
        { Level::WARN,  "\033[33m" },
        { Level::ERROR, "\033[31m" }
    };
    constexpr const char* RESET = "\033[0m";

    auto it = colors.find(level);
    return (it != colors.end() ? std::string(it->second) + text + RESET : text);
}