#include "pch.h"

#include <shroudtopia.h>
#include <memory_utils.h>

ModMetaData metaData = {
    "Flight Mod",
    "Enables free glider flight.",
    "1.0",
    "s0T7x",
    "0.0.1",
    true,
    true,
};


// Signature for GameVersion (SVN) 55812
class FlightMod : public Mod
{
    Mem::Detour* mod;
public:
    void Load(ModContext* modContext)
    {
        // F3 0F 10 05 ? ? ? ? ? ? ? ? F2 0F 11 4C 24 60
        const char* pattern = "\xF3\x0F\x10\x05\x00\x00\x00\x00\xF2\x0F\x11\x4C\x24\x60";
        const char* mask = "xxxx????xxxxxx";

        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
        uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

        if (address)
        {
            uint8_t modCode[] = {
                0xF3, 0x0F, 0x10, 0x05, 0x05, 0x00, 0x00, 0x00, // movss xmm0, [GlideData]
                0xE9, 0x00, 0x00, 0x00, 0x00,
                0xc3, 0xf5, 0xc8, 0xbf };
            mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 3);
            mod->shellcode->updateValue<uint32_t>(9, (uint32_t)(mod->patch->data->address + mod->patch->data->size) - ((uint32_t)((uintptr_t)mod->shellcode->data->address + mod->shellcode->data->size - 7)));
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

    void Update(ModContext* modContext) { }

    ModMetaData GetMetaData() {
        return metaData;
    }
};

extern "C" __declspec(dllexport) Mod* CreateModInstance() {
    return new FlightMod();
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

