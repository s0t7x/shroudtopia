#pragma once
#include <windows.h>
#include "utils.h"

namespace Mem {
    uintptr_t FindPattern(const char* pattern, const char* mask, uintptr_t start, size_t length) {
        for (uintptr_t i = start; i < start + length; i++) {
            bool found = true;
            for (size_t j = 0; mask[j] != '\0'; j++) {
                if (mask[j] != '?' && pattern[j] != *(char*)(i + j)) {
                    found = false;
                    break;
                }
            }
            if (found) return i;
        }
        return 0;
    }

    bool Write(uintptr_t address, const void* data, size_t size) {
        DWORD oldProtect;
        if (!VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            Utils::Log(std::string("VirtualProtect failed with error: ").append(std::to_string(GetLastError())).c_str());
            return false;
        }

        try {
            memcpy((void*)address, data, size);
        }
        catch (std::runtime_error err) {
            Utils::Log("Exception occurred while writing memory.");
            VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
            return false;
        }

        if (!VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect)) {
            Utils::Log(std::string("VirtualProtect restore failed with error: ").append(std::to_string(GetLastError())).c_str());
            return false;
        }



        return true;
    }

    LPVOID AllocateMemoryNearAddress(LPVOID address, SIZE_T dwSize) {
        HANDLE hProcess = GetCurrentProcess();
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        LPVOID lpBaseAddress = nullptr;

        // Calculate a range near the target address
        LPVOID lpMinAddress = (LPVOID)((DWORD_PTR)address - si.dwAllocationGranularity);
        LPVOID lpMaxAddress = (LPVOID)((DWORD_PTR)address + si.dwAllocationGranularity);

        // Attempt to allocate memory within the specified range
        lpBaseAddress = VirtualAllocEx(hProcess, lpMinAddress, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        if (!lpBaseAddress) {
            // Retry with a wider range if necessary
            lpBaseAddress = VirtualAllocEx(hProcess, (LPVOID)((uintptr_t)lpMinAddress - 0x10000000), dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        }

        return lpBaseAddress;
    }
}