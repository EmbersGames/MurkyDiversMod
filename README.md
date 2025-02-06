# Murky Divers Modkit

This repository provides an Unreal project for creating mods for Murky Divers, as well as a Python script to manage various steps such as setup and building the mod.

## Introduction to the Murky Divers Mod System

Murky Divers uses a flexible plugin-based architecture built on Unreal Engine 5.3, allowing you to extend and modify game features without altering the core codebase.

### How Mods Work

Each mod is packaged into a single `.pak` file containing various Unreal assets—such as Blueprints, textures, meshes, and more. **It is not possible to add any C++ code to mods.** The game recognizes these `.pak` files when they are placed in the `Mods` folder of your Murky Divers installation. On most systems, this path will be:

> `C:\Program Files (x86)\Steam\steamapps\common\Murky Divers\MurkyDivers\Mods`

Alongside each `.pak` file, there must be a corresponding `.umod` file of the same name (e.g. `[MyModName].pak` and `[MyModName].umod`). This `.umod` file contains metadata about the mod, such as its version and creator.

Additionally, the creation of these `.pak` and `.umod` files is facilitated by Python scripts provided in this repository. 

### Project Organization

All content in this Unreal project serves as a proxy to the Murky Divers game. The mod itself resides entirely within a plugin, which ultimately gets packaged into a .pak file. The proxy content is only used to "fake" the presence of real game assets; references made to these proxies will point to the actual Murky Divers assets once the mod is loaded in the game. Only the most critical features currently have proxies, but more will be added over time.

For this reason, it is not possible to test the mod directly in the Unreal Editor.

### Murky Divers Proxy Blueprints
```
MurkyDiversMod/Content/
```

### Your Mod Content
```
MurkyDiversMod/Plugins/[YourModName]/Content/
```

### Next step

To begin developing your mod for Murky Divers, follow the mandatory setup instructions provided in our [Getting Started](https://github.com/EmbersGames/MurkyDiversMod/wiki) page. These steps cover everything you need—from installing prerequisites to configuring your development environment—ensuring you’re ready to create and package your mod successfully.  


For more information about the game or discuss about mods creation, join our [discord](https://discord.gg/SE7rMaPcFs)
