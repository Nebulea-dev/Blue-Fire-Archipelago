#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

#include <Archipelago.h>

#include <ArchipelagoManager.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <ArchipelagoModConfig.hpp>
#include <ItemManager.hpp>
#include <LocationManager.hpp>
#include <DeathLinkManager.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

ArchipelagoManager::ArchipelagoManager() : successfulConnectionCallback(NULL), bResetConnectionStatusLoop(false), bDeathLinkEnabled(false)
{
	this->initCallbacks();
	Output::send<LogLevel::Verbose>(STR("ArchipelagoManager instance created\n"));
}

void ArchipelagoManager::initCallbacks()
{
	AP_SetItemClearCallback(StaticItemClearCallback);
	AP_SetItemRecvCallback(StaticItemReceiveCallback);
	AP_SetLocationCheckedCallback(StaticLocationCheckCallback);
	AP_SetDeathLinkSupported(true);
	AP_SetDeathLinkCallback(StaticDeathLinkCallback);
	AP_RegisterSlotDataIntCallback("item_price", [](int price) {
		if (BlueFireArchipelagoMod::locationManager)
		{
			BlueFireArchipelagoMod::locationManager->SetItemPrice(price);
			Output::send<LogLevel::Verbose>(STR("Received item price from slot data: {}\n"), price);
		}
	});
	AP_RegisterSlotDataBoolCallback("death_link", [](bool enabled) {
		if (BlueFireArchipelagoMod::arcManager)
		{
			BlueFireArchipelagoMod::arcManager->bDeathLinkEnabled = enabled;
			Output::send<LogLevel::Verbose>(STR("Received death_link setting from slot data: {}\n"), enabled);
		}
	});
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
	// If the item was already given
	if(!notifyPlayer) return;

	if (!BlueFireArchipelagoMod::itemManager)
	{
		Output::send<LogLevel::Error>(STR("itemManager is null in OnItemReceive\n"));
		return;
	}

	BlueFireArchipelagoMod::itemManager->itemReceiveCb((int)item - ArchipelagoModConfig::Archipelago::BF_BASE_ID);
}

void ArchipelagoManager::OnLocationCheck(int64_t location)
{
	Output::send<LogLevel::Verbose>(STR("Checked location with id {} from Archipelago\n"), location - ArchipelagoModConfig::Archipelago::BF_BASE_ID);
}

void ArchipelagoManager::OnDeathLink()
{
	if (!bDeathLinkEnabled)
	{
		Output::send<LogLevel::Verbose>(STR("Deathlink received but deathlink is disabled, ignoring\n"));
		return;
	}

	Output::send<LogLevel::Verbose>(STR("Received deathlink from Archipelago\n"));
	if (!BlueFireArchipelagoMod::deathLinkManager)
	{
		Output::send<LogLevel::Error>(STR("deathLinkManager is null in OnDeathLink\n"));
		return;
	}

	BlueFireArchipelagoMod::deathLinkManager->onDeathLinkReceived();
}

void ArchipelagoManager::StaticDeathLinkCallback()
{
	if (BlueFireArchipelagoMod::arcManager)
	{
		BlueFireArchipelagoMod::arcManager->OnDeathLink();
	}
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

	/// Loop until the connection is successful
	AP_ConnectionStatus status;
	bool bConnectionSuccessful = false;
	bResetConnectionStatusLoop = false;

	while(!bConnectionSuccessful && !bResetConnectionStatusLoop)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		status = AP_GetConnectionStatus();
		switch (status)
		{
		case AP_ConnectionStatus::Disconnected:
			Output::send<LogLevel::Error>(STR("Connection to Archipelago server failed, check your login details.\n"));
			break;

		case AP_ConnectionStatus::ConnectionRefused:
			Output::send<LogLevel::Error>(STR("Connection to Archipelago server refused, check the IP details\n"));
			break;

		case AP_ConnectionStatus::Connected:
			Output::send<LogLevel::Verbose>(STR("Connection to Archipelago server successful, authenticating ...\n"));
			break;

		case AP_ConnectionStatus::Authenticated:
			Output::send<LogLevel::Verbose>(STR("Authentication to Archipelago server successful, starting game ...\n"));
			this->successfulConnectionCallback();
			bConnectionSuccessful = true;
			break;

		default:
			Output::send<LogLevel::Error>(STR("Connection status is not recognized, open a Github issue please!\n"));
			break;
		}
	}

	bResetConnectionStatusLoop = false;
}

void ArchipelagoManager::cancelConnection()
{
	AP_Shutdown();
	this->bResetConnectionStatusLoop = true;
	this->initCallbacks();
}

void ArchipelagoManager::setSuccessfulConnectionCallback(void (*callback)(void))
{
	this->successfulConnectionCallback = callback;
}

void ArchipelagoManager::ReleaseWorld()
{
    Output::send<LogLevel::Verbose>(STR("Game completed, releasing world...\n"));
	AP_StoryComplete();
}

bool ArchipelagoManager::isDeathLinkEnabled() const
{
	return bDeathLinkEnabled;
}