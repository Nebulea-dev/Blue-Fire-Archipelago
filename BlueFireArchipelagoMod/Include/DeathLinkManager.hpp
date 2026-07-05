#pragma once

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

using namespace RC;
using namespace Unreal;

/*******************************************************************************
 * DeathLinkManager
 *
 * @brief   Manages deathlink functionality for Archipelago integration.
 *
 *          Responsible for intercepting player death events and sending deathlinks
 *          to other players in the multiworld session. Also handles receiving
 *          deathlinks from other players and killing the local player accordingly.
 *          Hooks into DeathHud.Construct and DeathHud.Remove to manage the death
 *          flow and prevent duplicate death notifications.
 */
class DeathLinkManager
{
	public:
	/*******************************************************************************
	 * @fn      DeathLinkManager
	 *
	 * @brief   Constructor - initializes death link manager state.
	 *
	 * @return  none
	 */
	DeathLinkManager();

	/*******************************************************************************
	 * @fn      onDeathLinkReceived
	 *
	 * @brief   Called when a deathlink is received from another player.
	 *
	 *          Marks that a deathlink was received so the next DeathHud.Construct
	 *          will not send an outgoing deathlink. Kills the local player using
	 *          the Player_Character_BP.RemoveLife function.
	 *
	 * @return  none
	 */
	void onDeathLinkReceived();

	/*******************************************************************************
	 * @fn      DeathHudConstructPreHook
	 *
	 * @brief   Pre-hook callback for DeathHud.Construct function.
	 *
	 *          Intercepts the death HUD construction. If a deathlink was just
	 *          received, skips the outgoing deathlink. Otherwise, sends a deathlink
	 *          to the Archipelago server to notify other players.
	 *
	 * @param   Context     - Unreal object context
	 * @param   Stack       - Function stack frame with parameters
	 * @param   RESULT_DECL - Result declaration pointer
	 *
	 * @return  false to allow original function execution
	 */
	bool DeathHudConstructPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	/*******************************************************************************
	 * @fn      DeathHudRemovePreHook
	 *
	 * @brief   Pre-hook callback for DeathHud.Remove function.
	 *
	 *          Clears the deathlink flag when the death HUD is removed, resetting
	 *          the state for the next death event.
	 *
	 * @param   Context     - Unreal object context
	 * @param   Stack       - Function stack frame with parameters
	 * @param   RESULT_DECL - Result declaration pointer
	 *
	 * @return  false to allow original function execution
	 */
	bool DeathHudRemovePreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	private:
	bool bSkipNextDeathHudConstruct;

	/*******************************************************************************
	 * @fn      killPlayer
	 *
	 * @brief   Kills the local player by calling RemoveLife on the player character.
	 *
	 *          Executes the Player_Character_BP.RemoveLife function with appropriate
	 *          parameters to kill the player and trigger death sequences.
	 *
	 * @return  none
	 */
	void killPlayer();

	/*******************************************************************************
	 * @fn      sendDeathLink
	 *
	 * @brief   Sends a deathlink to the Archipelago server.
	 *
	 *          Notifies all other players in the multiworld session that the
	 *          local player has died.
	 *
	 * @return  none
	 */
	void sendDeathLink();
};
