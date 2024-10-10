#pragma once
#include <fstream>
#include <filesystem>
#include "pch.h"
#include "defines.h"

#include "nlohmannjson.hpp"

using json = nlohmann::json;

class Config
{
    Config() {}
    static json jConfig;
public:
    inline static bool readFile()
    {
        try {
            std::ifstream configFile(CONFIG_FILE);
            if (configFile.is_open())
            {
                configFile >> jConfig;
                lastModifiedTime = std::filesystem::last_write_time(CONFIG_FILE);
                return true;
            }
            return false;
        }
        catch (std::exception err) {
            return false;
        }
    }

    template <typename t>
    inline static t get(const char* key, t defaultValue = t) {
        try {
            return jConfig[key].get<t>();
        }
        catch (...) {
            return defaultValue;
        }
    }

    template <typename t>
    inline static t modGet(const char* modKey, const char* key, t defaultValue = t) {
        try {
            return jConfig["mods"][modKey][key].get<t>();
        }
        catch (...) {
            return defaultValue;
        }
    }

    template <typename t>
    inline static void set(const char* key, t value) {
        try {
            jConfig[key] = value;
        }
        catch (...) {
        }
    }

    template <typename t>
    inline static t modSet(const char* modKey, const char* key, t value) {
        try {
            jConfig["mods"][modKey][key] = value;
        }
        catch (...) {
        }
    }

    inline static std::string dump()
    {
        return jConfig.dump(4);
    }

    inline static bool writeFile()
    {
        std::ofstream configFile(CONFIG_FILE);
        if (configFile.is_open())
        {
            // Write pretty-printed JSON to file
            configFile << dump();
            configFile.close();
            return true;
        }
        return false;
    }

    inline static void setConfigFromJSON(json config) {
        jConfig = config;
    }

    // Check if the configuration file has changed
    inline static bool hasFileChanged()
    {
        try {
            auto currentModifiedTime = std::filesystem::last_write_time(CONFIG_FILE);
            return currentModifiedTime != lastModifiedTime;
        }
        catch (std::exception err) {
            return false;
        }
    }

    // Reload configuration if the file has changed
    inline static bool reloadIfChanged()
    {
        if (hasFileChanged())
        {
            return readFile(); // Reload the config if changed
        }
        return false; // No changes detected
    }

private:
    static std::filesystem::file_time_type lastModifiedTime; // To track last modified time
};