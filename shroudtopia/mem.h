#pragma once
#include "pch.h"

#include "utils.h"

#include <exception>
#include <Psapi.h>


namespace Mem
{
	// Error logger function
	void LogError(const std::string& message, const std::exception& ex) {
		Utils::Log(Utils::ERRR, std::string().append(message).append(": ").append(ex.what()).c_str());
	}

	class MemoryProtector {
	public:
		// Constructor takes the target address and size, and changes the protection to the specified type (default is PAGE_EXECUTE_READWRITE).
		MemoryProtector(LPVOID targetAddress, SIZE_T size)
			: address(targetAddress), size(size), oldProtection(0), protectionChanged(false) {}

		bool changeProtection(DWORD newProtection = PAGE_EXECUTE_READWRITE) {
			if (VirtualProtect(address, size, newProtection, &oldProtection)) {
				protectionChanged = true;
				return true;
			}
			else {
				Utils::Log(Utils::ERRR, std::string("Failed to change memory protection. Error: ").append(std::to_string(GetLastError())).c_str());
			}
		}

		// Destructor restores the original protection if it was changed.
		~MemoryProtector() {
			if (protectionChanged) {
				if (!VirtualProtect(address, size, oldProtection, &oldProtection)) {
					Utils::Log(Utils::ERRR, std::string("Failed to restore original memory protection. Error: ").append(std::to_string(GetLastError())).c_str());
				}
			}
		}

		// Disable copy semantics to avoid accidental misuse
		MemoryProtector(const MemoryProtector&) = delete;
		MemoryProtector& operator=(const MemoryProtector&) = delete;

		// Allow move semantics
		MemoryProtector(MemoryProtector&& other) noexcept
			: address(other.address), size(other.size), oldProtection(other.oldProtection), protectionChanged(other.protectionChanged) {
			other.protectionChanged = false;  // Invalidate the original object after move
		}

		MemoryProtector& operator=(MemoryProtector&& other) noexcept {
			if (this != &other) {
				// Restore the original protection of the current object
				if (protectionChanged) {
					VirtualProtect(address, size, oldProtection, &oldProtection);
				}

				// Move the data
				address = other.address;
				size = other.size;
				oldProtection = other.oldProtection;
				protectionChanged = other.protectionChanged;

				// Invalidate the original object after move
				other.protectionChanged = false;
			}
			return *this;
		}

		// Manually restore the original protection
		bool restoreProtection() {
			if (protectionChanged) {
				protectionChanged = !VirtualProtect(address, size, oldProtection, &oldProtection);
				return !protectionChanged;
			}
			return false;
		}

	private:
		LPVOID address;           // Target address whose protection is modified
		SIZE_T size;              // Size of the memory region
		DWORD oldProtection;      // Stores the original protection type
		bool protectionChanged;   // Tracks if the protection was changed successfully
	};

	bool WriteToReadOnlyMemory(LPVOID targetAddress, LPVOID data, SIZE_T size)
	{
		try {
			MemoryProtector protector(targetAddress, size);
			if (!protector.changeProtection(PAGE_EXECUTE_READWRITE)) {
				return false;
			}
			SIZE_T bytesWritten;
			if (!WriteProcessMemory(GetCurrentProcess(), targetAddress, data, size, &bytesWritten) || bytesWritten != size) {
				protector.restoreProtection();
				return false;
			}
			return protector.restoreProtection();
		}
		catch (const std::exception& ex) {
			LogError("Failed to write to read-only memory", ex);
			return false;
		}
	}

	uintptr_t FindPattern(const char* pattern, const char* mask, uintptr_t start, size_t length)
	{
		try {
			MEMORY_BASIC_INFORMATION mbi;
			uintptr_t currentAddress = start;
			while (currentAddress < start + length) {
				if (VirtualQuery((LPCVOID)currentAddress, &mbi, sizeof(mbi)) == sizeof(mbi)) {
					if (mbi.State == MEM_COMMIT &&
						(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE | PAGE_READONLY | PAGE_EXECUTE_READWRITE | PAGE_READWRITE))) {
						for (uintptr_t i = (uintptr_t)mbi.BaseAddress; i < (uintptr_t)mbi.BaseAddress + mbi.RegionSize - strlen(mask); i++) {
							bool found = true;
							for (size_t j = 0; mask[j] != '\0'; j++) {
								if (mask[j] != '?' && pattern[j] != *(char*)(i + j)) {
									found = false;
									break;
								}
							}
							if (found) return i;
						}
					}
					currentAddress += mbi.RegionSize;
				}
				else break;
			}
		}
		catch (const std::exception& ex) {
			LogError("Error finding pattern", ex);
		}
		return 0;
	}

	// Optimized and fail-safe write
	static bool Write(uintptr_t address, const void* buffer, size_t size)
	{
		try {
			MemoryProtector protector((LPVOID)address, size);
			if (!protector.changeProtection(PAGE_EXECUTE_READWRITE)) return false;
			memcpy((void*)address, buffer, size);
			return protector.restoreProtection();
		}
		catch (const std::exception& ex) {
			LogError("Error during write", ex);
			return false;
		}
	}

	static bool Read(uintptr_t address, void* buffer, size_t size)
	{
		try {
			MemoryProtector protector((LPVOID)address, size);
			if (!protector.changeProtection(PAGE_EXECUTE_READWRITE)) return false;
			memcpy(buffer, (void*)address, size);
			return protector.restoreProtection();
		}
		catch (const std::exception& ex) {
			LogError("Error during read", ex);
			return false;
		}
	}

	// Helper function to safely write a jump instruction
	void WriteJump(LPVOID target, LPVOID destination) {
		try {
			MemoryProtector protector(target, 5);
			if (protector.changeProtection(PAGE_EXECUTE_READWRITE)) {
				DWORD offset = ((DWORD)destination - (DWORD)target) - 5;
				*(BYTE*)target = 0xE9;  // JMP instruction
				*(DWORD*)((BYTE*)target + 1) = offset;
				protector.restoreProtection();
			}
		}
		catch (const std::exception& ex) {
			LogError("Failed to write jump", ex);
		}
	}

	// Allocate memory near an address, fail-safe with retries
	LPVOID AllocateMemoryNearAddress(LPVOID address, SIZE_T dwSize)
	{
		try {
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			uintptr_t targetRange = (uintptr_t)address - si.dwAllocationGranularity * 10;
			LPVOID allocatedMemory = nullptr;

			int tries = 0;
			const int maxTries = 10;
			while (!allocatedMemory && tries < maxTries) {
				allocatedMemory = VirtualAllocEx(GetCurrentProcess(), (LPVOID)(targetRange - tries * 0x10000000), dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				tries++;
			}

			if (!allocatedMemory) {
				throw std::runtime_error("Failed to allocate memory near target address");
			}
			return allocatedMemory;
		}
		catch (const std::exception& ex) {
			LogError("Memory allocation error", ex);
			return nullptr;
		}
	}

	uintptr_t GetModuleBaseAddress(const std::string& moduleName) {
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

	class MemoryData
	{
	public:
		uintptr_t address;
		uint8_t* opcode;
		size_t size;

		MemoryData(uintptr_t addr, uint8_t* op, size_t sz)
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

		void updateOpcode(size_t offset, uint8_t* data, size_t dataSize)
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
				*reinterpret_cast<T*>(opcode + offset) = value;
			}
		}
	};

	class Shellcode
	{
	public:
		MemoryData* data;
		bool injected = false;

		Shellcode(uint8_t* opcode, size_t opcodeSize, uintptr_t nearAddr = 0)
		{
			uintptr_t address = (uintptr_t)Mem::AllocateMemoryNearAddress(reinterpret_cast<LPVOID>(nearAddr), 0x1000);
			data = new MemoryData(address, opcode, opcodeSize);
		}

		~Shellcode()
		{
			if (injected)
			{
				uint8_t* nullData = (uint8_t*)malloc(data->size);
				if (nullData) {
					memset(nullData, 0x90, data->size); // Use NOP (0x90) for nulling
					MemoryData nuller(data->address, nullData, data->size);
					nuller.write();
					free(nullData);
				}
			}
			delete data;
		}

		bool inject()
		{
			if (data->write())
				return injected = true;
			return false;
		}

		void updateOpcode(size_t offset, uint8_t* newOpcode, size_t newSize)
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
		MemoryData* data;
		uint8_t* originalBytes = nullptr;
		bool patched = false;

		Patch(uintptr_t address, uint8_t* opcode, size_t opcodeSize)
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

		void updateOpcode(size_t offset, uint8_t* newOpcode, size_t newSize)
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
		Shellcode* shellcode = nullptr;
		Patch* patch = nullptr;

		Detour(uintptr_t address, uint8_t* opcode, size_t opcodeSize, bool trampoline = true, size_t padding = 0)
		{
			uint8_t jmp[1] = { 0xE9 };
			uint8_t nop[1] = { 0x90 };

			size_t patchDataSize = 5 + padding;

			if (trampoline)
			{
				size_t shellcodeDataSize = 5 + opcodeSize;
				uint8_t* shellcodeData = new uint8_t[shellcodeDataSize];
				shellcode = new Shellcode(shellcodeData, shellcodeDataSize, address);
				shellcode->updateOpcode(opcodeSize, jmp, 1);
				shellcode->updateValue<uint32_t>(opcodeSize + 1, (uint32_t)(address + patchDataSize));
				delete[] shellcodeData;
			}
			else
			{
				shellcode = new Shellcode(opcode, opcodeSize, address);
			}

			uint8_t* patchData = new uint8_t[patchDataSize];
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
