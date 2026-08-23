#pragma once

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <vector>

using namespace RC;
using namespace Unreal;

/*******************************************************************************
 * ItemManager
 *
 * @brief   Manages item reception and display from the Archipelago server.
 *
 *          Responsible for intercepting item display events, modifying item
 *          parameters (name, description, amount), and handling item notifications.
 *          Integrates with the Archipelago server to receive item callbacks and
 *          display items in the game UI.
 */
class ItemManager
{
	public:
	/*******************************************************************************
	 * @fn      ItemManager
	 *
	 * @brief   Constructor - initializes item manager state.
	 *
	 * @return  none
	 */
	ItemManager();

	/*******************************************************************************
	 * @fn      itemReceiveCb
	 *
	 * @brief   Called when an item is received from the Archipelago server.
	 *
	 *          This callback is invoked by the Archipelago library whenever an
	 *          item is received from another player in the multiworld session.
	 *          Used to track item reception events and trigger notifications.
	 *
	 * @param   itemID  - The Archipelago item ID being received
	 *
	 * @return  none
	 */
	void itemReceiveCb(int itemID);

	/*******************************************************************************
	 * @fn      PlayNewItemPreHook
	 *
	 * @brief   Pre-hook callback for the NewItem.PlayNewItem function.
	 *
	 *          Intercepts the item display event before it executes, allowing
	 *          modification of item display parameters (name, description, amount).
	 *          This is a static hook callback that modifies function parameters
	 *          to display Archipelago items with custom information.
	 *
	 * @param   Context     - Unreal object context
	 * @param   Stack       - Function stack frame with parameters
	 * @param   RESULT_DECL - Result declaration pointer
	 *
	 * @return  true to prevent original function execution, false to allow it
	 */
	bool PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	static bool StartNewGame(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	/*******************************************************************************
	 * @fn      givePlayerEmote
	 *
	 * @brief   Give the player an emote item.
	 *
	 * @param   emoteID - The emote item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerEmote(int emoteID);

	/*******************************************************************************
	 * @fn      givePlayerWeapon
	 *
	 * @brief   Give the player a weapon item.
	 *
	 * @param   weaponID - The weapon item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerWeapon(int weaponID);

	/*******************************************************************************
	 * @fn      givePlayerTunic
	 *
	 * @brief   Give the player a tunic item.
	 *
	 * @param   tunicID - The tunic item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerTunic(int tunicID);

	/*******************************************************************************
	 * @fn      givePlayerSpirit
	 *
	 * @brief   Give the player a spirit item.
	 *
	 * @param   spiritID - The spirit item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerSpirit(int spiritID);

	/*******************************************************************************
	 * @fn      givePlayerAbility
	 *
	 * @brief   Give the player an ability item.
	 *
	 * @param   abilityID - The ability item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerAbility(int abilityID);

	/*******************************************************************************
	 * @fn      givePlayerItem
	 *
	 * @brief   Give the player an item.
	 *
	 * @param   itemID - The item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerItem(int itemID);

	/*******************************************************************************
	 * @fn      givePlayerKeyItem
	 *
	 * @brief   Give the player a key item.
	 *
	 * @param   itemID - The item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerKeyItem(int itemID);

	/*******************************************************************************
	 * @fn      givePlayerCustomItem
	 *
	 * @brief   Give the player a custom item requiring special handling logic.
	 *
	 *          Routes to specific custom item handlers based on itemID.
	 *          This includes progressive items (Pouch, Weapon) and items like
	 *          Fire Essence Slot that need custom upgrade/progression logic.
	 *
	 * @param   itemID - The custom item ID (0-based index)
	 *
	 * @return  none
	 */
	void givePlayerCustomItem(int itemID);

	/*******************************************************************************
	 * @fn      givePlayerProgressivePouch
	 *
	 * @brief   Handle progressive pouch upgrade logic.
	 *
	 *          Checks the current pouch level in the passive inventory and upgrades
	 *          to the next level. Progression: Large Pouch -> Extra Large Pouch.
	 *          First receipt gives Large Pouch, second receipt upgrades to Extra Large Pouch.
	 *
	 * @return  none
	 */
	void givePlayerProgressivePouch();

	/*******************************************************************************
	 * @fn      givePlayerProgressiveWeapon
	 *
	 * @brief   Handle progressive weapon upgrade logic.
	 *
	 *          Checks the current highest weapon level in the weapon inventory and adds
	 *          the next weapon in the progression sequence. Looks for the highest weapon ID
	 *          currently owned and adds a new weapon with ID + 1.
	 *
	 * @return  none
	 */
	void givePlayerProgressiveWeapon();

	/*******************************************************************************
	 * @fn      givePlayerCurrency
	 *
	 * @brief   Give the player currency.
	 *
	 * @param   amount - The currency amount to add
	 *
	 * @return  none
	 */
	void givePlayerCurrency(int32_t amount);
};

struct inventoryItem
{
	uint32_t item;
	uint32_t amount;
	uint32_t resets;
	uint32_t originalAmount;
	uint8_t type;
	uint8_t tunic;
	uint8_t weapon;
	uint8_t spirit;
	uint32_t price;
	uint8_t ability;
};

/*******************************************************************************
 * ItemQueue
 *
 * @brief   Manages persistence of items received when the game is not loaded.
 *
 *          Stores pending items to a JSON queue file next to the DLL, allowing
 *          items received while the game is closed to be applied when the game
 *          restarts.
 */
class ItemQueue
{
	public:
	/*******************************************************************************
	 * @fn      getQueueFilePath
	 *
	 * @brief   Gets the full path to the item queue JSON file.
	 *
	 * @return  String path to item_queue.json in the DLL directory
	 */
	static std::string getQueueFilePath();

	/*******************************************************************************
	 * @fn      queueFileExists
	 *
	 * @brief   Checks if the item queue file exists on disk.
	 *
	 * @return  true if queue file exists, false otherwise
	 */
	static bool queueFileExists();

	/*******************************************************************************
	 * @fn      saveItemToQueue
	 *
	 * @brief   Appends an item to the persistent queue file.
	 *
	 *          Reads the existing queue file (if it exists), appends the new item ID,
	 *          and writes it back. Safe to call multiple times - each call appends
	 *          another item to the queue.
	 *
	 * @param   itemID - The Archipelago item ID to queue
	 *
	 * @return  none
	 */
	static void saveItemToQueue(int itemID);

	/*******************************************************************************
	 * @fn      loadAndClearQueue
	 *
	 * @brief   Loads all pending items from queue file and deletes the file.
	 *
	 *          Reads the queue file, returns all queued item IDs as a vector,
	 *          then deletes the queue file from disk. Returns empty vector if
	 *          queue file doesn't exist.
	 *
	 * @return  Vector of item IDs that were queued
	 */
	static std::vector<int> loadAndClearQueue();
};