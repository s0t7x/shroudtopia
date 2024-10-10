#include "pch.h"
#include "mem.h"
#include <thread>
#include <sstream>
#include <iomanip>
#include <chrono>

#include "utils.h"
#include "shroudtopia.h"

#include <filesystem>

json Config::jConfig;
std::filesystem::file_time_type Config::lastModifiedTime;

namespace fs = std::filesystem;

// Get the last modification time of a file
std::chrono::system_clock::time_point GetLastWriteTime(const std::string& filePath) {
    auto ftime = fs::last_write_time(filePath);
    auto system_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return system_time;
}

typedef Mod* (*CreateModFunc)();

ModContext* modContext;

struct DllInfo {
    HMODULE hModule; // Handle to the loaded DLL
    std::chrono::system_clock::time_point lastModified; // Last modification time
};

// Unload a mod by freeing the DLL
void UnloadDll(DllInfo& modInfo) {
    if (modInfo.hModule) {
        FreeLibrary(modInfo.hModule);
    }
}

void LoadAndRegisterMod(const char* dllPath) {
    HMODULE hMod = LoadLibraryA(dllPath);
    if (hMod) {
        CreateModFunc createMod = (CreateModFunc)GetProcAddress(hMod, "CreateModInstance");
        if (createMod) {
            Mod* modInstance = createMod();
            if (modInstance) {
                modContext->registeredMods[std::string(dllPath)] = modInstance;
                Utils::Log(Utils::DEBUG, std::string().append(modInstance->GetMetaData().name).append(" registered.").c_str());
            }
        }
    }
}

std::map<std::string, DllInfo> loadedDLLs;

void CheckAndReloadDlls(const std::string& modFolder = MOD_FOLDER) {
    // Iterate over .dll files in the mod folder
    for (const auto& entry : fs::directory_iterator(modFolder)) {
        if (entry.path().extension() == ".dll") {
            std::string dllPath = entry.path().string();
            auto lastWriteTime = GetLastWriteTime(dllPath);

            // Check if this mod is already loaded
            auto it = loadedDLLs.find(dllPath);
            if (it != loadedDLLs.end()) {
                // If the mod is already loaded, check if it has been updated
                // // BROKEN
                //if (it->second.lastModified < lastWriteTime) {
                //   Utils::Log(Utils::DEBUG, std::string("Reloading updated mod: ".append(dllPath.append(std::endl;

                //    // Unload the previous version
                //    auto regIt = modContext->registeredMods.find(dllPath);
                //    if (regIt != modContext->registeredMods.end())
                //        if (regIt->second->loaded)
                //            regIt->second->Unload(modContext);

                //    UnloadDll(it->second);

                //    // Load the new version
                //    LoadAndRegisterMod(dllPath.c_str());

                //    // Update the mod's last modified time
                //    it->second.lastModified = lastWriteTime;
                //}
            }
            else {
                // If the mod is not loaded yet, load it
                Utils::Log(Utils::DEBUG, std::string("Loading new mod: ").append(dllPath).c_str());

                // Load and register the new mod
                LoadAndRegisterMod(dllPath.c_str());

                // Track the mod in the map
                loadedDLLs[dllPath] = DllInfo{ LoadLibraryA(dllPath.c_str()), lastWriteTime };
            }
        }
    }

    // Check if any mods were removed
    for (auto it = loadedDLLs.begin(); it != loadedDLLs.end(); ) {
        if (!fs::exists(it->first)) {
           Utils::Log(Utils::DEBUG, std::string("Unloading removed mod: ").append(it->first).c_str());

            // Unload the mod
            auto regIt = modContext->registeredMods.find(it->first);
            if (regIt != modContext->registeredMods.end())
                if (regIt->second->loaded)
                    regIt->second->Unload(modContext);
            
            UnloadDll(it->second);

            // Remove it from the map
            it = loadedDLLs.erase(it);
        }
        else {
            ++it;
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
    char path[MAX_PATH];  // Buffer to hold the path

    // Get the full path of the executable that loaded this DLL
    if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
        return std::string(path);  // Convert the path to a std::string and return it
    }
    else {
        return std::string();  // Return an empty string if there was an error
    }
}

bool IsRunningOnDedicatedServer() {
    auto filename = GetExecutableFilename();
    return (filename.find("_server.exe") >= 0);
}

void UpdateMods() {
    // Load unloaded registered mods
    for (auto const& [dllPath, regMod] : modContext->registeredMods) {
        if (regMod->loaded) continue;
        if (modContext->loadedMods.find(regMod) == modContext->loadedMods.end()) {
            ModMetaData modMeta = regMod->GetMetaData();
            if (modContext->game.isServer) {
                if (!modMeta.hasServerSupport) continue;
            }
            else {
                if (!modMeta.hasClientSupport) continue;
            }
            regMod->Load(modContext);
            regMod->loaded = true;
            modContext->loadedMods.insert(regMod);
        }
    }

    // Update loaded mods
    for (auto* mod : modContext->loadedMods) {
        ModMetaData meta = mod->GetMetaData();
        // Update active value from config
        bool shouldModBeActive = Config::modGet<bool>(meta.name.c_str(), "active", false);
        if (shouldModBeActive) {
            if (!mod->active) {
                mod->Activate(modContext);
            }
            mod->Update(modContext);
        }
        else {
            if (mod->active) {
                mod->Deactivate(modContext);
            }
        }
    }
}

void ThreadLoop()
{
    Utils::Log(Utils::INFO, "Thread started.");

    if (!Config::readFile()) {
        Config::setConfigFromJSON(defaultConfig);
        Config::writeFile();
    }
    Utils::Log(Utils::INFO, "Config loaded.");

    try {
        int bootDelay = Config::get<int>("bootDelay", 1000);

        Utils::Log(Utils::INFO, std::string("Wait before injection. Configured boot delay is ").append(std::to_string(bootDelay)).append("ms.").c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(bootDelay));
    }
    catch (const std::exception& e) {
        Utils::Log(Utils::INFO, std::string("Exception occurred: ").append(e.what()).c_str());
    }
    catch (...) {
        Utils::Log(Utils::INFO, "An unknown error occurred.");
    }

    while (true)
    {
        Config::reloadIfChanged();
        if (Config::get<bool>("active", false)) {
            CheckAndReloadDlls();
            UpdateMods();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::get<int>("updateDelay", 500)));
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    if (!modContext) {
        bool onServer = IsRunningOnDedicatedServer();
        modContext = new ModContext({
            {
                VERSION,
                CONFIG_FILE,
                LOG_FILE,
                MOD_FOLDER,
            },
            {
                "SVN HERE",
                !onServer,
                onServer
            },
            {
                [](const char* modKey, const char* key, bool defaultValue = false) { return Config::modGet<bool>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, const std::string& defaultValue = "") { return Config::modGet<std::string>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, int defaultValue = 0) { return Config::modGet<int>(modKey, key, defaultValue); },
                [](const char* modKey, const char* key, float defaultValue = 0.0f) { return Config::modGet<float>(modKey, key, defaultValue); }
            },
            [=](std::string msg) { Utils::Log(Utils::INFO, msg.c_str()); }
        });
    }
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::thread(ThreadLoop).detach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
