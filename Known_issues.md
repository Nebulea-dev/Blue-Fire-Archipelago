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

## Archipelago

- The locations checked are not saved in a file dynamically. This is required by the Archipelago spec to make sure that no locations are lost during a disconnection

## General

- Closing the game though the exit game option causes a UE crash because not all object listeners are unregistered. Very low priority
