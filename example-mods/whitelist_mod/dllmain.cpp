#include "pch.h"

#include <shroudtopia.h>
#include <memory_utils.h>

ModMetaData metaData = {
    "Whitelist",
    "Only allows steamIds on a whitelist to connect to the server.",
    "1.0",
    "s0T7x",
    "0.0.1",
    true,
    true,
};


// Signature for GameVersion (SVN) 602428
class Whitelist : public Mod
{
    Mem::Detour* mod;
    uint64_t steamIdWhitelist[64] = { 0 };
    uint64_t whitelistSize = 2;

    std::vector<uint64_t> steamIdCache;

public:
    void updateWhitelist() {
        // update 8byte whitelist array address
        mod->shellcode->updateValue<uintptr_t>(8, (uintptr_t)steamIdWhitelist);
        // update 8byte array size
        mod->shellcode->updateValue<uint64_t>(18, (uint64_t)whitelistSize);
    }
    
    void addToWhitelist(uint64_t steamId) {
        if (whitelistSize == 64) return;
        steamIdWhitelist[whitelistSize++] = steamId;
    }

    void clearWhitelist() {
        whitelistSize = 1;
    }

    void Load(ModContext* modContext)
    {
        const char* patternClientConnect = "\x48\x83\x79\x08\x00\x48\x8B\xF2";
        const char* maskClientConnect = "xxxxxxxx";

        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
        uintptr_t addressClientConnect = Mem::FindPattern(patternClientConnect, maskClientConnect, baseAddress, 0x1000000); // Scan 16MB

        const char* patternRealReturn = "\x48\x8B\x9C\x24\x28\x01\x00\x00\x48\x81\xC4\x00\x01\x00\x00";
        const char* maskRealReturn = "xxxxxxxxxxxxxxx";
        uintptr_t addressRealReturn = Mem::FindPattern(patternRealReturn, maskRealReturn, baseAddress, 0x1000000); // Scan 16MB

        if (addressClientConnect && addressRealReturn)
        {
            uint8_t modCode[] = {
                0x50, 
                0x53, 
                0x51, 
                0x48, 0x8B, 0x02, 
                0x48, 0xBB, 0x08, 0x01, 0x95, 0xB4, 0xF7, 0x7F, 0x00, 0x00, // <- 8 - 8byte whitelist array address
                0x48, 0xB9, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <- 18 - 8byte array size
                0x48, 0x39, 0x03, 
                0x0F, 0x84, 0x11, 0x00, 0x00, 0x00, // <-- 31 - je loop_return
                0x48, 0x83, 0xC3, 0x08, 
                0x48, 0xFF, 0xC9, 
                0x75, 0xEE,
                0x59, // <- 44 - block_connection:
                0x5B, 
                0x58, 
                0xE9, 0x2D, 0x29, 0x7B, 0x00, // <-- 48 - jmp realReturn + 8
                0x59, // <- 52 - loop_return:
                0x5B, 
                0x58, 
                0x83, 0x79, 0x08, 0x00, 
                0x48, 0x8B, 0xF2, 
                0xE9, 0xD4, 0x26, 0x7B, 0x00 // <-- 63 - jmp return
            };

            mod = new Mem::Detour(addressClientConnect, modCode, sizeof(modCode), false, 3);

            updateWhitelist();

            // update jmp realReturn + 8
            mod->shellcode->updateValue<uint32_t>(48, (uint32_t)((addressRealReturn + 8) - (mod->shellcode->data->address + 48 + 4)));
            // update jmp return
            mod->shellcode->updateValue<uint32_t>(63, (uint32_t)((mod->patch->data->address + mod->patch->data->size) - (mod->shellcode->data->address + mod->shellcode->data->size)));
        }
    }

    void Unload(ModContext* modContext) {
        if (mod) delete mod;
    }

    void Activate(ModContext* modContext) {
        active = mod->activate();
        modContext->Log(std::string().append(metaData.name).append(" activated").c_str());
    }

    void Deactivate(ModContext* modContext) {
        mod->deactivate();
        active = false;
        modContext->Log(std::string().append(metaData.name).append(" deactivated").c_str());
    }

    void Update(ModContext* modContext) {
        const char* modKey = metaData.name.c_str();
        std::vector<uint64_t> whitelistedSteamIds = modContext->config.GetUint64Array(modKey, "steamIds", {});

        if (steamIdCache != whitelistedSteamIds) {
            clearWhitelist();
            for (auto steamId : whitelistedSteamIds) {
                modContext->Log(std::string("add steamId ").append(std::to_string(steamId)).c_str());
                addToWhitelist(steamId);
            }
            steamIdCache = whitelistedSteamIds;
        }
    }

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

