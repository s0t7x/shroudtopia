// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "pch.h"
#include "mem.h"
#include <thread>
#include <sstream>
#include <iomanip>

// Signatures for GameVersion (SVN) 558123

class Mod {
public:
    virtual ~Mod() {}  // Add a virtual destructor to enable polymorphism

    bool active = false;
    Mem::Detour* mod = nullptr;

    void activate()
    {
        mod->activate();
        active = mod->active;
        if (active)
            LOG_CLASS("Activated");
        else
            LOG_CLASS("Failed to activate");
    }

    // Deactivates the mod by restoring the original code.
    void deactivate()
    {
        mod->deactivate();
        active = false;
        LOG_CLASS("Deactivated");
    }
};

class GliderFlight : public Mod
{
public:
    GliderFlight()
    {
        const char *pattern = "\xF3\x0F\x10\x05\x00\x00\x00\x00\x77";
        const char *mask = "xxxx????x";

        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
        uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

        if (address)
        {
            uint8_t modCode[] = {
                0xF3, 0x0F, 0x10, 0x05, 0x05, 0x00, 0x00, 0x00, // movss xmm0, [GlideData]
                0xE9, 0x00, 0x00, 0x00, 0x00,
                0xc3, 0xf5, 0xc8, 0xbf};
            mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 3);
            mod->shellcode->updateValue<uint32_t>(9, (uint32_t)(mod->patch->data->address + mod->patch->data->size) - ((uint32_t)((uintptr_t)mod->shellcode->data->address + mod->shellcode->data->size - 4)));
        }
    }
};

class NoStaminaLoss : public Mod
{
public:
    NoStaminaLoss()
    {
        // Pattern matching the AOB scan for the original code in the target process.
        const char* pattern = "\x8B\x04\x81\x89\x44\x24\x3C";
        const char* mask = "xxxxxxx";

        // Base address of the module (the game).
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

        // Find the pattern within the game memory (scans within 16MB).
        uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

        // If the address was found, proceed with allocating memory and creating the detour.
        if (address)
        {
            // The modded code that will replace the original instructions at the found address.
            uint8_t modCode[] = {
                0x53,                               // push rbx
                0x8B, 0x5C, 0x81, 0x08,             // mov ebx, [rcx+rax*4+8]
                0x89, 0x1C, 0x81,                   // mov [rcx+rax*4], ebx
                0x5B,                               // pop rbx
                0x8B, 0x04, 0x81,                   // mov eax, [rcx+rax*4]
                0x89, 0x44, 0x24, 0x3C,             // mov [rsp+3C], eax
                0xE9, 0x00, 0x00, 0x00, 0x00        // jmp return
            };

            // Creating the detour by replacing the original code with our custom modCode.
            mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 2);

            // Calculate the jump address and update the shellcode.
            mod->shellcode->updateValue<uint32_t>(
                sizeof(modCode) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
                - ((uint32_t)((uintptr_t)mod->shellcode->data->address
                    + mod->shellcode->data->size))
            );
        }
    }
};

class NoFallDamage : public Mod
{
public:
    NoFallDamage()
    {
        // Pattern matching the AOB scan for the original code in the target process.
        const char* pattern = "\x89\x04\x91\x48\x8D\x4D\xE0";
        const char* mask = "xxxxxxx";

        // Base address of the module (the game).
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

        uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x10000000);
        if (address)
        {
            // add fall damage as health
            //uint8_t modCode[] = {
            //    0x01, 0x04, 0x91,                    // add[rcx + rdx * 4], eax
            //    0x48, 0x8D, 0x4D, 0xE0,              // lea rcx, [rbp-20]
            //    0xE9, 0x00, 0x00, 0x00, 0x00         // jmp return (dynamic, needs to be calculated)
            //};

            // no fall damage
            uint8_t modCode[] = {
                0x48, 0x83, 0xC0, 0x00,              // add eax, 0 (equivalent to no operation)
                0x48, 0x8D, 0x4D, 0xE0,              // lea rcx, [rbp-20]
                0xE9, 0x00, 0x00, 0x00, 0x00         // jmp return (dynamic, needs to be calculated)
            };

            // Creating the detour by replacing the original code with our custom modCode.
            mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 2);

            // Calculate the jump address and update the shellcode.
            mod->shellcode->updateValue<uint32_t>(
                sizeof(modCode)-4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
                - ((uint32_t)((uintptr_t)mod->shellcode->data->address
                    + mod->shellcode->data->size))
            );
        }
        else
        {
            LOG_CLASS(std::string("Address not found: Base Address: ").append(std::to_string(baseAddress)).c_str());
        }
    }
};

class NoCraftCost : public Mod
{
public:
    NoCraftCost()
    {
        // Pattern matching the AOB scan for the original code in the target process.
        const char* pattern = "\x43\x8B\x74\xF5\x04";
        const char* mask = "xxxxx";

        // Base address of the module (the game).
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

        // Find the pattern within the game memory (scans within 16MB).
        uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

        // If the address was found, proceed with allocating memory and creating the detour.
        if (address)
        {
            // The modded code that will replace the original instructions at the found address.
            uint8_t modCode[] = {
                0x43, 0x8B, 0x74, 0xF5, 0x04, // - mov esi,[r13 + r14 * 8 + 04]
                0xBE, 0x00, 0x00, 0x00, 0x00, // - mov esi,00000000
                0xE9, 0x00, 0x00, 0x00, 0x00  // - jmp
            };

            // Creating the detour by replacing the original code with our custom modCode.
            mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 0);

            // Calculate the jump address and update the shellcode.
            mod->shellcode->updateValue<uint32_t>(
                sizeof(modCode) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
                - ((uint32_t)((uintptr_t)mod->shellcode->data->address
                    + mod->shellcode->data->size))
            );
        }
    }
};

class InfiniteItemUse : public Mod
{
public:
    Mem::Detour* free_ItemUseMod2 = nullptr;

    InfiniteItemUse()
    {
        // AOB patterns for the original code in the target process.
        const char* pattern1 = "\x45\x29\x7E\x04\x41\x2B\xEF"; // First AOB
        const char* pattern2 = "\x33\xC9\x49\x89\x0E";        // Second AOB

        // Base address of the module (the game).
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

        // Find the patterns within the game memory (scans within 16MB).
        uintptr_t address1 = Mem::FindPattern(pattern1, "xxxxxxx", baseAddress, 0xF0000000);
        uintptr_t address2 = Mem::FindPattern(pattern2, "xxxxx", baseAddress, 0xF0000000);

        // Check if both addresses were found.
        if (address1 && address2)
        {
            // Allocate memory for new instructions to be injected at address1.
            uint8_t modCode1[] = {
                0x49, 0x83, 0xFB, 0x01, 0x0F, 0x85, 0x08, 0x00, 0x00, 0x00, 0x44, 0x29, 0xFD, 0xE9, 0x00, 0x00, 0x00, 0x00
            };

            // Creating the detour for the first address.
            mod = new Mem::Detour(address1, modCode1, sizeof(modCode1), false, 2);

            // Calculate the jump address for the first detour.
            mod->shellcode->updateValue<uint32_t>(
                sizeof(modCode1)-4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
                - ((uint32_t)((uintptr_t)mod->shellcode->data->address
                    + mod->shellcode->data->size))
            );

            // Allocate memory for new instructions to be injected at address2.
            uint8_t modCode2[] = {
                0x31, 0xC9,              // xor ecx, ecx
                0x49, 0x83, 0xFB, 0x01,
                0x0F, 0x84, 0x03, 0x00, 0x00, 0x00,
                0x49, 0x89, 0x0E,
                0xE9, 0x00, 0x00, 0x00, 0x00 // jmp
            };

            // Creating the detour for the second address.
            free_ItemUseMod2 = new Mem::Detour(address2, modCode2, sizeof(modCode2), false);

            // Calculate the jump address for the second detour.
            free_ItemUseMod2->shellcode->updateValue<uint32_t>(
                sizeof(modCode2)-4, (uint32_t)(free_ItemUseMod2->patch->data->address + free_ItemUseMod2->patch->data->size)
                - ((uint32_t)((uintptr_t)free_ItemUseMod2->shellcode->data->address
                    + free_ItemUseMod2->shellcode->data->size - 4))
            );
        }
    }

    // Activates the Infinite Item Use mod.
    void activate()
    {
        mod->activate();
        free_ItemUseMod2->activate();
        active = mod->active && free_ItemUseMod2->active;
        if (active)
            LOG_CLASS("Activated");
        else
            LOG_CLASS("Failed to activate");
    }

    // Deactivates the Infinite Item Use mod.
    void deactivate()
    {
        mod->deactivate();
        free_ItemUseMod2->deactivate();
        active = false;
        LOG_CLASS("Deactivated");
    }
};

class BypassWorldBorders : public Mod
{
public:
    BypassWorldBorders()
    {
        // AOB pattern for the original code in the target process.
        const char* pattern = "\x89\x88\x00\x00\x00\x0F\x10\x00\xF2\x0F\x10\x48\x10"; // Unique AOB

        // Base address of the module (the game).
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

        // Find the pattern within the game memory (scans within 16MB).
        uintptr_t address = Mem::FindPattern(pattern, "xxxxxxxxxxxxx", baseAddress, 0xF000000);

        // Check if the address was found.
        if (address)
        {
            // Allocate memory for new instructions to be injected.
            uint8_t modCode[] = {
                0xF3, 0x0F, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, // movups xmm0, [_P]
                0xF2, 0x0F, 0x10, 0x48, 0x10,                   // movsd xmm1, [_P + 10]
                0xE9, 0x00, 0x00, 0x00, 0x00                    // jmp return (dynamic, needs to be calculated)
            };

            // Creating the detour for the AOB address.
            mod = new Mem::Detour(address + 5, modCode, sizeof(modCode), false, 3);

            // Calculate the jump address for the detour.
            mod->shellcode->updateValue<uint32_t>(
                sizeof(modCode)-4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
                - ((uint32_t)((uintptr_t)mod->shellcode->data->address
                    + mod->shellcode->data->size))
            );
        }
    }

    // Activates the Bypass World Borders mod.
    void activate()
    {
        mod->activate();
        active = mod->active;
        if (active)
            LOG_CLASS("Activated BypassWorldBorders");
        else
            LOG_CLASS("Failed to activate BypassWorldBorders");
    }

    // Deactivates the Bypass World Borders mod.
    void deactivate()
    {
        mod->deactivate();
        active = false;
        LOG_CLASS("Deactivated BypassWorldBorders");
    }
};

class BypassAltarLimit : public Mod
{
public:
    BypassAltarLimit()
    {
        // AOB pattern for the original code in the target process.
        const char* pattern = "\x49\x8B\x8E\xD0\x00\x00\x00\x3B"; // Unique AOB

        // Base address of the module (the game).
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

        // Find the pattern within the game memory (scans within 16MB).
        uintptr_t address = Mem::FindPattern(pattern, "xxxxxxxx", baseAddress, 0x1000000);

        // Check if the address was found.
        if (address)
        {
            // Allocate memory for new instructions to be injected.
            uint8_t modCode[] = {
                0x49, 0x8B, 0x8E, 0xD0, 0x00, 0x00, 0x00, // mov rcx, [r14 + 0xD0]
                0xB8, 0xFF, 0x00, 0x00, 0x00, // mov eax, 0xFF
                0xE9, 0x00, 0x00, 0x00, 0x00 // jmp return (dynamic, needs to be calculated)
            };

            // Creating the detour for the AOB address.
            mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 2);

            // Calculate the jump address for the detour.
            mod->shellcode->updateValue<uint32_t>(
                sizeof(modCode)-4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
                - ((uint32_t)((uintptr_t)mod->shellcode->data->address
                    + mod->shellcode->data->size))
            );
        }
    }
};

void PatchMemoryLoop()
{
    Utils::Log("Thread started.");

    Utils::Log(std::string("Loading config from '").append(CONFIG_FILE).append("'...").c_str());
    Config config;
    if (!config.read())
    {
        config.write();
    }
    Utils::Log("Config loaded.");

    GliderFlight gliderFlight;
    NoStaminaLoss noStaminaLoss;
    NoFallDamage noFallDamage;
    NoCraftCost noCraftCost;
    InfiniteItemUse infiniteItemUse;
    BypassWorldBorders bypassWorldBorders;
    BypassAltarLimit bypassAltarLimit;

    Utils::Log("Detours initialized.");

    Utils::Log(std::string("Wait for server. Configured boot delay is ").append(std::to_string(config.boot_delay)).append("ms.").c_str());
    Sleep(config.boot_delay);

    while (true)
    {
        if (config.active) {
           if (config.glider_flight && !gliderFlight.active)
                gliderFlight.activate();
            else if (!config.glider_flight && gliderFlight.active)
                gliderFlight.deactivate();

            if (config.no_stamina_loss && !noStaminaLoss.active)
                noStaminaLoss.activate();
            else if (!config.no_stamina_loss && noStaminaLoss.active)
                noStaminaLoss.deactivate();

            if (config.no_fall_damage && !noFallDamage.active)
                noFallDamage.activate();
            else if (!config.no_fall_damage && noFallDamage.active)
                noFallDamage.deactivate();

            if (config.no_craft_cost && !noCraftCost.active)
                noCraftCost.activate();
            else if (!config.no_craft_cost && noCraftCost.active)
                noCraftCost.deactivate();

            if (config.inf_item_use && !infiniteItemUse.active)
                infiniteItemUse.activate();
            else if (!config.inf_item_use && infiniteItemUse.active)
                infiniteItemUse.deactivate();

            if (config.bypass_world_borders && !bypassWorldBorders.active)
                bypassWorldBorders.activate();
            else if (!config.bypass_world_borders && bypassWorldBorders.active)
                bypassWorldBorders.deactivate();

            if (config.bypass_altar_limit && !bypassAltarLimit.active)
                bypassAltarLimit.activate();
            else if (!config.bypass_altar_limit && bypassAltarLimit.active)
                bypassAltarLimit.deactivate();
        }
        Sleep(2000); // Wait before trying again
        config.reloadIfChanged();
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Utils::Log("Shroudtopia initialized. Starting thread...");
        std::thread(PatchMemoryLoop).detach();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
