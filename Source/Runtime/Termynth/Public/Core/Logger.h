#pragma once

#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <format>

enum class ELogLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger
{
public:
    Logger(bool bLogConsole, bool bLogFile, const std::string &filename, ELogLevel minLevel = ELogLevel::Info, bool bNoStamp = false, bool autoFlush = true);
    ~Logger();

    void Log(ELogLevel level, const std::string &message);

    void Debug(const std::string &msg) { Log(ELogLevel::Debug, msg); }
    void Info(const std::string &msg) { Log(ELogLevel::Info, msg); }
    void Warning(const std::string &msg) { Log(ELogLevel::Warning, msg); }
    void Error(const std::string &msg) { Log(ELogLevel::Error, msg); }
    void Fatal(const std::string &msg) { Log(ELogLevel::Fatal, msg); }

    void SetMinLevel(ELogLevel level);
    void Flush();

private:
    std::string GetLevelString(ELogLevel level);
    std::string GetTimestamp();

private:
    std::ofstream file;
    std::mutex mtx;
    ELogLevel minLevel;
    bool autoFlush;
    bool m_bLogToFile;
    bool m_bLogToConsole;
    bool m_bNoStamp;
};

// Global logger instance
inline Logger* g_Logger = nullptr;

// Helper macros for convenient logging
#define LOG_INIT_FILE(filename) g_Logger = new Logger(false,true, filename)
#define LOG_INIT_LEVEL_FILE(filename, level) g_Logger = new Logger(false, true, filename, level)
#define LOG_INIT(console, file, filename) g_Logger = new Logger(console, file, filename) 
#define LOG_INIT_LEVEL(console, file, filename, level) g_Logger = new Logger(console, file, filename, level) 
#define LOG_INIT_NOSTAMP(level) g_Logger = new Logger(true, false, "", level, true);
#define LOG_SHUTDOWN() \
    delete g_Logger;   \
    g_Logger = nullptr

#define LOG_DEBUG(msg) \
    if (g_Logger)      \
    g_Logger->Debug(msg)
#define LOG_INFO(msg) \
    if (g_Logger)     \
    g_Logger->Info(msg)
#define LOG_WARNING(msg) \
    if (g_Logger)        \
    g_Logger->Warning(msg)
#define LOG_ERROR(msg) \
    if (g_Logger)      \
    g_Logger->Error(msg)
#define LOG_FATAL(msg) \
    if (g_Logger)      \
    g_Logger->Fatal(msg)

