# Known issues

## Main menu

- When loading a game and then going back to the main menu, it's not possible to open a new file. The game must be restarted

## Locations

- NPC/Quests/Bosses still give the vanilla item when completing the quest
- The first emote statue will not check the location. This is because I'm currently hooking the payment dialog. This will be fixed by hooking the blue eyes instead

## Archipelago

- The locations checked are not saved in a file dynamically. This is required by the Archipelago spec to make sure that no locations are lost during a disconnection