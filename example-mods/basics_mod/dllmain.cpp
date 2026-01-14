#include "pch.h"

#include <shroudtopia.h>
#include <memory_utils.h>

#define LOG_CLASS(msg) modContext->Log(std::string(typeid(*this).name()).append(msg).c_str())

ModMetaData metaData = {
	"basics",
	"Enables legacy shroudtopia mods.",
	"1.1-2026",
	"s0T7x",
	"0.0.1",
	true,
	true,
};

// Signature for GameVersion (SVN) 874630
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
		// AOB: 8B 04 91 89 44 24 40
		const char* pattern = "\x8B\x04\x91\x89\x44\x24\x40";
		const char* mask = "xxxxxxx";
		uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
		uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x2000000);

		if (address)
		{
			uint8_t modCode[] = {
				0x53,                               // push rbx
				0x8B, 0x5C, 0x91, 0x08,             // mov ebx, [rcx+rdx*4+08] (Max Stamina)
				0x89, 0x1C, 0x91,                   // mov [rcx+rdx*4], ebx    (Set Current to Max)
				0x5B,                               // pop rbx
				0x8B, 0x04, 0x91,                   // mov eax, [rcx+rdx*4]
				0x89, 0x44, 0x24, 0x40,             // mov [rsp+40], eax
				0xE9, 0x00, 0x00, 0x00, 0x00        // jmp return
			};

			mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 2);
			mod->shellcode->updateValue<uint32_t>(
				sizeof(modCode) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
				- ((uint32_t)((uintptr_t)mod->shellcode->data->address + mod->shellcode->data->size))
			);
		}
	}
};

class NoFallDamage : public _Mod
{
public:
	NoFallDamage(ModContext* modContext) : _Mod(modContext)
	{
		// We scan for 7 bytes now to make sure we have room for the 5-byte Jump
		// Original instructions:
		// 45 89 0C 88    - mov [r8+rcx*4], r9d (4 bytes)
		// 48 8B CB       - mov rcx, rbx        (3 bytes)
		// Total: 7 bytes
		const char* pattern = "\x45\x89\x0C\x88\x48\x8B\xCB";
		const char* mask = "xxxxxxx";

		uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
		uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x10000000);

		if (address)
		{
			uint8_t modCode[] = {
				0x45, 0x01, 0x0C, 0x88,             // add [r8+rcx*4], r9d (Your Cheat)
				0x48, 0x8B, 0xCB,                   // mov rcx, rbx        (The instruction we had to overwrite)
				0xE9, 0x00, 0x00, 0x00, 0x00        // jmp return
			};

			// We use 2 NOPs because: 
			// 5 (Jump size) + 2 (NOPs) = 7 (Total bytes overwritten)
			mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 2);

			// Calculate jump back to address + 7
			mod->shellcode->updateValue<uint32_t>(
				sizeof(modCode) - 4,
				(uint32_t)(address + 7) - ((uint32_t)((uintptr_t)mod->shellcode->data->address + sizeof(modCode)))
			);
		}
		else
		{
			LOG_CLASS(" NoFallDamage: Pattern not found!");
		}
	}
};

class InfiniteItemUse : public _Mod
{
public:
	InfiniteItemUse(ModContext* modContext) : _Mod(modContext)
	{
		const char* pattern = "\x41\x29\x6E\x04";
		uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
		uintptr_t address = Mem::FindPattern(pattern, "xxxx", baseAddress, 0xF0000000);
		if (address)
		{
			uint8_t modCode[] = {
				0x41, 0xBF, 0x00, 0x00, 0x00, 0x00, 
				0xBD, 0x00, 0x00, 0x00, 0x00, 
				0xE9, 0x00, 0x00, 0x00, 0x00
			};

			mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 2);

			// Calculate the jump address for detour
			mod->shellcode->updateValue<uint32_t>(
				sizeof(modCode) - 4, (uint32_t)(mod->patch->data->address + mod->patch->data->size)
				- ((uint32_t)((uintptr_t)mod->shellcode->data->address
					+ mod->shellcode->data->size))
			);
		}
	}

	// Activates the Infinite Item Use mod.
	void activate()
	{
		mod->activate();
		active = mod->active;
		if (active)
			LOG_CLASS("Activated");
		else
			LOG_CLASS("Failed to activate");
	}

	// Deactivates the Infinite Item Use mod.
	void deactivate()
	{
		mod->deactivate();
		active = false;
		LOG_CLASS("Deactivated");
	}
};

class NoResourceCost : public _Mod
{
public:
	NoResourceCost(ModContext* modContext) : _Mod(modContext)
	{
		// Pattern matching the AOB scan for the original code in the target process.
		const char* pattern = "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x18\x48\x89\x7C\x24\x20\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x70\xFF\xFF\xFF\x48\x81\xEC\x90\x01\x00\x00\x44\x8B\xBD\xE8\x00\x00\x00\x4C\x8B\xE1\x48\x8B\x0A\x41\x8B\xF9\x48\x8B\x5A\x10\x41\x8B\xF0\x4C\x8B\xEA\x48\x8B\x09";
		const char* mask = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

		// Base address of the module (the game).
		uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

		uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x10000000);
		if (address)
		{
			// no fall damage
			uint8_t modCode[] = {
				0xC7, 0x44, 0x24, 0x30, 0x00, 0x00, 0x00, 0x00,	// - mov[rsp + 30],00000000
				0x48, 0x89, 0x5C, 0x24, 0x08,					// - mov[rsp + 08],rbx
				0xE9, 0x00, 0x00, 0x00, 0x00					// - jmp return
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
		else
		{
			LOG_CLASS(std::string("Address not found: Base Address: ").append(std::to_string(baseAddress)).c_str());
		}
	}
};

class UnlockBlueprints : public _Mod
{
public:
	UnlockBlueprints(ModContext* modContext) : _Mod(modContext)
	{
		if (modContext->game.isServer) return;
		// Pattern matching the AOB scan for the original code in the target process.
		const char* pattern = "\xC9\x74\x5E\x44\x8B\x1A\x48\x8D\x43\xFF";
		const char* mask = "xxxxxxxxxx";

		// Base address of the module (the game).
		uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

		uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x10000000);
		if (address)
		{
			// no fall damage
			uint8_t modCode[] = {
				0x41,0xBB,0x82,0x53,0x2B,0x54,	// - mov r11d,542B5382
				0x48,0x8D,0x43,0xFF,			// - lea rax,[rbx - 01]
				0xE9,0x27,0x9F,0x89,0x00		// - jmp enshrouded.exe + 879F36

			};

			// Creating the detour by replacing the original code with our custom modCode.
			mod = new Mem::Detour(address + 3, modCode, sizeof(modCode), false, 2);

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

	void activate()
	{
		if (modContext->game.isServer) {
			LOG_CLASS("Cannot activate on server");
			return;
		}
		mod->activate();
		active = mod->active;
		if (active)
			LOG_CLASS("Activated");
		else
			LOG_CLASS("Failed to activate");
	}

};

class InfItemSplit : public _Mod
{
public:
	InfItemSplit(ModContext* modContext) : _Mod(modContext)
	{
		// Pattern matching the AOB scan for the original code in the target process.
		const char* pattern = "\x29\x6E\x04\xEB\xC5";
		const char* mask = "xxxxx";

		const char* handlingPattern = "\x0F\x10\x44\x24\x28\xC6\x07\x00";
		const char* handlingMask = "xxxxxxxx";

		// Base address of the module (the game).
		uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

		uintptr_t address = Mem::FindPattern(pattern, mask, baseAddress, 0x10000000);
		uintptr_t handlingAddress = Mem::FindPattern(handlingPattern, handlingMask, baseAddress, 0x10000000);
		if (address && handlingAddress)
		{
			uint8_t modCode[] = {
				0x83, 0x6E, 0x04, 0x00,			// - sub dword ptr[rsi + 04],00
				0xE9, 0xCA, 0x75, 0x71, 0x00,	// - jmp ItemHandling
				0xE9, 0x00, 0x76, 0x71, 0x00	// - jmp enshrouded_server.exe + 70760E
			};

			// Creating the detour by replacing the original code with our custom modCode.
			mod = new Mem::Detour(address, modCode, sizeof(modCode), false, 1);

			mod->shellcode->updateValue<uint32_t>(
				sizeof(modCode) - 9,
				(uint32_t)(handlingAddress - ((uintptr_t)mod->shellcode->data->address
					+ mod->shellcode->data->size))
			);

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

class BasicsMod : public Mod
{
	NoStaminaLoss* noStaminaLoss;
	NoFallDamage* noFallDamage;
	InfiniteItemUse* infiniteItemUse;
	NoResourceCost* noResourceCost;
	UnlockBlueprints* unlockBlueprints;
	InfItemSplit* infItemSplit;

public:
	void Load(ModContext* modContext)
	{
		noStaminaLoss = new NoStaminaLoss(modContext);
		noFallDamage = new NoFallDamage(modContext);
		noResourceCost = new NoResourceCost(modContext);
		infiniteItemUse = new InfiniteItemUse(modContext);
		unlockBlueprints = new UnlockBlueprints(modContext);
		infItemSplit = new InfItemSplit(modContext);
	}

	void Unload(ModContext* modContext) {
		delete noStaminaLoss;
		delete noFallDamage;
		delete noResourceCost;
		delete infiniteItemUse;
		delete unlockBlueprints;
		delete infItemSplit;
	}

	void Activate(ModContext* modContext) {
		active = true;
		modContext->Log(std::string().append(metaData.name).append(" activated").c_str());
	}

	void Deactivate(ModContext* modContext) {
		if (noStaminaLoss->active) noStaminaLoss->deactivate();
		if (noFallDamage->active) noFallDamage->deactivate();
		if (noResourceCost->active) noResourceCost->deactivate();
		if (infiniteItemUse->active) infiniteItemUse->deactivate();
		if (unlockBlueprints->active) unlockBlueprints->deactivate();
		if (infItemSplit->active) infItemSplit->deactivate();
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


		bool infiniteItemUseEnabled = modContext->config.GetBool(modKey, "_inf_item_use", false);
		if (infiniteItemUseEnabled && !infiniteItemUse->active)
			infiniteItemUse->activate();
		else if (!infiniteItemUseEnabled && infiniteItemUse->active)
			infiniteItemUse->deactivate();

		bool noResourceCostEnabled = modContext->config.GetBool(modKey, "_no_resource_cost", false);
		if (noResourceCostEnabled && !noResourceCost->active)
			noResourceCost->activate();
		else if (!noResourceCostEnabled && noResourceCost->active)
			noResourceCost->deactivate();


		if(modContext->game.isClient) {
			bool unlockBlueprintsEnabled = modContext->config.GetBool(modKey, "_unlock_blueprints", false);
			if (unlockBlueprintsEnabled && !unlockBlueprints->active)
				unlockBlueprints->activate();
			else if (!unlockBlueprintsEnabled && unlockBlueprints->active)
				unlockBlueprints->deactivate();
		}


		bool infItemSplitEnabled = modContext->config.GetBool(modKey, "_inf_item_split", false);
		if (infItemSplitEnabled && !infItemSplit->active)
			infItemSplit->activate();
		else if (!infItemSplitEnabled && infItemSplit->active)
			infItemSplit->deactivate();
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

