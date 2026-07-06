#pragma once

#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObject.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/Class.hpp>

#include <Helper/ObjectCreateListener.hpp>
#include <Helper/HookHelper.hpp>

using namespace RC;
using namespace Unreal;

// Forward declarations
class MainMenuManager;
class HookHelper;
class ItemManager;
class ArchipelagoManager;
class LocationManager;
class DeathLinkManager;

/*******************************************************************************
 * BlueFireArchipelagoMod
 *
 * @brief   Main entry point for the Blue Fire Archipelago mod.
 *
 *          Inherits from CppUserModBase to integrate with the mod loader.
 *          Manages initialization of all subsystems (menu, hooks, items, server
 *          connection) and serves as the central hub for static manager instances.
 *          All hook callbacks route through this class to delegate to manager
 *          instances.
 */
class BlueFireArchipelagoMod : public RC::CppUserModBase
{
public:
	/// Object creation listener for detecting Unreal object instantiation
	static inline ObjectCreateListener* objectListener = nullptr;

	/// Menu manager instance - handles UI creation and navigation
	static inline MainMenuManager* mainMenuManager = nullptr;

	/// Hook helper instance - manages function hooking infrastructure
	static inline HookHelper* hookManager = nullptr;

	/// Item manager instance - handles item display and reception
	static inline ItemManager* itemManager = nullptr;

	/// Archipelago manager instance - manages server connection and events
	static inline ArchipelagoManager* arcManager = nullptr;

	/// Location manager instance - manages location checks and events
	static inline LocationManager* locationManager = nullptr;

	/// Death link manager instance - manages deathlink events
	static inline DeathLinkManager* deathLinkManager = nullptr;

	/*******************************************************************************
	 * @fn      BlueFireArchipelagoMod
	 *
	 * @brief   Constructor - initializes the mod instance.
	 *
	 * @return  none
	 */
	BlueFireArchipelagoMod();

	/*******************************************************************************
	 * @fn      ~BlueFireArchipelagoMod
	 *
	 * @brief   Destructor - cleans up all manager instances.
	 *
	 *          Deletes all static manager instances and releases resources.
	 *
	 * @return  none
	 */
	~BlueFireArchipelagoMod();

	/*******************************************************************************
	 * @fn      on_unreal_init
	 *
	 * @brief   Called when Unreal Engine has initialized.
	 *
	 *          Performs all mod setup: creates manager instances, registers
	 *          hooks, registers object creation listeners, and initializes
	 *          the Archipelago event system. This is the main initialization
	 *          entry point called by the mod loader.
	 *
	 * @return  none
	 */
	void on_unreal_init();

	/*******************************************************************************
	 * @fn      ObjectCreatedNewItem
	 *
	 * @brief   Static callback when NewItem objects are created in the engine.
	 *
	 *          Invoked by ObjectCreateListener when a NewItem UI element is
	 *          instantiated. Triggers Archipelago menu UI creation.
	 *
	 * @param   object - Pointer to the created UObject
	 * @param   index  - Index of the object in the object array
	 *
	 * @return  none
	 */
	static void ObjectCreatedNewItem(const UObjectBase* object, int32 index);

	/*******************************************************************************
	 * @fn      PlayNewItemPreHook
	 *
	 * @brief   Static hook callback for NewItem.PlayNewItem function.
	 *
	 *          Pre-hook that intercepts item display, delegating to ItemManager
	 *          to modify item parameters before display. Allows customization
	 *          of Archipelago item appearance and information.
	 *
	 * @param   Context     - Unreal object context
	 * @param   Stack       - Function stack frame with parameters
	 * @param   RESULT_DECL - Result declaration pointer
	 *
	 * @return  true to prevent original function, false to allow it
	 */
	static bool PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	private:
	/*******************************************************************************
	 * @fn      connectToArchipelagoServer
	 *
	 * @brief   Internal helper to delegate server connection to ArchipelagoManager.
	 *
	 *          Private method called by the menu system when connection is
	 *          submitted. Routes the credentials to the ArchipelagoManager
	 *          instance for actual server connection.
	 *
	 * @param   IP       - Pointer to FText containing server IP address
	 * @param   Username - Pointer to FText containing Archipelago username
	 * @param   Password - Pointer to FText containing session password
	 *
	 * @return  none
	 */
	static void connectToArchipelagoServer(FText* IP, FText* Username, FText* Password);
};