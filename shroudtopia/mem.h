#pragma once
#include <windows.h>
#include <iostream>

namespace Mem
{

    bool WriteToReadOnlyMemory(LPVOID targetAddress, LPVOID data, SIZE_T size)
    {
        HANDLE hProcess = GetCurrentProcess();
        DWORD oldProtect;

        // Change memory protection to PAGE_EXECUTE_READWRITE
        if (!VirtualProtectEx(hProcess, targetAddress, size, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            return false;
        }

        // Write the data to the target address
        SIZE_T bytesWritten;
        if (!WriteProcessMemory(hProcess, targetAddress, data, size, &bytesWritten) || bytesWritten != size)
        {
            // Restore original protection before exiting
            VirtualProtectEx(hProcess, targetAddress, size, oldProtect, &oldProtect);
            return false;
        }

        // Restore original protection
        if (!VirtualProtectEx(hProcess, targetAddress, size, oldProtect, &oldProtect))
        {
            return false;
        }
        return true;
    }

    uintptr_t FindPattern(const char* pattern, const char* mask, uintptr_t start, size_t length)
    {
        MEMORY_BASIC_INFORMATION mbi;
        uintptr_t currentAddress = start;

        while (currentAddress < start + length)
        {
            if (VirtualQuery((LPCVOID)currentAddress, &mbi, sizeof(mbi)) == sizeof(mbi))
            {
                if (mbi.State == MEM_COMMIT &&
                    (mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE | PAGE_READONLY | PAGE_EXECUTE_READWRITE | PAGE_READWRITE)))
                {
                    uintptr_t regionStart = (uintptr_t)mbi.BaseAddress;
                    uintptr_t regionEnd = regionStart + mbi.RegionSize;

                    for (uintptr_t i = regionStart; i < regionEnd - strlen(mask); i++)
                    {
                        bool found = true;
                        for (size_t j = 0; mask[j] != '\0'; j++)
                        {
                            if (mask[j] != '?' && pattern[j] != *(char*)(i + j))
                            {
                                found = false;
                                break;
                            }
                        }
                        if (found)
                        {
                            return i; // Found the pattern
                        }
                    }
                }
                currentAddress += mbi.RegionSize; // Move to the next region
            }
            else
            {
                break; // Exit if VirtualQuery fails
            }
        }

        return 0; // Pattern not found
    }

    static bool Write(uintptr_t address, const void *buffer, size_t size)
    {
        HANDLE handle = GetCurrentProcess();
        DWORD oldProtect;
        if (VirtualProtectEx(handle, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            memcpy((void *)address, buffer, size);
            VirtualProtectEx(handle, (LPVOID)address, size, oldProtect, &oldProtect);
            return true;
        }
        return false;
    }

    static bool Read(uintptr_t address, void *buffer, size_t size)
    {
        HANDLE handle = GetCurrentProcess();
        DWORD oldProtect;
        if (VirtualProtectEx(handle, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            memcpy(buffer, (void *)address, size);
            VirtualProtectEx(handle, (LPVOID)address, size, oldProtect, &oldProtect);
            return true;
        }
        return false;
    }

    template <typename T>
    static bool Write(uintptr_t address, const T &value)
    {
        return Write(address, &value, sizeof(T));
    }

    template <typename T>
    static bool Read(uintptr_t address, T &value)
    {
        return Read(address, &value, sizeof(T));
    }

    LPVOID AllocateMemoryNearAddress(LPVOID address, SIZE_T dwSize)
    {
        HANDLE hProcess = GetCurrentProcess();
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        LPVOID lpBaseAddress = nullptr;

        // Calculate a range near the target address
        LPVOID lpMinAddress = (LPVOID)((DWORD_PTR)address - si.dwAllocationGranularity * 10);

        MEMORY_BASIC_INFORMATION mbi;
        VirtualQueryEx(hProcess, lpMinAddress, &mbi, sizeof(mbi));

        DWORD oldProtect;
        VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtect);

        // Attempt to allocate memory within the specified range
        lpBaseAddress = VirtualAllocEx(hProcess, lpMinAddress, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        int tries = 0;
        static const int maxTries = 10;
        while (!lpBaseAddress && tries < maxTries)
        {
            // Retry with a wider range if necessary
            lpBaseAddress = VirtualAllocEx(hProcess, (LPVOID)((uintptr_t)lpMinAddress - 0x10000000 * (tries + 1)), dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            tries++;
        }

        VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, oldProtect, &oldProtect);

        return lpBaseAddress;
    }

    class MemoryData
    {
    public:
        uintptr_t address;
        uint8_t *opcode;
        size_t size;

        MemoryData(uintptr_t addr, uint8_t *op, size_t sz)
            : address(addr), opcode(new uint8_t[sz]), size(sz)
        {
            memcpy(opcode, op, sz);
        }

        ~MemoryData()
        {
            delete[] opcode;
        }

        bool write()
        {
            return Mem::Write(address, opcode, size);
        }

        bool read()
        {
            return Mem::Read(address, opcode, size);
        }

        void updateOpcode(size_t offset, uint8_t *data, size_t dataSize)
        {
            if (offset + dataSize <= size)
            {
                memcpy(opcode + offset, data, dataSize);
            }
        }

        template <typename T>
        void updateValue(size_t offset, T value)
        {
            if (offset + sizeof(T) <= size)
            {
                *reinterpret_cast<T *>(opcode + offset) = value;
            }
        }
    };

    class Shellcode
    {
    public:
        MemoryData *data;
        bool injected = false;

        Shellcode(uint8_t *opcode, size_t opcodeSize, uintptr_t nearAddr = 0)
        {
            uintptr_t address = (uintptr_t)Mem::AllocateMemoryNearAddress(reinterpret_cast<LPVOID>(nearAddr), 0x1000);
            data = new MemoryData(address, opcode, opcodeSize);
        }

        ~Shellcode()
        {
            if (injected)
            {
                uint8_t *nullData = (uint8_t *)malloc(data->size);
                memset(nullData, 0x90, data->size); // Use NOP (0x90) for nulling
                MemoryData nuller(data->address, nullData, data->size);
                nuller.write();
                free(nullData);
            }
            delete data;
        }

        bool inject()
        {
            if (data->write())
                return injected = true;
            return false;
        }

        void updateOpcode(size_t offset, uint8_t *newOpcode, size_t newSize)
        {
            data->updateOpcode(offset, newOpcode, newSize);
        }

        template <typename T>
        void updateValue(size_t offset, T value)
        {
            data->updateValue(offset, value);
        }
    };

    class Patch
    {
    public:
        MemoryData *data;
        uint8_t *originalBytes = nullptr;
        bool patched = false;

        Patch(uintptr_t address, uint8_t *opcode, size_t opcodeSize)
        {
            data = new MemoryData(address, opcode, opcodeSize);
        }

        ~Patch()
        {
            delete[] originalBytes;
            delete data;
        }

        bool apply()
        {
            if (patched)
                return false;
            originalBytes = new uint8_t[data->size];
            Mem::Read(data->address, originalBytes, data->size);
            if (data->write())
            {
                return patched = true;
            }
            return false;
        }

        void undo()
        {
            if (patched && originalBytes)
            {
                MemoryData original(data->address, originalBytes, data->size);
                original.write();
                patched = false;
            }
        }

        void updateOpcode(size_t offset, uint8_t *newOpcode, size_t newSize)
        {
            data->updateOpcode(offset, newOpcode, newSize);
        }

        template <typename T>
        void updateValue(size_t offset, T value)
        {
            data->updateValue(offset, value);
        }
    };

    class Detour
    {
    public:
        bool active = false;
        Shellcode *shellcode = nullptr;
        Patch *patch = nullptr;

        Detour(uintptr_t address, uint8_t *opcode, size_t opcodeSize, bool trampoline = true, size_t padding = 0)
        {
            uint8_t jmp[1] = {0xE9};
            uint8_t nop[1] = {0x90};

            size_t patchDataSize = 5 + padding;

            if (trampoline)
            {
                size_t shellcodeDataSize = 5 + opcodeSize;
                uint8_t *shellcodeData = new uint8_t[shellcodeDataSize];
                shellcode = new Shellcode(shellcodeData, shellcodeDataSize, address);
                shellcode->updateOpcode(opcodeSize, jmp, 1);
                shellcode->updateValue<uint32_t>(opcodeSize + 1, (uint32_t)(address + patchDataSize));
                delete[] shellcodeData;
            }
            else
            {
                shellcode = new Shellcode(opcode, opcodeSize, address);
            }

            uint8_t *patchData = new uint8_t[patchDataSize];
            patch = new Patch(address, patchData, patchDataSize);
            delete[] patchData;
            patch->updateOpcode(0, jmp, 1);
            for (int i = 0; i < padding; i++)
            {
                patch->updateOpcode(5 + i, nop, 1);
            }
            patch->updateValue<uint32_t>(1, (uint32_t)((uintptr_t)shellcode->data->address - (patch->data->address + 5)));
        }

        bool activate()
        {
            if (!active && shellcode && patch)
                return shellcode->inject() && patch->apply() && (active = true);
            return false;
        }

        bool deactivate()
        {
            if (active && patch) {
                patch->undo();
                return (active = false);
            }
            return false;
        }
    };

}