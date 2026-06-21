# Blue Fire Archipelago Mod

A Blue Fire mod that randomizes Blue Fire through [Archipelago](https://archipelago.gg/)

This mod randomizes the items that are obtained when :
- Completing a void gate
- Buying a shop item
- Picking up an item from the ground
- Opening a chest
- Purchasing an emote at an emote statue

The obtains that can be obtained are :
- Abilities
- Ores
- Key items (Beira vessel, Sanctuary Stone, Keys ...)
- Spirits
- Emotes
- Weapons

The game is completed when the queen is slayed.


# Getting Started


## Adding the Archipelago Mod to Blue Fire

1. Download the latest release of the mod and its dependencies from the [Archipelago-Bluefire Github Releases Page](https://github.com/Nebulea-dev/Blue-Fire-Archipelago)
2. Go to the Blue Fire installation folder. You can access it on Steam by right-clicking the game and selecting `Manage > Browse local files`
3. Move the content of the archive inside the Blue Fire installation folder. You should now have a `ue4ss` folder at `<installation path>\Blue Fire\Binaries\Win64\`


## Installing the world and generating a YAML
1. Install [Archipelago](https://archipelago.gg/tutorial/Archipelago/setup/en)
2. Download the latest .apworld file from the [Archipelago-Bluefire Github Releases Page](https://github.com/Nebulea-dev/Archipelago-BlueFire/releases)
3. Close the Archipelago Launcher if it's currently opened, and install the .apworld by double clicking it
4. Open the Archipelago Launcher, select the `Generate Template Options`. This will generate a YAML template at `C:\ProgramData\Archipelago\Players\Templates\Blue Fire.yaml`
5. Copy this YAML in `C:\ProgramData\Archipelago\Players\`, and customize the options inside the file as you wish
6. Generate the game by selecting `Generate` in the Archipelago Launcher

You can then host the game on any Archipelago server you want.
The easiest way would be to follow the [Hosting an Archipelago server](https://archipelago.gg/tutorial/Archipelago/setup_en#hosting-an-archipelago-server) guide on archipelago.gg.


## Joining an Archipelago Server

1. Create a new Blue Fire save file
2. Type in the login details of the Archipelago server. You can move around using ENTER/TAB to move down, ESC to unfocus the text boxes, and directional arrows
3. You can see the connection status in the UE4SS console


# Reporting an issue / Suggesting a feature

This is a very new Archipelago mod. It WILL have issues, bugs, broken logic, etc ...

For example, when completing a location, it's possible that this location was not mapped out yet. In this case, an error will be printed out in the UE4SS console. Whenever an issue like this happens, you can see a more detailed flow of what happened in the UE4SS console. Please add a screenshot of this console when creating the issue

The steps to report an issue or suggest a new feature are :
1. Check if the issue/feature is not already present in the [known issue page](./Known_issues.md) / [planned features page](./Planned_features.md)
2. Check if the issue/feature has not already been asked in the [Github issue page](https://github.com/Nebulea-dev/Blue-Fire-Archipelago/issues)
3. If you cannot find your issue/feature in the 2 previous steps, create a new issue in the [Github issue page](https://github.com/Nebulea-dev/Blue-Fire-Archipelago/issues) with the appropriate labels
