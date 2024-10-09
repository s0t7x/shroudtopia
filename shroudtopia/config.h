#pragma once
#include <fstream>
#include <filesystem>
#include "pch.h"

#define CONFIG_FILE "shroudtopia.json"

using json = nlohmann::json;

class Config
{
public:
    Config() {
        try {
            lastModifiedTime = std::filesystem::last_write_time(CONFIG_FILE);
        } catch (std::exception err) { }
    }

    bool active = true;
    int boot_delay = 3000;
    int update_delay = 1000;

    bool glider_flight = false;
    bool no_stamina_loss = false;
    bool no_fall_damage = false;
    bool no_craft_cost = false;
    bool inf_item_use = false;
    bool bypass_world_borders = false;
    bool bypass_altar_limit = false;

    bool read()
    {
        try {
            std::ifstream configFile(CONFIG_FILE);
            if (configFile.is_open())
            {
                json jConfig;
                configFile >> jConfig;

                active = jConfig.value("active", false);
                boot_delay = jConfig.value("boot_delay", 3000);
                update_delay = jConfig.value("update_delay", 1000);
                glider_flight = jConfig.value("glider_flight", false);
                no_stamina_loss = jConfig.value("no_stamina_loss", false);
                no_fall_damage = jConfig.value("no_fall_damage", false);
                no_craft_cost = jConfig.value("no_craft_cost", false);
                inf_item_use = jConfig.value("infinite_item_use", false);
                bypass_world_borders = jConfig.value("bypass_world_borders", false);
                bypass_altar_limit = jConfig.value("bypass_altar_limit", false);

                lastModifiedTime = std::filesystem::last_write_time(CONFIG_FILE);
                return true;
            }
            return false;
        }
        catch (std::exception err) {
            return false;
        }
    }

    std::string dump()
    {
        json jConfig;
        jConfig["active"] = active;
        jConfig["boot_delay"] = boot_delay;
        jConfig["update_delay"] = update_delay;
        jConfig["glider_flight"] = glider_flight;
        jConfig["no_fall_damage"] = no_fall_damage;
        jConfig["no_stamina_loss"] = no_stamina_loss;
        jConfig["no_craft_cost"] = no_craft_cost;
        jConfig["infinite_item_use"] = inf_item_use;
        jConfig["bypass_world_borders"] = bypass_world_borders;
        jConfig["bypass_altar_limit"] = bypass_altar_limit;

        return jConfig.dump(4);
    }

    bool write()
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


    // Check if the configuration file has changed
    bool hasFileChanged()
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
    bool reloadIfChanged()
    {
        if (hasFileChanged())
        {
            return read(); // Reload the config if changed
        }
        return false; // No changes detected
    }

private:
    std::filesystem::file_time_type lastModifiedTime; // To track last modified time
};