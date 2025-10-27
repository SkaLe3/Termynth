#include "Core/Logger.h"
#include <filesystem>
#include <iostream>

Logger::Logger(bool bLogConsole, bool bLogFile, const std::string &filename, ELogLevel minLevel, bool bNoStamp, bool autoFlush)
    : minLevel(minLevel), autoFlush(autoFlush), m_bLogToFile(bLogFile), m_bLogToConsole(bLogConsole), m_bNoStamp(bNoStamp)
{
    if (bLogFile)
    {
        if (std::filesystem::exists(filename))
        {
            std::filesystem::remove(filename);
        }
        file.open(filename, std::ios::out | std::ios::app);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
    }
    if (bLogConsole)
    {
    }
    if (!bNoStamp)
    {
        Log(ELogLevel::Info, "=== Logging session started ===");
    }
}

Logger::~Logger()
{
    if (m_bLogToFile)
    {
        if (file.is_open())
        {
            Log(ELogLevel::Info, "=== Logging session ended ===");
            file.close();
        }
    }
}

void Logger::Log(ELogLevel level, const std::string &message)
{
    if (level < minLevel)
        return;
    std::lock_guard<std::mutex> lock(mtx);
    std::string logData;
    if (!m_bNoStamp)
    {
        logData = "[" + GetTimestamp() + "] " + "[" + GetLevelString(level) + "] ";
    }
    logData += message + "\n";
    
    if (m_bLogToFile)
    {
        file << logData;
        if (autoFlush)
        {
            Flush();
        }
    }
    if (m_bLogToConsole)
    {
        std::cout << logData;
    }
}

void Logger::SetMinLevel(ELogLevel level)
{
    std::lock_guard<std::mutex> lock(mtx);
    minLevel = level;
}

void Logger::Flush()
{
    std::lock_guard<std::mutex> lock(mtx);
    file.flush();
}

std::string Logger::GetLevelString(ELogLevel level)
{
    switch (level)
    {
    case ELogLevel::Debug:
        return "DEBUG";
    case ELogLevel::Info:
        return "INFO";
    case ELogLevel::Warning:
        return "WARNING";
    case ELogLevel::Error:
        return "ERROR";
    case ELogLevel::Fatal:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}
std::string Logger::GetTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}