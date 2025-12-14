#include "Core/Logger.h"
#include <filesystem>
#include <iostream>
#include <filesystem>
#include <regex>
#include <cmath>
#include <windows.h>

namespace fs = std::filesystem;

constexpr int32_t MAX_DIGITS = 4;
const std::string SEPARATOR = "_";
const int MAX_VERSION = (int)std::pow(10, MAX_DIGITS) - 1; // 9999

static bool is_file_available(const fs::path& filepath) 
{
	std::wstring w_path = filepath.wstring();

	// Use CreateFileW to open the file with specific, exclusive permissions:
	HANDLE hFile = CreateFileW(
		w_path.c_str(),
		GENERIC_WRITE,             // Request write access
		0,                         // **dwShareMode = 0 (No Sharing!)**
		NULL,                      // Default security attributes
		CREATE_ALWAYS,             // Create a new file or overwrite existing (truncation)
		FILE_ATTRIBUTE_NORMAL,     // Normal file attributes
		NULL                       // No template file
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// If the call failed (GetLastError() will usually be ERROR_SHARING_VIOLATION), 
		// the file is locked by another process.
		return false;
	}

	// Success! The file was opened exclusively and truncated. 
	// Close the handle and return true.
	CloseHandle(hFile);

	return true;
}

static fs::path get_available_filename(const std::string& base_filename) 
{
    fs::path original_path(base_filename);

    if (is_file_available(original_path)) 
    {
        return original_path;
    }

    std::string stem = original_path.stem().string();
    std::string extension = original_path.extension().string();
    
    int32_t initial_version = 1;
    std::string base_stem = stem;

    std::regex suffix_regex(SEPARATOR + "([0-9]{1," + std::to_string(MAX_DIGITS) + "})$");
    std::smatch match;

    if (std::regex_search(stem, match, suffix_regex)) 
    {
        initial_version = std::stoi(match[1].str()) + 1;
        base_stem = stem.substr(0, match.position());
    } 

    for (int version = initial_version; version <= MAX_VERSION; ++version) 
    {
        std::stringstream ss;
        
        ss << base_stem << SEPARATOR 
           << std::setw(MAX_DIGITS) << std::setfill('0') << version;
        
        fs::path candidate_path = original_path;
        candidate_path.replace_filename(ss.str() + extension);

        if (is_file_available(candidate_path)) {
            return candidate_path;
        }
    }
    
    return {};
}


Logger::Logger(bool bLogConsole, bool bLogFile, const std::string &filename, ELogLevel minLevel, bool bNoStamp, bool autoFlush)
    : minLevel(minLevel), autoFlush(autoFlush), m_bLogToFile(bLogFile), m_bLogToConsole(bLogConsole), m_bNoStamp(bNoStamp)
{
    fs::path finalFilename = get_available_filename(filename);
    if (bLogFile)
    {
        if (std::filesystem::exists(finalFilename))
        {
            std::filesystem::remove(finalFilename);
        }
        file.open(finalFilename, std::ios::out | std::ios::app);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + finalFilename.string());
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
            file.flush();
        }
    }
    if (m_bLogToConsole)
    {
        std::cout << logData;
        std::cout.flush();
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