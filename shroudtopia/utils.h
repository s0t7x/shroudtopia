#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "defines.h"

#include "config.h"

#define LOG_CLASS(msg) Utils::Log(typeid(*this).name(), msg)


namespace Utils
{
    enum LogLevel {
        NONE,
        INFO,
        DEBUG,
        VERBOSE
    };

    void Log(LogLevel level, const char* szInput) {
        if (!Config::get<bool>("enableLogging", true)) return;
        // Check if the current log level allows for logging this message
        auto llStr = Config::get<std::string>("logLevel", "VERBOSE");
        LogLevel currentLogLevel = INFO;
        if (llStr == std::string("NONE")) currentLogLevel = NONE;
        if (llStr == std::string("DEBUG")) currentLogLevel = DEBUG;
        if (llStr == std::string("VERBOSE")) currentLogLevel = VERBOSE;
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

        // Open log file in append mode
        std::ofstream log(LOG_FILE, std::ios_base::app | std::ios_base::out);

        // Log to the file and console
        log << "[shroudtopia][" << levelString << "] " << szInput << "\n";
        std::cout << "[shroudtopia][" << levelString << "] " << szInput << std::endl;
    }

    void Log(const char* className, const char* szInput)
    {
        Log(LogLevel::DEBUG, std::string("(").append(className).append(")").c_str());
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