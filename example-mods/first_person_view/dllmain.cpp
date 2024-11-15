#include "pch.h"

#include <shroudtopia.h>
#include <memory_utils.h>

ModMetaData metaData = {
    "FirstPersonView",
    "Enables simple but immersive 1st Person view. (Client only)",
    "1.0",
    "s0T7x",
    "0.0.1",
    true,
    false
};


// Signature for GameVersion (SVN) 55812
class FirstPersonMod : public Mod
{
    Mem::Detour* mod;
public:
    void Load(ModContext* modContext)
    {
        const char* pattern = "\xF3\x0F\x11\x9F\x2C\x07\x00\x00";
        const char* mask = "xxxxxxxx";

        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
        uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

        if (address)
        {
            uint8_t modCode[] = {
                0xF3,0x0F,0x11,0x9F,0x2C,0x07,0x00,0x00,
                0xC7,0x87,0x2C,0x07,0x00,0x00,0x00,0x00,0x00,0x00,
                0xC7,0x87,0x30,0x07,0x00,0x00,0x00,0x00,0x80,0xBF,
                0xE9,0x00,0x00,0x00,0x00
            };
            mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 3);
            mod->shellcode->updateValue<uint32_t>(sizeof(modCode)-4, (uint32_t)(mod->patch->data->address + mod->patch->data->size) - ((uint32_t)((uintptr_t)mod->shellcode->data->address + mod->shellcode->data->size)));
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
    return new FirstPersonMod();
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

