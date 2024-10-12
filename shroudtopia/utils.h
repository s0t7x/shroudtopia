#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdio>

#include "defines.h"

#include "config.h"

#define LOG_CLASS(msg) Utils::LogClass(typeid(*this).name(), msg)


namespace Utils
{
    enum LogLevel {
        NONE,
        INFO,
        DEBUG,
        VERBOSE
    };

    void Log(LogLevel level, const char* format, ...) {
        if (!Config::get<bool>("enableLogging", true)) return;

        // Check if the current log level allows for logging this message
        auto llStr = Config::get<std::string>("logLevel", "VERBOSE");
        LogLevel currentLogLevel = INFO;
        if (llStr == "NONE") currentLogLevel = NONE;
        else if (llStr == "DEBUG") currentLogLevel = DEBUG;
        else if (llStr == "VERBOSE") currentLogLevel = VERBOSE;

        if (level > currentLogLevel) {
            return; // Do not log if the level is not allowed
        }

        std::string levelString;
        switch (level) {
        case DEBUG:
            levelString = "DEBUG";
            break;
        case VERBOSE:
            levelString = "VERBOSE";
            break;
        case INFO:
            levelString = "INFO";
            break;
        }

        // Create a buffer for the formatted message
        const size_t bufferSize = 512; // Adjust size as necessary
        char buffer[bufferSize];

        // Initialize the variadic arguments
        va_list args;
        va_start(args, format);

        // Format the string
        vsnprintf(buffer, bufferSize, format, args);

        // Clean up the variadic arguments
        va_end(args);

        // Open log file in append mode
        std::ofstream log("log.txt", std::ios_base::app | std::ios_base::out);

        // Log to the file and console
        log << "[shroudtopia][" << levelString << "] " << buffer << "\n";
        std::cout << "[shroudtopia][" << levelString << "] " << buffer << std::endl;
    }

    void Log(const char* format, ...) {
        va_list args;
        va_start(args, format);
        Log(INFO, format, args);

        // Clean up the variadic arguments
        va_end(args);
    }

    void LogClass(const char* className, const char* szInput)
    {
        Log(LogLevel::DEBUG, std::string("(").append(className).append(")").c_str());
    }

    const char* FormatString(const char* format, ...) {
        // Create a string to hold the formatted result
        static std::string result; // Use static to keep the memory valid after the function exits

        va_list args;
        va_start(args, format);

        // Clear previous content
        result.clear();

        // Format the string
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);

        // Store the formatted string
        result = buffer; // Store the result in the static string

        va_end(args);

        return result.c_str(); // Return a pointer to the internal C-string
    }

    void PrintHexBytes(const uint8_t *data, size_t length, const std::string &label, uintptr_t baseAddress = 0)
    {
        std::ostringstream oss;
        oss << std::hex << std::setw(16) << std::setfill('0') << baseAddress << " - " << label << " (size: " << std::dec << length << "):" << std::endl;
        for (size_t i = 0; i < length; ++i)
        {
            if (i % 16 == 0)
                oss << std::setw(4) << std::setfill('0') << i << ": ";
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
        }
        oss << std::endl;
        Log(DEBUG, oss.str().c_str());
    }

    void PrintJumpDetails(uintptr_t baseAddress, const std::string &label)
    {
        std::ostringstream oss;
        oss << label << " baseAddress: " << std::hex << baseAddress << std::endl;
        uintptr_t nextInstruction = baseAddress + 5;
        uintptr_t offset = *(uintptr_t *)(baseAddress + 1);
        uintptr_t targetAddress = nextInstruction + offset;
        oss << "Next instruction address: " << std::hex << nextInstruction << std::endl;
        oss << "Offset: " << std::hex << offset << std::endl;
        oss << "Target address: " << std::hex << targetAddress << std::endl;
        Log(DEBUG, oss.str().c_str());
    }
}