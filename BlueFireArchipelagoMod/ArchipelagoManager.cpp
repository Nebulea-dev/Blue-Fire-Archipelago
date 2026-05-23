#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

#include <Archipelago.h>

#include <ArchipelagoManager.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <ArchipelagoModConfig.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

ArchipelagoManager::ArchipelagoManager()
{
	Output::send<LogLevel::Verbose>(STR("ArchipelagoManager instance created\n"));
}

void ArchipelagoManager::init()
{
	AP_SetItemClearCallback(StaticItemClearCallback);
	AP_SetItemRecvCallback(StaticItemReceiveCallback);
	AP_SetLocationCheckedCallback(StaticLocationCheckCallback);

	Output::send<LogLevel::Verbose>(STR("Archipelago callbacks set\n"));
}

void ArchipelagoManager::StaticItemClearCallback()
{
	if (BlueFireArchipelagoMod::arcManager)
	{
		BlueFireArchipelagoMod::arcManager->OnItemClear();
	}
}

void ArchipelagoManager::StaticItemReceiveCallback(int64_t item, bool notifyPlayer)
{
	if (BlueFireArchipelagoMod::arcManager)
	{
		BlueFireArchipelagoMod::arcManager->OnItemReceive(item, notifyPlayer);
	}
}

void ArchipelagoManager::StaticLocationCheckCallback(int64_t location)
{
	if (BlueFireArchipelagoMod::arcManager)
	{
		BlueFireArchipelagoMod::arcManager->OnLocationCheck(location);
	}
}

void ArchipelagoManager::OnItemClear()
{
	Output::send<LogLevel::Verbose>(STR("Clearing items received from Archipelago\n"));
}

void ArchipelagoManager::OnItemReceive(int64_t item, bool notifyPlayer)
{
	Output::send<LogLevel::Verbose>(STR("Received item with id {} from Archipelago, notifyPlayer = {}\n"), item, notifyPlayer);
}

void ArchipelagoManager::OnLocationCheck(int64_t location)
{
	Output::send<LogLevel::Verbose>(STR("Checked location with id {} from Archipelago\n"), location);
}

void ArchipelagoManager::connectToArchipelagoServer(const FText* IP, const FText* Username, const FText* Password)
{
    // AP_Init only accepts const char*, so we use wcstombs to convert FText to const char*

    // First we check the length so no buffer overflow
    if(IP->ToString().length() >= 256 || Username->ToString().length() >= 256 || Password->ToString().length() >= 256)
    {
        Output::send<LogLevel::Error>(STR("One or more input strings are too long\n"));
        return;
    }

    char IPCstr[256];
    char UsernameCstr[256];
    char PasswordCstr[256];

    // Convert FText to const char*
    wcstombs_s(nullptr, IPCstr, sizeof(IPCstr), IP->ToString().c_str(), _TRUNCATE);
    wcstombs_s(nullptr, UsernameCstr, sizeof(UsernameCstr), Username->ToString().c_str(), _TRUNCATE);
    wcstombs_s(nullptr, PasswordCstr, sizeof(PasswordCstr), Password->ToString().c_str(), _TRUNCATE);

    /// Initialize Archipelago client
    Output::send<LogLevel::Verbose>(STR("Connecting to Archipelago server...\n"));
    AP_Init(IPCstr, Archipelago::GAME_NAME, UsernameCstr, PasswordCstr);
    AP_Start();
}
