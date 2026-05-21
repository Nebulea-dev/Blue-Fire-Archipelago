#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

#include <Archipelago.h>

#include <ArchipelagoLogic.hpp>

using namespace RC;
using namespace Unreal;

void ArchipelagoLogic::init()
{
	AP_SetItemClearCallback(itemClearCallback);
	AP_SetItemRecvCallback(itemReceiveCallback);
	AP_SetLocationCheckedCallback(locationCheckCallback);

	Output::send<LogLevel::Verbose>(STR("Archipelago callbacks set\n"));
}

void ArchipelagoLogic::connectToArchipelagoServer(const FText* IP, const FText* Username, const FText* Password)
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
    AP_Init(IPCstr, "Blue Fire", UsernameCstr, PasswordCstr);
    AP_Start();
}

void ArchipelagoLogic::itemClearCallback()
{
	Output::send<LogLevel::Verbose>(STR("Clearing items received from Archipelago\n"));
}

void ArchipelagoLogic::itemReceiveCallback(int64_t item, bool notifyPlayer)
{
	Output::send<LogLevel::Verbose>(STR("Received item with id {} from Archipelago, notifyPlayer = {}\n"), item, notifyPlayer);
}

void ArchipelagoLogic::locationCheckCallback(int64_t location)
{
	Output::send<LogLevel::Verbose>(STR("Checked location with id {} from Archipelago\n"), location);
}