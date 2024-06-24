// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "pch.h"
#include "mem.h"
#include <thread>

bool ApplyExpMultiplier(const Config& config) {
    const char* pattern = "\x48\x03\xC3\x01\x34\x88\x48\x8B";
    const char* mask = "xxxxxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

    if (address) {
        void* newmem = Mem::AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            uint8_t newCode[] = {
                0x0F, 0xAF, 0x35, 0x00, 0x00, 0x00, 0x00,  // imul esi, [multipler_exp]
                0x48, 0x03, 0xC3,                          // add rax, rbx
                0x01, 0x34, 0x88,                          // add [rax+rcx*4], esi
                0xE9, 0x00, 0x00, 0x00, 0x00               // jmp return
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,  // jmp newmem
                0x90                           // nop
            };

            uintptr_t multDataAddress = (uintptr_t)newmem + sizeof(newCode);

            *(int32_t*)(newCode + 3) = (int32_t)(multDataAddress - ((uintptr_t)newmem + 7));  // Offset to multipler_exp

            *(int32_t*)(newCode + 14) = (int32_t)((address + sizeof(patch)) - ((uintptr_t)newmem + sizeof(newCode)));

            int expMultiplier = config.exp_multiplier;
            memcpy((void*)multDataAddress, &config.exp_multiplier, sizeof(int));

            memcpy(newmem, newCode, sizeof(newCode));


            *(int32_t*)(patch + 1) = (uintptr_t)newmem - (address + 5);

            Mem::Write(address, patch, sizeof(patch));

            Utils::Log(std::string("Exp multiplier set to: ").append(std::to_string(config.exp_multiplier)).c_str());
            return true;
        }
    }

    Utils::Log("Failed to set exp multiplier");
    return false;
}

bool ApplyGliderFlight(const Config& config) {
    if (!config.glider_flight) return true;

    const char* pattern = "\xF3\x0F\x10\x05\x00\x00\x00\x00\x77";
    const char* mask = "xxxx????x";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000); // Scan 16MB

    if (address) {
        void* newmem = Mem::AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            float glideData = -1.57f;

            uint8_t newCode[] = {
                0xF3, 0x0F, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00,  // movss xmm0, [GlideData]
                0xE9, 0x00, 0x00, 0x00, 0x00,                    // jmp returnAddress
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,  // jmp newmem
                0x90, 0x90, 0x90               // nop 3
            };

            uintptr_t returnAddress = address + sizeof(patch);

            uintptr_t glideDataAddress = (uintptr_t)newmem + sizeof(newCode);
            *(uint32_t*)(newCode + 4) = (uintptr_t)glideDataAddress - ((uintptr_t)newmem + 8);  // Offset to GlideData
            *(uint32_t*)(newCode + 9) = (uintptr_t)returnAddress - ((uintptr_t)newmem + sizeof(newCode));  // Offset to return address

            memcpy((void*)glideDataAddress, &glideData, sizeof(glideData));  // Write float value to GlideData
            memcpy(newmem, newCode, sizeof(newCode));



            *(uint32_t*)(patch + 1) = (uintptr_t)newmem - (address + 5);
            Mem::Write(address, patch, sizeof(patch));

            Utils::Log("Activated GliderFlight");
            return true;
        }
    }
    Utils::Log("Failed to activate GliderFlight");
    return false;
}

bool ApplyNoStaminaLoss(const Config& config) {
    if (!config.no_stamina_loss) return true;

    const char* pattern = "\x8B\x04\x81\x89\x44\x24\x3C";
    const char* mask = "xxxxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

    if (address) {
        void* newmem = Mem::AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            uint8_t newCode[] = {
                0x53,                               // push rbx
                0x8B, 0x5C, 0x81, 0x08,             // mov ebx, [rcx+rax*4+8]
                0x89, 0x1C, 0x81,                   // mov [rcx+rax*4], ebx
                0x5B,                               // pop rbx
                0x8B, 0x04, 0x81,                   // mov eax, [rcx+rax*4]
                0x89, 0x44, 0x24, 0x3C,             // mov [rsp+3C], eax
                0xE9, 0x00, 0x00, 0x00, 0x00        // jmp return
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,  // jmp newmem
                0x66, 0x90                     // nop 2 (x86 nop)
            };

            int32_t returnAddress = (int32_t)((address + sizeof(patch)) - ((uintptr_t)newmem + sizeof(newCode)));
            *(int32_t*)(newCode + sizeof(newCode) - 4) = returnAddress;

            memcpy(newmem, newCode, sizeof(newCode));

            *(int32_t*)(patch + 1) = (int32_t)((uintptr_t)newmem - (address + 5));
            Mem::Write(address, patch, sizeof(patch));

            Utils::Log("Activated NoStaminaLoss");

            return true;
        }

    }

    Utils::Log("Failed to activate NoStaminaLoss");
    return false;
}

bool ApplyNoFallDamage(const Config& config) {
    if (!config.no_fall_damage) return true;

    // \x40\x53\x48\x83\xEC\x60\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xCB\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\xD9\x00\x00\x00\x48\x89\x7C\x24\x70
    // xxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxxxxxxxxxxxx
    const char* pattern = "\x40\x53\x48\x83\xEC\x60\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x41\xB8\x20\x00\x00\x00\x48\x8D\x54\x24\x20\x48\x8B\xCB\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\xD9\x00\x00\x00\x48\x89\x7C\x24\x70";
    const char* mask = "xxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxxxxxxxxxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

    if (address) {
        uint8_t patch[] = {
            0xC3
        };

        Mem::Write(address, patch, sizeof(patch));

        Utils::Log("Activated NoFallDamage");

        return true;
    }

    Utils::Log("Failed to activate NoFallDamage");
    return false;
}

bool ApplyNoDurabilityLoss(const Config& config) {
    if (!config.no_durability_loss) return true;

    const char* pattern = "\x0F\x84\x00\x00\x00\x00\x8B\x00\x00\x48\x03\x00\x01";
    const char* mask = "xx????x??xx?x";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

    if (address) {

        void* newmem = Mem::AllocateMemoryNearAddress((LPVOID)address, 0x1000);

        if (newmem) {
            uint8_t newCode[] = {
              0x85, 0xF6,
              0x0F, 0x89, 0x08, 0x00, 0x00, 0x00,
              0x48, 0x03, 0xC3,
              0xE9, 0x00, 0x00, 0x00, 0x00, // return addr
              0x48, 0x03, 0xC3,
              0x01, 0x34, 0x88,
              0xE9, 0x00, 0x00, 0x00, 0x00, // return addr
            };

            uint8_t patch[] = {
                0xE9, 0x00, 0x00, 0x00, 0x00,
                0x90
            };

            uintptr_t returnAddress = address + sizeof(patch);

            *(uint32_t*)(newCode + 12) = (uintptr_t)returnAddress - ((uintptr_t)newmem + 16);
            *(uint32_t*)(newCode + 23) = (uintptr_t)returnAddress - ((uintptr_t)newmem + sizeof(newCode));

            memcpy(newmem, newCode, sizeof(newCode));

            *(uint32_t*)(patch + 1) = (uintptr_t)newmem - ((uintptr_t)address + 5);

            Mem::Write(address, patch, sizeof(patch));

            Utils::Log("Activated NoDurabilityLoss");

            return true;
        }
    }

    Utils::Log("Failed to activate NoDurabilityLoss");
    return false;
}

bool ApplyCloneItemSplits(const Config& config) {
    if (!config.clone_item_splits) return true;

    const char* pattern = "\x29\x00\x04\xEB\x00\x0F\x57";
    const char* mask = "x?xx?xx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

    if (address) {
        uint8_t patch[] = {
            0x90, 0x90, 0x90 // NOP 3
        };

        Mem::Write(address, patch, sizeof(patch));

        Utils::Log("Activated CloneItemSplits");

        return true;
    }

    Utils::Log("Failed to activate CloneItemSplits");
    return false;
}

bool ApplyFreeCraft(const Config& config) {
    if (!config.free_craft) return true;

    const char* pattern = "\x43\x8B\x74\xF5\x04";
    const char* mask = "xxxxx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

    if (address) {

        uint8_t patch[] = {
            0xBE, 0x00, 0x00, 0x00, 0x00
        };

        Mem::Write(address, patch, sizeof(patch));

        Utils::Log("Activated FreeCraft");

        return true;
    }

    Utils::Log("Failed to activate FreeCraft");
    return false;
}

bool ApplyFreeBuild(const Config& config) {
    if (!config.free_craft) return true;

    const char* pattern = "\x48\x0F\x00\x00\x00\x8B\x00\x00\x8B\x00\x04\x89";
    const char* mask = "xx???x??x?xx";

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x1000000);

    if (address) {

        uint8_t patch[] = {
            0x31, 0xF6, 0x90, 0x90
        };

        Mem::Write(address + 7, patch, sizeof(patch));

        Utils::Log("Activated FreeBuild");

        return true;
    }

    Utils::Log("Failed to activate FreeBuild");
    return false;
}

void PatchMemoryLoop() {
    Utils::Log("Thread started.");

    Utils::Log(std::string("Loading config from '").append(CONFIG_FILE).append("'...").c_str());
    Config config;
    if (!config.read()) {
        config.write();
    }
    Utils::Log("Config loaded.");

    static bool ActivatedExpMultiplier = false;
    static bool ActivatedGliderFlight = false;
    static bool ActivatedNoFallDamage = false;
    static bool ActivatedNoStaminaLoss = false;
    static bool ActivatedNoDurabilityLoss = false;
    static bool ActivatedCloneItemSplits = false;
    static bool ActivatedFreeCraft = false;
    static bool ActivatedFreeBuild = false;

    Utils::Log(std::string("Wait for server. Configured boot delay is ").append(std::to_string(config.boot_delay)).append("ms.").c_str());
    Sleep(config.boot_delay);

    // Silent retries if some patch didnt work initally
    static int retries = 0;
    while (retries < 10) {

        if (!ActivatedExpMultiplier)    ActivatedExpMultiplier = ApplyExpMultiplier(config);
        if (!ActivatedGliderFlight)    ActivatedGliderFlight = ApplyGliderFlight(config);
        if (!ActivatedNoStaminaLoss)    ActivatedNoStaminaLoss = ApplyNoStaminaLoss(config);
        if (!ActivatedNoFallDamage)    ActivatedNoFallDamage = ApplyNoFallDamage(config);
        if (!ActivatedNoDurabilityLoss)    ActivatedNoDurabilityLoss = ApplyNoDurabilityLoss(config);
        if (!ActivatedCloneItemSplits)    ActivatedCloneItemSplits = ApplyCloneItemSplits(config);
        if (!ActivatedFreeCraft)    ActivatedFreeCraft = ApplyFreeCraft(config);
        if (!ActivatedFreeBuild)    ActivatedFreeBuild = ApplyFreeBuild(config);

        retries++;
        Sleep(2000);  // Wait for 1 second before trying again
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
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

