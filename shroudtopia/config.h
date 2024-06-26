#pragma once
#include <fstream>
#include "pch.h"

#define CONFIG_FILE "shroudtopia.json"

using json = nlohmann::json;

class Config
{
public:
    bool active = true;
    int boot_delay = 3000;
    int exp_multiplier = 5;
    bool glider_flight = true;
    bool no_fall_damage = true;
    bool no_stamina_loss = true;
    bool no_durability_loss = true;
    bool clone_item_splits = true;
    bool free_craft = true;

    bool read()
    {
        std::ifstream configFile(CONFIG_FILE);
        if (configFile.is_open())
        {
            json jConfig;
            configFile >> jConfig;
            active = jConfig.value("active", true);
            boot_delay = jConfig.value("boot_delay", 3000);
            exp_multiplier = jConfig.value("exp_multiplier", 5);
            glider_flight = jConfig.value("glider_flight", true);
            no_fall_damage = jConfig.value("no_fall_damage", true);
            no_stamina_loss = jConfig.value("no_stamina_loss", true);
            no_durability_loss = jConfig.value("no_durability_loss", true);
            clone_item_splits = jConfig.value("clone_item_splits", true);
            free_craft = jConfig.value("free_craft", true);
            return true;
        }
        return false;
    }

    std::string dump()
    {
        json jConfig;
        jConfig["exp_multiplier"] = exp_multiplier;
        jConfig["glider_flight"] = glider_flight;
        jConfig["no_fall_damage"] = no_fall_damage;
        jConfig["no_stamina_loss"] = no_stamina_loss;
        jConfig["no_durability_loss"] = no_durability_loss;
        jConfig["clone_item_splits"] = clone_item_splits;
        jConfig["free_craft"] = free_craft;
        return jConfig.dump(4);
    }

    bool write()
    {
        std::ofstream configFile("shroudtopia.json");
        if (configFile.is_open())
        {
            // Write pretty-printed JSON to file
            configFile << dump();
            configFile.close();
            return true;
        }
        return false;
    }
};