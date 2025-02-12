<p align="center">
<img alt="Shroudtopia" src="https://github.com/s0t7x/shroudtopia/blob/main/header.gif">
</p>
<p align="center">
<b>Modloader for Enshrouded (Server & Client)</b>
</p>
<p align="center">
<img alt="Static Badge" src="https://img.shields.io/badge/Game%20Version%20(SVN)-637515-blue">
</p>

I'm excited to introduce **Shroudtopia**, a modloader that allows easy management and integration of mods for Enshrouded. With Shroudtopia, you can dynamically load, activate, and deactivate mods without restarting, giving you the ultimate flexibility to enhance gameplay.

## Table of Contents
- [Modloader Features](#modloader-features)
- [Installation](#installation)
- [Configuration](#configuration)
- [Example Mods](#example-mods)
- [Creating Mods](#creating-mods)
- [Contributing](#contributing)
- [License](#license)

## Modloader Features

- **Mod Management**: Dynamically load and unload mods from the "mods" folder.
- **Live Configuration**: Modify mod settings at runtime without restarting the server.
- **Dependency Injection**: Each mod is fully integrated into the system via the `ModContext`, enabling shared access to configuration, logging, and other utilities.

## Installation

| **Requirements:**                                             |
|---------------------------------------------------------------|
| Microsoft Visual C++ Redistributable ~ [x64 Download](https://aka.ms/vs/17/release/vc_redist.x64.exe). |

### Steps to Install:
1. **Download the Mod Loader:** Get the latest Shroudtopia binaries from the [release section](https://github.com/s0t7x/shroudtopia/releases).
2. **Extract the Files:** Extract the contents into your Enshrouded game or dedicated server folder.
3. **Download Example Mods:** You can download example mods from the [release section](https://github.com/s0t7x/shroudtopia/releases).
4. **Create Mods Folder:** If it doesn’t already exist, create a `mods` folder and place your mod DLLs inside.
5. **Launch the Server:** Start the server. A default config (`shroudtopia.json`) will be generated if it's absent.

Once set up, you're ready to start using Shroudtopia to manage your mods.

On servers: If Shroudtopia is loaded correctly, you should see something like this in the server console:
```
[shroudtopia][INFO] Config loaded.
[shroudtopia][INFO] Wait before injection. Configured boot delay is 3000ms.
```

Upon the first launch, a default configuration file `shroudtopia.json` is created. All mods are deactivated by default, so you must manually activate them by adjusting the configuration.

## Configuration

Each mod can be enabled or customized via the `shroudtopia.json` config file. Here’s an example configuration:

```json
{
    "active": true,
    "bootDelay": 3000,
    "enableLogging": true,
    "logLevel": "INFO",
    "mods": {
        "basics": {
            "active": true,
            "no_stamina_loss": true,
            "no_fall_damage": true,
            "no_craft_cost": true,
            "inf_item_use": true,
            "bypass_altar_limit": true
        },
        "Flight Mod": {
            "active": true
        }
    },
    "updateDelay": 500
}
```

Each mod exposes its configuration options, which can be altered in `shroudtopia.json`. For instance, to enable or disable specific features for the basics mod:
```json
"basics": {
    "active": true,
    "no_stamina_loss": true,
    "no_fall_damage": false,
    "no_craft_cost": true,
    "inf_item_use": true,
    "bypass_altar_limit": true
}
```

## Example Mods

- **Flight Mod:** Enjoy full flight capabilities with the glider. No more losing height!
  
![Flight Mod](https://github.com/s0t7x/shroudtopia/blob/main/example-mods/flight_mod/demo.gif)


- **First Person View:** Play Enshrouded from another persepctive. Example for client-only mod.
  
![First Person View](https://github.com/s0t7x/shroudtopia/blob/main/example-mods/first_person_view/demo.gif)


- **BasicsMod:** Flight Mod is no fun with fall damage. All other legacy Shroudtopia features have been put into this mod. You can selectively activate them in the configuration file.


# Creating Mods
Mods for Shroudtopia are written as dynamic libraries (DLLs) and need to include `shroudtopia.h` from `./shroudtopia/`.
They must implement the `Mod` interface and provide the factory function `extern "C" __declspec(dllexport) Mod* CreateModInstance()`.

Here’s an example factory function:
```cpp
extern "C" __declspec(dllexport) Mod* CreateModInstance() {
    return new BasicsMod();
}
```

## Mod Interface
Every mod must implement the following functions to integrate with the modloader:
```cpp
class Mod {
public:
    virtual ~Mod() {}
    virtual ModMetaData GetMetaData() = 0;
    virtual void Load(ModContext* modContext) = 0;
    virtual void Unload(ModContext* modContext) = 0;
    virtual void Activate(ModContext* modContext) = 0;
    virtual void Deactivate(ModContext* modContext) = 0;
    virtual void Update(ModContext* modContext) = 0;
};
```

`ModMetaData` contains essential information about the mod:
```cpp
struct ModMetaData {
    std::string name;
    std::string description;
    std::string version;
    std::string author;
    std::string targetShroudtopiaVersion;
    bool hasClientSupport;
    bool hasServerSupport;
};
```

## ModContext
The `ModContext` is passed to each mod and provides access to core functionality, such as configuration, logging, and mod management. For example, the mod can retrieve config values via:
```cpp
bool enabled = modContext->config.GetBool("modName", "feature_name", false);
```


# Contributing
Currently no really game specific functions are implemented in the modContext. This is first try. Anyways, contributions are welcome! Fork the repository, add improvements, and submit pull requests. I would be happy to see more mods for this around.


# License
This project is licensed under the [MIT License](https://github.com/s0t7x/shroudtopia/blob/0.1-stable/LICENSE).

<hr />

Let's make Enshrouded even more exciting with **Shroudtopia**! 🌟

Special thanks to the following folks. Their work gave me a headstart implementing most features:

- cfe
- Turk
- Atamg
- ndoa
