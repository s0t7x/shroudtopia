#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>

class Mod;

struct ModMetaData {
    std::string name;
    std::string description;
    std::string version;
    std::string author;
    std::string targetShroudtopiaVersion;
    bool hasClientSupport;
    bool hasServerSupport;
};

struct ModContext {
    struct {
        std::string version;
        std::string config_file;
        std::string log_file;
        std::string mod_folder;
    } shroudtopia;
    struct {
        std::string version;
        bool isClient;
        bool isServer;
    } game;
    struct {
        std::function<bool(const char* modKey, const char * key, bool defaultValue)> GetBool;
        std::function<std::string(const char* modKey, const char* key, std::string defaultValue)> GetString;
        std::function<int(const char* modKey, const char* key, int defaultValue)> GetInt;
        std::function<float(const char* modKey, const char* key, float defaultValue)> GetFloat;
    } config;
    struct {
        std::function<bool(LPVOID targetAddress, LPVOID data, SIZE_T size)> WriteToReadOnlyMemory;
        std::function<uintptr_t(const char* pattern, const char* mask, uintptr_t start, size_t length)> FindPattern;
        std::function<bool(uintptr_t address, const void* buffer, size_t size)> Write;
        std::function<bool(uintptr_t address, void* buffer, size_t size)> Read;
        std::function<void(LPVOID target, LPVOID destination)> WriteJump;
        std::function<LPVOID(LPVOID address, SIZE_T dwSize)> AllocateMemoryNearAddress;
        std::function<uintptr_t(const std::string& moduleName)> GetModuleBaseAddress;
        std::function<bool(uintptr_t address, uint8_t* opcode, size_t opcodeSize)> ApplyPatch;
        std::function<bool(uint8_t* opcode, size_t opcodeSize, uintptr_t nearAddr, uintptr_t* injectedAt)> InjectShellcode;
        std::function<bool(LPVOID targetFunction, LPVOID detourFunction, void** trampoline)> CreateDetour;
    } memory;


    std::function<void(const char *)> Log;

    std::unordered_map<std::string, Mod*> registeredMods;
    std::unordered_set<Mod*> loadedMods;
};

class Mod {
public:
    Mod(): active(false), loaded(false) {};

    bool loaded;
    bool active;

    virtual ~Mod() {}  // Add a virtual destructor to enable polymorphism

    virtual ModMetaData GetMetaData() = 0;

    virtual void Load(ModContext *modContext) = 0;
    virtual void Unload(ModContext* modContext) = 0;

    virtual void Activate(ModContext* modContext) = 0;
    virtual void Deactivate(ModContext* modContext) = 0;

    virtual void Update(ModContext* modContext) = 0;
};