#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include <Archipelago.h>

#include <BlueFireArchipelagoMod.hpp>
#include <DeathLinkManager.hpp>

using namespace RC;
using namespace Unreal;

DeathLinkManager::DeathLinkManager()
	: bSkipNextDeathHudConstruct(false)
{
	Output::send<LogLevel::Verbose>(STR("DeathLinkManager instance created\n"));

	BlueFireArchipelagoMod::hookManager->registerPreHook(STR("Function /Game/BlueFire/Player/Logic/Misc/DeathHud.DeathHud_C:Construct"), [](UObject* Context, FFrame& Stack, void* RESULT_DECL) {
		if (BlueFireArchipelagoMod::deathLinkManager)
		{
			return BlueFireArchipelagoMod::deathLinkManager->DeathHudConstructPreHook(Context, Stack, RESULT_DECL);
		}
		return false;
	});

	BlueFireArchipelagoMod::hookManager->registerPreHook(STR("Function /Game/BlueFire/Player/Logic/Misc/DeathHud.DeathHud_C:Remove"), [](UObject* Context, FFrame& Stack, void* RESULT_DECL) {
		if (BlueFireArchipelagoMod::deathLinkManager)
		{
			return BlueFireArchipelagoMod::deathLinkManager->DeathHudRemovePreHook(Context, Stack, RESULT_DECL);
		}
		return false;
	});
}

void DeathLinkManager::onDeathLinkReceived()
{
	Output::send<LogLevel::Verbose>(STR("Deathlink received, killing player\n"));
	bSkipNextDeathHudConstruct = true;
	killPlayer();
}

bool DeathLinkManager::DeathHudConstructPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
	if (bSkipNextDeathHudConstruct)
	{
		Output::send<LogLevel::Verbose>(STR("Skipping deathlink send (received deathlink)\n"));
		bSkipNextDeathHudConstruct = false;
	}
	else if (BlueFireArchipelagoMod::arcManager && BlueFireArchipelagoMod::arcManager->isDeathLinkEnabled())
	{
		Output::send<LogLevel::Verbose>(STR("Player died, sending deathlink\n"));
		sendDeathLink();
	}

	return false;
}

bool DeathLinkManager::DeathHudRemovePreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
	Output::send<LogLevel::Verbose>(STR("DeathHud removed, clearing deathlink flag\n"));
	bSkipNextDeathHudConstruct = false;
	return false;
}

void DeathLinkManager::killPlayer()
{
	UFunction* RemoveLifeFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, L"/Game/BlueFire/Player/Logic/Player_Character_BP.Player_Character_BP_C:RemoveLife");
	if (!RemoveLifeFunc)
	{
		Output::send<LogLevel::Error>(STR("Could not find RemoveLife function\n"));
		return;
	}

	UObject* playerCharacter = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, L"/Game/BlueFire/Player/Logic/Player_Character_BP");
	if (!playerCharacter)
	{
		Output::send<LogLevel::Error>(STR("Could not find Player_Character_BP\n"));
		return;
	}

	struct removeLifeStruct
	{
		float inputPin;
		bool PlayMontage;
		bool SkipStopMontage;
		bool keep1;
		bool isFall;
	} params{
		10000,  // inputPin
		true,   // PlayMontage
		false,  // SkipStopMontage
		false,  // keep1
		false   // isFall
	};

	playerCharacter->ProcessEvent(RemoveLifeFunc, &params);
	Output::send<LogLevel::Verbose>(STR("RemoveLife function called on player\n"));
}

void DeathLinkManager::sendDeathLink()
{
	if (!BlueFireArchipelagoMod::arcManager)
	{
		Output::send<LogLevel::Error>(STR("ArchipelagoManager not initialized, cannot send deathlink\n"));
		return;
	}

	Output::send<LogLevel::Verbose>(STR("Sending deathlink to Archipelago server\n"));
	AP_DeathLink();
}
