#include "pch.h"

#include <shroudtopia.h>
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

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

class ISteamGameServer; // Forward declaration
typedef ISteamGameServer* (__cdecl* SteamGameServer_t)();

class ISteamGameServer {
public:
    virtual bool HandleIncomingPacket(const void* pData, int cbData, uint32 srcIP, uint16 srcPort) = 0;
    // Add other methods as needed...
};

typedef bool (*HandleIncomingPacket_t)(const void* pData, int cbData, uint32 srcIP, uint16 srcPort);

// Signature for GameVersion (SVN) 55812
class Whitelist : public Mod
{   
    HMODULE hSteamAPI;
    uintptr_t getSteamIDAddress;
    void* originalGetSteamId = nullptr;
    HandleIncomingPacket_t originalHandleIncomingPacket = nullptr;

    std::function<uint64_t()> hookedGetSteamId = [=]()->uint64_t {
        _modContext->Log("Log from hooked GetSteamId");
        return originalGetSteamId ? ((uintptr_t (*)(void))originalGetSteamId)() : 0;
    };

public:
    void Load(ModContext* modContext)
    {
        _modContext = modContext;

        hSteamAPI = LoadLibraryA("steam_api.dll");
        if (!hSteamAPI) {
            _modContext->Log("Failed to load steam_api.dll");
            return;
        }

        // Get function pointers
        SteamGameServer_t SteamGameServer = (SteamGameServer_t)GetProcAddress(hSteamAPI, "SteamGameServer");

        if (!SteamGameServer) {
            _modContext->Log("Failed to get function pointer");
            FreeLibrary(hSteamAPI);
            return;
        }

        // Get ISteamGameServer instance
        ISteamGameServer* gameServer = SteamGameServer();
        if (!gameServer) {
            _modContext->Log("Failed to get ISteamGameServer instance");
            FreeLibrary(hSteamAPI);
            return;
        }

        uintptr_t** vtable = reinterpret_cast<uintptr_t**>(gameServer);
        originalHandleIncomingPacket = reinterpret_cast<HandleIncomingPacket_t>((*vtable)[YOUR_FUNCTION_INDEX]);

        getSteamIDAddress = (uintptr_t)&gameServer->HandleIncomingPacket;
    }

    void Unload(ModContext* modContext) {
        // Free the DLL
        FreeLibrary(hSteamAPI);
    }

    void Activate(ModContext* modContext) {
        if (getSteamIDAddress && !active) {
            active = modContext->memory.CreateDetour((LPVOID)getSteamIDAddress, &hookedGetSteamId, &originalGetSteamId);
        }
    }

    void Deactivate(ModContext* modContext) {
        if (originalGetSteamId && active) {
            active = !modContext->memory.CreateDetour((LPVOID)getSteamIDAddress, &originalGetSteamId, nullptr);
        }
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

