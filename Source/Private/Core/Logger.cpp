#include "Core/Logger.h"
#include <filesystem>

Logger::Logger(const std::string& filename, ELogLevel minLevel, bool autoFlush )
    : minLevel(minLevel), autoFlush(autoFlush)
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
    Log(ELogLevel::Info, "=== Logging session started ===");
}

Logger::~Logger()
{
    if (file.is_open())
    {
        Log(ELogLevel::Info, "=== Logging session ended ===");
        file.close();
    }
}

void Logger::Log(ELogLevel level, const std::string &message)
{
    if (level < minLevel)
        return;
    std::lock_guard<std::mutex> lock(mtx);
    file << "[" << GetTimestamp() << "] "
         << "[" << GetLevelString(level) << "] "
         << message << std::endl;
    if (autoFlush)
    {
        file.flush();
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
    switch(level) {
        case ELogLevel::Debug:   return "DEBUG";
        case ELogLevel::Info:    return "INFO";
        case ELogLevel::Warning: return "WARNING";
        case ELogLevel::Error:   return "ERROR";
        case ELogLevel::Fatal:   return "FATAL";
        default:                return "UNKNOWN";
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