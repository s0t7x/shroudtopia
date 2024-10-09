
<h1><p align="center">
Shroudtopia
</p></h1>
<p align="center">
<b>Creative Mode Mod for Enshrouded Dedicated Servers</b>
</p>
<p align="center">
<img alt="Static Badge" src="https://img.shields.io/badge/Game%20Version%20(SVN)-558123-blue">
</p>

I'm excited to introduce **Shroudtopia**, a mod that brings creative mode functionalities to Enshrouded dedicated servers. Whether you're a builder looking for unlimited resources or an explorer seeking freedom from limitations, Shroudtopia enhances your gameplay experience.

## Features

- **Fly:** Enjoy full flight capabilities with the glider. No more losing height!
- **Infinite Altars:** Bypass the general altar limit and make the world yours.
- ~~**Custom Experience Multiplier:** Adjust experience gain to your preference.~~ *(Obsolete since official gameSettings implementation)*
- **No Fall Damage:** Explore without the fear of taking fall damage.
- **No Stamina Loss:** Infinite stamina for uninterrupted gameplay.
- ~~**No Durability Loss:** Gear and items never degrade or break.~~ *(Obsolete since official gameSettings implementation)*
- ~~**Item Duplication:** Splitting stacks results in cloned items.~~ *(Broken)*
- **Free Crafting:** Resources are required but crafting has no cost.
- **Infinite Item Use & No Building Cost:** Resources are required but using and building has no cost.

## Installation

1. **Download:** Get latest binaries from [release section](https://github.com/s0t7x/shroudtopia/releases).
2. **Setup:** Extract the files into your Enshrouded dedicated server folder.
3. **Launch:** Start the server - a default config is generated if `shroudtopia.json` is absent.

If Shroudtopia is loaded you should see something like this in the server console:
```
[shroudtopia] Shroudtopia initialized. Starting thread...
[shroudtopia] Thread started.
[shroudtopia] Loading config from 'shroudtopia.json'...
[shroudtopia] Config loaded.
[shroudtopia] Wait for server. Configured boot delay is 1000ms.
```

Upon first launch the file ```shroudtopia.json``` is created. By default all mods are deactivated and you have to manually alter the configuration to your likings.
The file can be changed while the server is running and will be reloaded on-the-fly.

## Customization

Each aspect of Shroudtopia is customizable via the `shroudtopia.json` config file. Adjust settings to tailor the mod to your preferred gameplay style:

```json
{
    "active": true,
    "boot_delay": 3000,
    "bypass_altar_limit": true,
    "glider_flight": true,
    "infinite_item_use": true,
    "no_craft_cost": true,
    "no_fall_damage": true,
    "no_stamina_loss": true
}
```

## Contributing
Contributions are welcome! Feel free to fork the repository, make improvements, and submit pull requests.

## License
This project is licensed under the [MIT License](https://github.com/s0t7x/shroudtopia/blob/0.1-stable/LICENSE).

<hr />

Let's make Enshrouded even more exciting with **Shroudtopia**! 🌟

Special thanks to the following folks. Their work gave me a headstart implementing most features:

- cfe
- Turk
- Atamg
