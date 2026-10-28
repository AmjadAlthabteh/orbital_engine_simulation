#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

// ANSI color codes for console (works on most modern terminals)
namespace ConsoleColor
{
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_CYAN = "\033[96m";
}

enum class LogLevel
{
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
    DEBUG,
    SYSTEM,
    SCIENCE,
    ACHIEVEMENT
};

class EnhancedLogger
{
private:
    bool enableColors;
    bool showTimestamp;
    bool showLogLevel;

    std::string getTimestamp() const
    {
        time_t now = time(0);
        tm ltm;
        localtime_s(&ltm, &now);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << ltm.tm_hour << ":"
           << std::setw(2) << ltm.tm_min << ":"
           << std::setw(2) << ltm.tm_sec;
        return ss.str();
    }

    std::string getColorForLevel(LogLevel level) const
    {
        if (!enableColors) return "";

        switch (level)
        {
            case LogLevel::INFO:        return ConsoleColor::BLUE;
            case LogLevel::SUCCESS:     return ConsoleColor::BRIGHT_GREEN;
            case LogLevel::WARNING:     return ConsoleColor::BRIGHT_YELLOW;
            case LogLevel::ERROR:       return ConsoleColor::BRIGHT_RED;
            case LogLevel::DEBUG:       return ConsoleColor::MAGENTA;
            case LogLevel::SYSTEM:      return ConsoleColor::CYAN;
            case LogLevel::SCIENCE:     return ConsoleColor::GREEN;
            case LogLevel::ACHIEVEMENT: return ConsoleColor::BRIGHT_CYAN;
            default:                    return ConsoleColor::WHITE;
        }
    }

    std::string getLevelPrefix(LogLevel level) const
    {
        switch (level)
        {
            case LogLevel::INFO:        return "[INFO]";
            case LogLevel::SUCCESS:     return "[SUCCESS]";
            case LogLevel::WARNING:     return "[WARNING]";
            case LogLevel::ERROR:       return "[ERROR]";
            case LogLevel::DEBUG:       return "[DEBUG]";
            case LogLevel::SYSTEM:      return "[SYSTEM]";
            case LogLevel::SCIENCE:     return "[SCIENCE]";
            case LogLevel::ACHIEVEMENT: return "[ACHIEVEMENT]";
            default:                    return "[LOG]";
        }
    }

public:
    EnhancedLogger(bool colors = true, bool timestamp = true, bool loglevel = true)
        : enableColors(colors), showTimestamp(timestamp), showLogLevel(loglevel)
    {}

    void log(LogLevel level, const std::string& message) const
    {
        std::stringstream output;

        // Add color
        output << getColorForLevel(level);

        // Add timestamp
        if (showTimestamp)
        {
            output << "[" << getTimestamp() << "] ";
        }

        // Add log level
        if (showLogLevel)
        {
            output << getLevelPrefix(level) << " ";
        }

        // Add message
        output << message;

        // Reset color
        if (enableColors)
        {
            output << ConsoleColor::RESET;
        }

        std::cout << output.str() << std::endl;
    }

    // Convenience methods
    void info(const std::string& message) const { log(LogLevel::INFO, message); }
    void success(const std::string& message) const { log(LogLevel::SUCCESS, message); }
    void warning(const std::string& message) const { log(LogLevel::WARNING, message); }
    void error(const std::string& message) const { log(LogLevel::ERROR, message); }
    void debug(const std::string& message) const { log(LogLevel::DEBUG, message); }
    void system(const std::string& message) const { log(LogLevel::SYSTEM, message); }
    void science(const std::string& message) const { log(LogLevel::SCIENCE, message); }
    void achievement(const std::string& message) const { log(LogLevel::ACHIEVEMENT, message); }

    // Boxed messages for important events
    void boxedMessage(const std::string& title, const std::vector<std::string>& lines, LogLevel level = LogLevel::INFO) const
    {
        std::string color = getColorForLevel(level);

        // Find max line length
        size_t maxLen = title.length();
        for (const auto& line : lines)
        {
            if (line.length() > maxLen)
                maxLen = line.length();
        }

        // Top border
        std::cout << color << "╔";
        for (size_t i = 0; i < maxLen + 4; ++i) std::cout << "═";
        std::cout << "╗" << ConsoleColor::RESET << "\n";

        // Title
        std::cout << color << "║  " << title;
        for (size_t i = title.length(); i < maxLen + 2; ++i) std::cout << " ";
        std::cout << "║" << ConsoleColor::RESET << "\n";

        // Separator
        std::cout << color << "╠";
        for (size_t i = 0; i < maxLen + 4; ++i) std::cout << "═";
        std::cout << "╣" << ConsoleColor::RESET << "\n";

        // Content lines
        for (const auto& line : lines)
        {
            std::cout << color << "║  " << line;
            for (size_t i = line.length(); i < maxLen + 2; ++i) std::cout << " ";
            std::cout << "║" << ConsoleColor::RESET << "\n";
        }

        // Bottom border
        std::cout << color << "╚";
        for (size_t i = 0; i < maxLen + 4; ++i) std::cout << "═";
        std::cout << "╝" << ConsoleColor::RESET << "\n\n";
    }

    // Section headers
    void sectionHeader(const std::string& title) const
    {
        std::cout << ConsoleColor::BRIGHT_CYAN << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "  " << title << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << ConsoleColor::RESET << "\n";
    }

    // Progress bar
    void progressBar(const std::string& label, float progress, int width = 40) const
    {
        int filled = static_cast<int>(progress * width);

        std::cout << ConsoleColor::CYAN << label << " [";

        for (int i = 0; i < width; ++i)
        {
            if (i < filled)
                std::cout << ConsoleColor::BRIGHT_GREEN << "█";
            else
                std::cout << ConsoleColor::WHITE << "░";
        }

        std::cout << ConsoleColor::CYAN << "] "
                  << static_cast<int>(progress * 100) << "%"
                  << ConsoleColor::RESET << "\n";
    }

    // Table row
    void tableRow(const std::string& col1, const std::string& col2, int width1 = 30, int width2 = 20) const
    {
        std::cout << ConsoleColor::WHITE << "│ " << col1;
        for (size_t i = col1.length(); i < static_cast<size_t>(width1); ++i) std::cout << " ";
        std::cout << "│ " << col2;
        for (size_t i = col2.length(); i < static_cast<size_t>(width2); ++i) std::cout << " ";
        std::cout << "│" << ConsoleColor::RESET << "\n";
    }

    // Enable/disable features
    void setEnableColors(bool enable) { enableColors = enable; }
    void setShowTimestamp(bool show) { showTimestamp = show; }
    void setShowLogLevel(bool show) { showLogLevel = show; }
};

// Global logger instance
static EnhancedLogger gLogger(true, true, true);
