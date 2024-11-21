#include "pch.h"
#include "defines.h"
#include "mem.h"
#include "utils.h"
#include "shroudtopia.h"

#include <filesystem>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

ModContext* modContext;

json Config::jConfig;
std::filesystem::file_time_type Config::lastModifiedTime;

typedef Mod* (*CreateModFunc)();
std::string activeModName = "";

// Unload a DLL and log the operation
void UnloadDll(HMODULE hModule) {
    if (hModule) {
        Utils::Log(Utils::INFO, "Unloading DLL.");
        FreeLibrary(hModule);
    }
}

// Load a mod and register it
void LoadAndRegisterMod(const std::string& dllPath) {
    Utils::Log(Utils::DEBUG, std::string("Loading DLL: ").append(dllPath).c_str());

    HMODULE hMod = LoadLibraryA(dllPath.c_str());
    if (!hMod) {
        Utils::Log(Utils::ERRR, std::string("Failed to load DLL: ").append(dllPath).c_str());
        return;
    }

    CreateModFunc createMod = (CreateModFunc)GetProcAddress(hMod, "CreateModInstance");
    if (!createMod) {
        Utils::Log(Utils::ERRR, std::string("CreateModInstance function not found in DLL: ").append(dllPath).c_str());
        FreeLibrary(hMod);
        return;
    }

    Mod* modInstance = createMod();
    if (!modInstance) {
        Utils::Log(Utils::ERRR, "Failed to create mod instance.");
        FreeLibrary(hMod);
        return;
    }

    Utils::Log(Utils::INFO, std::string("Registering mod: ").append(modInstance->GetMetaData().name).c_str());
    modContext->registeredMods[dllPath] = modInstance;
}

std::map<std::string, HMODULE> loadedDLLs;

// Check for new DLLs and load them
void CheckAndLoadDlls(const std::string& modFolder = SHROUDTOPIA_MOD_FOLDER) {
    // Load new DLLs
    for (const auto& entry : fs::directory_iterator(modFolder)) {
        if (entry.path().extension() == ".dll") {
            std::string dllPath = entry.path().string();

            auto it = loadedDLLs.find(dllPath);
            if (it == loadedDLLs.end()) {
                // Load new DLL
                Utils::Log(Utils::DEBUG, std::string("Loading new DLL: ").append(dllPath).c_str());
                LoadAndRegisterMod(dllPath);
                loadedDLLs[dllPath] = LoadLibraryA(dllPath.c_str());
            }
        }
    }
}

// Load and update mods
void UpdateMods() {
    // Load unloaded mods
    for (auto& [dllPath, regMod] : modContext->registeredMods) {
        if (!regMod->loaded) {
            ModMetaData modMeta = regMod->GetMetaData();
            activeModName = modMeta.name;
            if ((modContext->game.isServer && !modMeta.hasServerSupport) ||
                (!modContext->game.isServer && !modMeta.hasClientSupport)) {
                continue;
            }
            Utils::Log(Utils::INFO, std::string("Loading mod: ").append(modMeta.name).c_str());
            regMod->Load(modContext);
            regMod->loaded = true;
            modContext->loadedMods.insert(regMod);
        }
    }

    // Update active mods
    for (Mod* mod : modContext->loadedMods) {
        ModMetaData meta = mod->GetMetaData();
        activeModName = meta.name;
        bool shouldBeActive = Config::modGet<bool>(meta.name.c_str(), "active", false);
        if (shouldBeActive) {
            if (!mod->active) {
                Utils::Log(Utils::INFO, std::string("Activating mod: ").append(meta.name).c_str());
                mod->Activate(modContext);
            }
            mod->Update(modContext);
        }
        else if (mod->active) {
            Utils::Log(Utils::INFO, std::string("Deactivating mod: ").append(meta.name).c_str());
            mod->Deactivate(modContext);
        }
    }
}

json defaultConfig({
    {"active", true},
    {"bootDelay", 3000},
    {"updateDelay", 500},
    {"enableLogging", true},
    {"logLevel", "INFO"},
    {"mods", json::object()}
    });

std::string GetExecutableFilename() {
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
        return std::string(path);
    }
    return {};
}

bool IsRunningOnDedicatedServer() {
    auto filename = GetExecutableFilename();
    Utils::Log(Utils::DEBUG, std::string("Running in file: " + filename).c_str());
    bool onServer = (filename.find("_server.exe") != std::string::npos);
    Utils::Log(Utils::DEBUG, std::string("Running on server: " + std::to_string(onServer)).c_str());
    return onServer;
}

// Thread for continuous DLL checking and mod updates
void ThreadLoop() {
    Utils::Log(Utils::INFO, "Thread started.");

    if (!Config::readFile()) {
        Utils::Log(Utils::WARN, "Config file not found, loading default configuration.");
        Config::setConfigFromJSON(defaultConfig);
        Config::writeFile();
    }
    Utils::Log(Utils::INFO, "Configuration loaded.");

    int bootDelay = Config::get<int>("bootDelay", 1000);
    Utils::Log(Utils::INFO, std::string("Boot delay set to: ").append(std::to_string(bootDelay)).append(" ms").c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(bootDelay));

    while (true) {
        Config::reloadIfChanged();
        if (Config::get<bool>("active", false)) {
            CheckAndLoadDlls();
            UpdateMods();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::get<int>("updateDelay", 500)));
    }
}

void ModLog(const char* msg) {
    Utils::Log(Utils::INFO, std::string("(").append(activeModName).append(") ").append(msg).c_str());
}

// Mod Context Wrapper functions
void ModContext_Log(const char* msg) {
    ModLog(msg);
}

bool ModContext_WriteToReadOnlyMemory(LPVOID targetAddress, LPVOID data, SIZE_T size) {
    try {
        Mem::MemoryProtector protector(targetAddress, size);
        if (!protector.changeProtection(PAGE_EXECUTE_READWRITE)) {
            ModContext_Log("Failed to change protection");
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
        ModContext_Log("Failed to write to read-only memory");
        return false;
    }
}

uintptr_t ModContext_FindPattern(const char* pattern, const char* mask, uintptr_t start, size_t length) {
    try {
        return Mem::FindPattern(pattern, mask, start, length);
    }
    catch (const std::exception& ex) {
        ModContext_Log("Error finding pattern");
    }
    return 0;
}

bool ModContext_Write(uintptr_t address, const void* buffer, size_t size) {
    try {
        return Mem::Write(address, buffer, size);
    }
    catch (const std::exception& ex) {
        ModContext_Log("Error during write");
        return false;
    }
}

bool ModContext_Read(uintptr_t address, void* buffer, size_t size) {
    try {
        return Mem::Read(address, buffer, size);
    }
    catch (const std::exception& ex) {
        ModContext_Log("Error during read");
        return false;
    }
}

void ModContext_WriteJump(LPVOID target, LPVOID destination) {
    try {
        Mem::WriteJump(target, destination);
    }
    catch (const std::exception& ex) {
        ModContext_Log("Failed to write jump");
    }
}

LPVOID ModContext_AllocateMemoryNearAddress(LPVOID address, SIZE_T dwSize) {
    try {
        return Mem::AllocateMemoryNearAddress(address, dwSize);
    }
    catch (const std::exception& ex) {
        ModContext_Log("Memory allocation error");
        return nullptr;
    }
}

uintptr_t ModContext_GetModuleBaseAddress(const std::string& moduleName) {
    try {
        return Mem::GetModuleBaseAddress(moduleName);
    }
    catch (const std::exception& ex) {
        ModContext_Log("Error retrieving module base address");
        return 0;
    }
}

bool ModContext_ApplyPatch(uintptr_t address, uint8_t* opcode, size_t opcodeSize) {
    try {
        Mem::Patch patch(address, opcode, opcodeSize);
        return patch.apply();
    }
    catch (const std::exception& ex) {
        ModContext_Log("Failed to apply patch");
        return false;
    }
}

bool ModContext_InjectShellcode(uint8_t* opcode, size_t opcodeSize, uintptr_t nearAddr = 0, uintptr_t* injectedAt = nullptr) {
    Mem::Shellcode shellcode(opcode, opcodeSize, nearAddr);
    if (injectedAt) *injectedAt = shellcode.data->address;
    if (shellcode.inject()) {
        ModLog(std::string("Successfully injected shellcode at address: ").append(std::to_string(shellcode.data->address)).c_str());
    }
    else {
        ModLog("Failed to inject shellcode");
    }
}

bool ModContext_CreateDetour(LPVOID targetFunction, LPVOID detourFunction, void** trampoline = nullptr) {
    //Mem::Detour detour((uintptr_t)targetFunction, detourFunction, true);
    //if (!detour.activate()) {
    //    ModLog("Failed to install detour");
    //    return false;
    //}
    //if (trampoline) *trampoline = detour.originalFunction;
    //ModLog("Successfully installed detour");
    return true;
}

// Entry point for the DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (!modContext) {
        Utils::Log(Utils::INFO, "Initializing ModContext.");
        bool onServer = IsRunningOnDedicatedServer();
        modContext = new ModContext({
            { SHROUDTOPIA_VERSION, SHROUDTOPIA_CONFIG_FILE, SHROUDTOPIA_LOG_FILE, SHROUDTOPIA_MOD_FOLDER },
            { "SVN HERE", !onServer, onServer },
            {
                [](const char* modKey, const char* key, bool defaultValue = false) { return Config::modGet<bool>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, std::vector<bool> defaultValue) { return Config::modGet<std::vector<bool>>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, std::string defaultValue = "") { return Config::modGet<std::string>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, std::vector<std::string> defaultValue = {}) { return Config::modGet<std::vector<std::string>>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, int defaultValue = 0) { return Config::modGet<int>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, std::vector<int> defaultValue = {}) { return Config::modGet<std::vector<int>>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, float defaultValue = 0.0f) { return Config::modGet<float>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, std::vector<float> defaultValue = {}) { return Config::modGet<std::vector<float>>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, uint64_t defaultValue = 0) { return Config::modGet<uint64_t>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, std::vector<uint64_t> defaultValue = {}) { return Config::modGet<std::vector<uint64_t>>(modKey, key, defaultValue); }
            },
            {
                ModContext_WriteToReadOnlyMemory,
                ModContext_FindPattern,
                ModContext_Write,
                ModContext_Read,
                ModContext_WriteJump,
                ModContext_AllocateMemoryNearAddress,
                ModContext_GetModuleBaseAddress,
                ModContext_ApplyPatch,
                ModContext_InjectShellcode,
                ModContext_CreateDetour
            },
            ModContext_Log
            });
    }

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        Utils::Log(Utils::INFO, "DLL process attach.");
        std::thread(ThreadLoop).detach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Utils::Log(Utils::INFO, "DLL process detach.");
        break;
    }
    return TRUE;
}
