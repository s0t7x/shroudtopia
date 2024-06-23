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

1. **Download:** Get the source code from git.
2. **Setup:** Build 'winmm' solution and drop 'winmm.dll' into your Enshrouded dedicated server folder.
3. **Launch:** Start the server - a default config is generated if `shroudtopia.json` is absent.

## Customization

Each aspect of Shroudtopia is highly customizable via the `shroudtopia.json` config file. Adjust settings to tailor the mod to your preferred gameplay style:

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
This project is licensed under the [MIT License](#).

<hr />

Let's make Enshrouded even more exciting with **Shroudtopia**! 🌟
