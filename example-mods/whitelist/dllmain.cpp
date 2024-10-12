#include "pch.h"

#include <shroudtopia.h>
#include <mem.h>

#include <cstdio>
#include <string>

#include <psapi.h>
#include <algorithm>

ModMetaData metaData = {
    "whitelist",
    "Enables whitelisting steam ids. No more passwords for friends-only servers!",
    "1.0",
    "s0T7x",
    "0.0.1",
    false,
    true,
};

ModContext* _modContext;

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

typedef int (*GetSteamID_t)(); // Define the original function type
GetSteamID_t originalGetSteamID = nullptr;

uint8_t originalBytes[23];
BYTE* trampoline = nullptr; // To store the trampoline code

void CreateTrampoline(uintptr_t address, ModContext* modContext) {
    DWORD oldProtect;
    VirtualProtect((LPVOID)address, 23, PAGE_EXECUTE_READWRITE, &oldProtect);

    modContext->Log("GetSteamID: Saving original function");
    // Copy the original bytes to trampoline
    memcpy(originalBytes, (void*)address, 23);

    // Calculate the address after the jump instruction
    modContext->Log("GetSteamID: Allocate trampoline");
    trampoline = (BYTE*)VirtualAlloc(0, 23, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    modContext->Log("GetSteamID: Move original function to trampoline");
    // Copy the original bytes to trampoline
    memcpy(trampoline, originalBytes, 23);

    //modContext->Log("GetSteamID: Write jmp back");
    //// Write a jump back to the original code execution after the original function
    //Mem::WriteJump((LPVOID)((DWORD)trampoline + 23), (LPVOID)((DWORD)address + 23 - 1));

    VirtualProtect((LPVOID)address, 23, oldProtect, &oldProtect);
}

// Our custom GetSteamID function
extern "C" __declspec(dllexport) uint64_t __stdcall HookedGetSteamID()
{
    _modContext->Log("GetSteamID has been called!");

    uint64_t steamId = ((GetSteamID_t)trampoline)(); // Call the original function

    _modContext->Log(FormatString("Retrieved Steam ID: %i", steamId));

    return steamId; // Return the original Steam ID
}

void UnhookGetSteamID() {
    // Check if we have a valid address for GetSteamID
    if (originalGetSteamID != nullptr) {
        // Restore the original bytes at the hooked address
        DWORD oldProtect;
        VirtualProtect((LPVOID)(uintptr_t)originalGetSteamID, 23, PAGE_EXECUTE_READWRITE, &oldProtect);

        // Write back the original bytes
        Mem::Write((uintptr_t)originalGetSteamID, originalBytes, sizeof(originalBytes));

        // Restore the original protection
        VirtualProtect((LPVOID)(uintptr_t)originalGetSteamID, 23, oldProtect, &oldProtect);

        // Free the allocated trampoline memory if it was created
        if (trampoline != nullptr) {
            VirtualFree(trampoline, 0, MEM_RELEASE);
            trampoline = nullptr;
        }
        // Reset the original function pointer
        originalGetSteamID = nullptr;
    }
}

void HookGetSteamID(uintptr_t getSteamIDAddress, ModContext* modContext) {
    Mem::Read(getSteamIDAddress, originalBytes, sizeof(originalBytes));

    // Store the original function pointer
    originalGetSteamID = (GetSteamID_t)(getSteamIDAddress);

    modContext->Log(FormatString("originalGetSteamID: %x", originalGetSteamID));

    CreateTrampoline(getSteamIDAddress, modContext);

    // Write the jump to our hooked function
    Mem::WriteJump((LPVOID)getSteamIDAddress, (LPVOID)HookedGetSteamID);
}

uintptr_t GetModuleBaseAddress(const std::string& moduleName, ModContext* modContext) {
    // Get a handle to the current process
    HANDLE hProcess = GetCurrentProcess();

    // Get the list of all modules in the current process
    HMODULE hModules[1024];
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
        // Iterate through the modules to find the one matching the name
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            char szModName[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, hModules[i], szModName, sizeof(szModName) / sizeof(char))) {
                // Convert to std::string
                std::string fullPath(szModName);
                // Compare the module name only (case insensitive)
                std::string lowerModuleName = moduleName;
                std::transform(lowerModuleName.begin(), lowerModuleName.end(), lowerModuleName.begin(), ::tolower);
                std::transform(fullPath.begin(), fullPath.end(), fullPath.begin(), ::tolower);

                if (fullPath.find(lowerModuleName) != std::string::npos) {
                    MODULEINFO moduleInfo;
                    if (GetModuleInformation(hProcess, hModules[i], &moduleInfo, sizeof(moduleInfo))) {
                        return reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll); // Return the actual base address
                    }
                }
            }
        }
    }

    return 0; // Module not found
}

// Signature for GameVersion (SVN) 55812
class Whitelist : public Mod
{
    uintptr_t getSteamIDAddress;
public:
    void Load(ModContext* modContext)
    {
        _modContext = modContext;
        //  48 83 EC 28 48 8B 01 48 8D 54 24 30 FF 50 10 48 8B 00 48 83 C4 28
        const char* pattern = "\x48\x83\xEC\x28\x48\x8B\x01\x48\x8D\x54\x24\x30\xFF\x50\x10\x48\x8B\x00\x48\x83\xC4\x28";
        const char* mask = "xxxxxxxxxxxxxxxxxxxxxx";

        uintptr_t steamApiBase = GetModuleBaseAddress("steam_api64.dll", modContext);
        getSteamIDAddress = steamApiBase ? steamApiBase + 0x29A0 : Mem::FindPattern(pattern, mask, 0x0, 0x7FFFFFFFF);

        modContext->Log(FormatString("Address of GetSteamID: %x", getSteamIDAddress));
    }

    void Unload(ModContext* modContext) {
    }

    void Activate(ModContext* modContext) {
        if (getSteamIDAddress && originalGetSteamID == nullptr)
        {
            HookGetSteamID(getSteamIDAddress, modContext);
            modContext->Log("Hooked GetSteamID for whitlisting");
            active = true;
        }
        else {
            modContext->Log(FormatString("Cannot hook GetSteamID. GetSteamID Adress is %x", getSteamIDAddress));
        }
    }

    void Deactivate(ModContext* modContext) {
        if (getSteamIDAddress && originalGetSteamID != nullptr) {
            UnhookGetSteamID();
            modContext->Log("Unhooked GetSteamID and removed whitlisting");
        }
            active = false;
    }

    void Update(ModContext* modContext) { }

    ModMetaData GetMetaData() {
        return metaData;
    }
};

extern "C" __declspec(dllexport) Mod* CreateModInstance() {
    return new Whitelist();
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

