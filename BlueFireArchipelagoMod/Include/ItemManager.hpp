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
	 * @param   notify  - Whether to show a notification to the player
	 *
	 * @return  none
	 */
	void itemReceiveCb(int itemID, bool notify);

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

	private:
    FText* itemName;
    FText* itemDescription;
};