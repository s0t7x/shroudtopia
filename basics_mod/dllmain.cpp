#include "pch.h"

#include <shroudtopia.h>
#include <mem.h>

#define LOG_CLASS(msg) modContext->Log(std::string(typeid(*this).name()).append(msg).c_str())

ModMetaData metaData = {
	"basics",
	"Enables legacy shroudtopia mods.",
	"1.0",
	"s0T7x",
	"0.0.1",
	true,
	true,
};

// Signature for GameVersion (SVN) 55812
class _Mod {
public:
	ModContext* modContext;
	_Mod(ModContext* modContext) : modContext(modContext) {}
	virtual ~_Mod() {}  // Add a virtual destructor to enable polymorphism

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

class NoStaminaLoss : public _Mod
{
public:
	NoStaminaLoss(ModContext* modContext) : _Mod(modContext)
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

class NoFallDamage : public _Mod
{
public:
	NoFallDamage(ModContext* modContext) : _Mod(modContext)
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
				sizeof(modCode) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
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

class NoCraftCost : public _Mod
{
public:
	NoCraftCost(ModContext* modContext) : _Mod(modContext)
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

class InfiniteItemUse : public _Mod
{
public:
	Mem::Detour* free_ItemUseMod2 = nullptr;

	InfiniteItemUse(ModContext* modContext) : _Mod(modContext)
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
				sizeof(modCode1) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
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
				sizeof(modCode2) - 4, (uint32_t)(free_ItemUseMod2->patch->data->address + free_ItemUseMod2->patch->data->size)
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

// BROKEN! Actually does opposite of what it should... :(
class BypassWorldBorders : public _Mod
{
public:
	BypassWorldBorders(ModContext* modContext) : _Mod(modContext)
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
			// BROKEN! Actually does opposite of what it should... :(
			uint8_t modCode[] = {
				0xF3, 0x0F, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, // movups xmm0, [_P]
				0xF2, 0x0F, 0x10, 0x48, 0x10,                   // movsd xmm1, [_P + 10]
				0xE9, 0x00, 0x00, 0x00, 0x00                    // jmp return (dynamic, needs to be calculated)
			};

			// Creating the detour for the AOB address.
			mod = new Mem::Detour(address + 5, modCode, sizeof(modCode), false, 3);

			// Calculate the jump address for the detour.
			mod->shellcode->updateValue<uint32_t>(
				sizeof(modCode) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
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

class BypassAltarLimit : public _Mod
{
public:
	BypassAltarLimit(ModContext* modContext) : _Mod(modContext)
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
				sizeof(modCode) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
				- ((uint32_t)((uintptr_t)mod->shellcode->data->address
					+ mod->shellcode->data->size))
			);
		}
	}
};


class BasicsMod : public Mod
{
	NoStaminaLoss* noStaminaLoss;
	NoFallDamage* noFallDamage;
	NoCraftCost* noCraftCost;
	InfiniteItemUse* infiniteItemUse;
	BypassWorldBorders* bypassWorldBorders;
	BypassAltarLimit* bypassAltarLimit;

public:
	void Load(ModContext* modContext)
	{
		noStaminaLoss = new NoStaminaLoss(modContext);
		noFallDamage = new NoFallDamage(modContext);
		noCraftCost = new NoCraftCost(modContext);
		infiniteItemUse = new InfiniteItemUse(modContext);
		bypassWorldBorders = new BypassWorldBorders(modContext);
		bypassAltarLimit = new BypassAltarLimit(modContext);
	}

	void Unload(ModContext* modContext) {
		delete noStaminaLoss;
		delete noFallDamage;
		delete noCraftCost;
		delete infiniteItemUse;
		delete bypassWorldBorders;
		delete bypassAltarLimit;
	}

	void Activate(ModContext* modContext) {
		active = true;
		modContext->Log(std::string().append(metaData.name).append(" activated").c_str());
	}

	void Deactivate(ModContext* modContext) {
		if (noStaminaLoss->active) noStaminaLoss->deactivate();
		if (noFallDamage->active) noFallDamage->deactivate();
		if (noCraftCost->active) noCraftCost->deactivate();
		if (infiniteItemUse->active) infiniteItemUse->deactivate();
		if (bypassWorldBorders->active) bypassWorldBorders->deactivate();
		if (bypassAltarLimit->active) bypassAltarLimit->deactivate();
		active = false;
		modContext->Log(std::string().append(metaData.name).append(" deactivated").c_str());
	}

	void Update(ModContext* modContext) {
		const char* modKey = metaData.name.c_str();
		bool noStaminaLossEnabled = modContext->config.GetBool(modKey, "no_stamina_loss", false);
		if (noStaminaLossEnabled && !noStaminaLoss->active)
			noStaminaLoss->activate();
		else if (!noStaminaLossEnabled && noStaminaLoss->active)
			noStaminaLoss->deactivate();

		bool noFallDamageEnabled = modContext->config.GetBool(modKey, "no_fall_damage", false);
		if (noFallDamageEnabled && !noFallDamage->active)
			noFallDamage->activate();
		else if (!noFallDamageEnabled && noFallDamage->active)
			noFallDamage->deactivate();

		bool noCraftCostEnabled = modContext->config.GetBool(modKey, "no_craft_cost", false);
		if (noCraftCostEnabled && !noCraftCost->active)
			noCraftCost->activate();
		else if (!noCraftCostEnabled && noCraftCost->active)
			noCraftCost->deactivate();

		bool infiniteItemUseEnabled = modContext->config.GetBool(modKey, "inf_item_use", false);
		if (infiniteItemUseEnabled && !infiniteItemUse->active)
			infiniteItemUse->activate();
		else if (!infiniteItemUseEnabled && infiniteItemUse->active)
			infiniteItemUse->deactivate();

		bool bypassWorldBordersEnabled = modContext->config.GetBool(modKey, "bypass_world_borders", false);
		if (bypassWorldBordersEnabled && !bypassWorldBorders->active)
			bypassWorldBorders->activate();
		else if (!bypassWorldBordersEnabled && bypassWorldBorders->active)
			bypassWorldBorders->deactivate();

		bool bypassAltarLimitEnabled = modContext->config.GetBool(modKey, "bypass_altar_limit", false);
		if (bypassAltarLimitEnabled && !bypassAltarLimit->active)
			bypassAltarLimit->activate();
		else if (!bypassAltarLimitEnabled && bypassAltarLimit->active)
			bypassAltarLimit->deactivate();

	}

	ModMetaData GetMetaData() {
		return metaData;
	}
};

extern "C" __declspec(dllexport) Mod* CreateModInstance() {
	return new BasicsMod();
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
