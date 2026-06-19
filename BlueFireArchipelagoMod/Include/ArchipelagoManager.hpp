#pragma once

#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

/*******************************************************************************
 * ArchipelagoManager
 *
 * @brief   Manages the core Archipelago server connection and event handling.
 *
 *          Responsible for establishing connections to the Archipelago server
 *          with user credentials, registering event callbacks for items and
 *          location checks, and routing server events to appropriate handlers.
 *          Acts as the bridge between the game and the Archipelago multiworld
 *          server.
 */
class ArchipelagoManager
{
	public:
	/*******************************************************************************
	 * @fn      ArchipelagoManager
	 *
	 * @brief   Constructor - initializes Archipelago manager state.
	 *
	 * @return  none
	 */
	ArchipelagoManager();

	/*******************************************************************************
	 * @fn      connectToArchipelagoServer
	 *
	 * @brief   Establish a connection to the Archipelago multiworld server.
	 *
	 *          Attempts to connect using the provided IP, username, and password.
	 *          On successful connection, the server will begin sending events
	 *          (items, location checks, etc.) to the registered callbacks.
	 *
	 * @param   IP       - Pointer to FText containing server IP address
	 * @param   Username - Pointer to FText containing Archipelago username
	 * @param   Password - Pointer to FText containing session password
	 *
	 * @return  none
	 */
	void connectToArchipelagoServer(const FText* IP, const FText* Username, const FText* Password);

	/*******************************************************************************
	 * @fn      cancelConnection
	 *
	 * @brief   Cancels the attempts to establish an Archipelago connection.
	 * 			Should be called upon the user exiting the Archipelago menu.
	 *
	 * @return  none
	 */
	void cancelConnection();

	/*******************************************************************************
	 * @fn      setSuccessfulConnectionCallback
	 *
	 * @brief   Sets the function to be called upon a successful connection
	 * 			established with the Archipelago server. Is used to start the
	 * 			game once the connection is successful.
	 *
	 * @return  none
	 */
	void setSuccessfulConnectionCallback(void (*callback)(void));

	/*******************************************************************************
	 * @fn      ReleaseWorld
	 *
	 * @brief   Releases all the Archipelago locations.
	 *
	 * @return  none
	 */
	void ReleaseWorld();

	private:

	void (*successfulConnectionCallback)(void);
	bool bResetConnectionStatusLoop;

	/*******************************************************************************
	 * @fn      initCallbacks
	 *
	 * @brief   initializes Archipelago callbacks.
	 *
	 *          Registers all event callbacks with the Archipelago library
	 *          (item receive, item clear, location check). Must be called
	 *          once during mod initialization before any server connections
	 *          are attempted and after calling AP_Shutdown.
	 *
	 * @return  none
	 */
	void initCallbacks();

	/*******************************************************************************
	 * @fn      OnItemClear
	 *
	 * @brief   Event handler when the server clears all received items.
	 *
	 *          Called when the Archipelago server sends an item clear event,
	 *          typically used to reset the item state (e.g., on world reset).
	 *
	 * @return  none
	 */
	void OnItemClear();

	/*******************************************************************************
	 * @fn      OnItemReceive
	 *
	 * @brief   Event handler when an item is received from another player.
	 *
	 *          Called by the Archipelago library when another player sends an
	 *          item to this player. Routes the item to ItemManager for display.
	 *
	 * @param   item         - The item ID being received
	 * @param   notifyPlayer - Whether to show a notification to the player
	 *
	 * @return  none
	 */
	void OnItemReceive(int64_t item, bool notifyPlayer);

	/*******************************************************************************
	 * @fn      OnLocationCheck
	 *
	 * @brief   Event handler when a location is checked in another player's world.
	 *
	 *          Called when a location is checked (item collected) in another
	 *          player's game, allowing synchronization of progress across
	 *          the multiworld session.
	 *
	 * @param   location - The location ID being checked
	 *
	 * @return  none
	 */
	void OnLocationCheck(int64_t location);

	/*******************************************************************************
	 * @fn      StaticItemClearCallback
	 *
	 * @brief   Static callback wrapper for item clear events.
	 *
	 *          Static function required by the Archipelago library's callback
	 *          registration system. Delegates to the instance method
	 *          OnItemClear() via the mod manager instance.
	 *
	 * @return  none
	 */
	static void StaticItemClearCallback();

	/*******************************************************************************
	 * @fn      StaticItemReceiveCallback
	 *
	 * @brief   Static callback wrapper for item receive events.
	 *
	 *          Static function required by the Archipelago library's callback
	 *          registration system. Delegates to the instance method
	 *          OnItemReceive() via the mod manager instance.
	 *
	 * @param   item         - The item ID being received
	 * @param   notifyPlayer - Whether to show a notification to the player
	 *
	 * @return  none
	 */
	static void StaticItemReceiveCallback(int64_t item, bool notifyPlayer);

	/*******************************************************************************
	 * @fn      StaticLocationCheckCallback
	 *
	 * @brief   Static callback wrapper for location check events.
	 *
	 *          Static function required by the Archipelago library's callback
	 *          registration system. Delegates to the instance method
	 *          OnLocationCheck() via the mod manager instance.
	 *
	 * @param   location - The location ID being checked
	 *
	 * @return  none
	 */
	static void StaticLocationCheckCallback(int64_t location);
};
