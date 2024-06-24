<center>

<h1>Shroudtopia</h1>
<b>Creative Mode Mod for Enshrouded Dedicated Servers</b>

</center>
<hr/>

I'm excited to introduce **Shroudtopia**, a mod that brings creative mode functionalities to Enshrouded dedicated servers. Whether you're a builder looking for unlimited resources or an explorer seeking freedom from limitations, Shroudtopia enhances your gameplay experience.

## Features

- **Custom Experience Multiplier:** Adjust experience gain to your preference.
- **Fly Mode with Glider:** Enjoy full flight capabilities with the glider.
- **No Fall Damage:** Explore without the fear of taking fall damage.
- **No Stamina Loss:** Infinite stamina for uninterrupted gameplay.
- **No Durability Loss:** Gear and items never degrade or break.
- **Item Duplication:** Splitting stacks results in cloned items.
- **Free Crafting & Building:** Resources are required but crafting and building have no resource cost.

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

## Customization

Each aspect of Shroudtopia is customizable via the `shroudtopia.json` config file. Adjust settings to tailor the mod to your preferred gameplay style:

```json
{
    "clone_item_splits": true,
    "exp_multiplier": 5,
    "free_craft": true,
    "glider_flight": true,
    "no_durability_loss": true,
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