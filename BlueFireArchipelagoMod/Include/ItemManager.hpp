#pragma once

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

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