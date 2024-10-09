#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#define LOG_FILE "shroudtopia.log"

#define LOG_CLASS(msg) Utils::Log(typeid(*this).name(), msg)

namespace Utils
{
    void Log(const char *szInput)
    {
        std::ofstream log(LOG_FILE, std::ios_base::app | std::ios_base::out);
        log << szInput;
        log << "\n";
        std::cout << "[shroudtopia] " << szInput << std::endl;
    }

    void Log(const char* className, const char* szInput)
    {
        std::ofstream log(LOG_FILE, std::ios_base::app | std::ios_base::out);
        log << "[shroudtopia]" << "[" << className << "] " << szInput;
        log << "\n";
        std::cout << "[shroudtopia]" << "[" << className << "] " << szInput << std::endl;
    }

    void PrintMemory(uintptr_t address, uint8_t* buffer, size_t size)
    {
        if (buffer)
        {
            std::cout << "Memory content at address " << std::hex << address << ":\n";
            for (size_t i = 0; i < size; ++i)
            {
                if (i % 16 == 0 && i != 0)
                    std::cout << "\n";
                std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)buffer[i] << " ";
            }
            std::cout << std::dec << std::endl;
        }
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
        Log(oss.str().c_str());
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
        Log(oss.str().c_str());
    }
}