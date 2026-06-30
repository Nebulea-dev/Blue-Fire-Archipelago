# Known issues

## Main menu

- When loading a game and then going back to the main menu, it's not possible to open a new file. The game must be restarted
- Very rarely, the game can crash in the menu. The game must be restarted. No easy way to reproduce so far
- Very rarely, "FILE" line is not removed from the menu. The game must be restarted. No easy way to reproduce so far
- Very rarely, the completion percentage of the Archipelago save file is way too high. No easy way to reproduce so far

## Locations

- NPC/Quests/Bosses still give the vanilla item when completing the quest
- Something seems to be shifting the index of the locations by 1 somewhere in StoneHeart City, creating missmatches between the name of the location and it's actual in-game location
- All chests inside void gates have the same name, and map to only one location. They need to be differentiated
- The Ancient Relic pickup at the entrance of Stoneheart City is not properly removed from the inventory of the player after pickup. It still correctly gives the location though

## Archipelago

- The locations checked are not saved in a file dynamically. This is required by the Archipelago spec to make sure that no locations are lost during a disconnection
- The conditions to access the chest in Water Ways is incorrect, as it should require all 4 doors to be unlocked (currently requires 1 door)
- The AP World gives Rare keys instead of Holy Keys
- The AP World gives useless keys that were removed from the game (ex: Key Ice Master)
- The name of many locations are a bit obscure (ex: "Loot Chest 3" instead of a useful name like "Left Side Chest")
- The name of Spirits are missmatched with the actual spirit in-game

## General

- Closing the game though the exit game option causes a UE crash because not all object listeners are unregistered. Very low priority
