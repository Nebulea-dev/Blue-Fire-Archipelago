#include <LocationManager.hpp>
#include <ArchipelagoModConfig.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <Helper/UnrealObjectQueries.hpp>

#include <Archipelago.h>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

LocationManager::LocationManager() {}

void LocationManager::Init(HookHelper* hookManager, ObjectCreateListener* objectListener) {
	hookManager->registerPreHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Chest/Chest_Master.Chest_Master_C:Set Used Chest"), OnChestOpened);
	hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/EmoteStatue/EmoteStatue_BP.EmoteStatue_BP_C:CustomEvent_5"), OnDialogueWithStatueEnded);

	// Listen to object creation for EditableTextBox controls
	// objectListener->registerObjectCallback(std::wstring(L"NewItem_C"), OnNewItemCreated);
}

bool LocationManager::OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL) {
	// Get the Used_Chest parameter
	std::optional<bool> Used_Chest = HookHelper::readParamValue<bool>(STR("Used_Chest"), Stack);
    if (!Used_Chest.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the Used_Chest parameter in Set Used Chest function call\n"));
		return false;
    }

	// Get the path of the chest being opened
	const std::wstring chestName = Context->GetNamePrivate().ToString();

	// Match the chest name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromChestName(chestName);
	if (!locationID.has_value())
	{
		Output::send<LogLevel::Error>(STR("Chest name {} not found in ChestNameToLocationID mapping\n"), chestName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Chest {} opened, marking location ID {} as checked in Archipelago\n"), chestName, locationID.value());
	AP_SendItem(locationID.value());

    return false;
}

std::optional<uint32_t> LocationManager::GetLocationIDFromChestName(const std::wstring& chestName) {
	auto it = ChestNameToLocationID.find(chestName);
	if (it != ChestNameToLocationID.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}

bool LocationManager::OnDialogueWithStatueEnded(UObject* Context, FFrame& Stack, void* RESULT_DECL) {

	// Get the emote of the statue
	uint8_t* emoteEnum = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Emote");

	// Check the game inventory
	std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
	if(!gameInstance.has_value())
	{
		Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
		return false;
	}

	// Get the emote inventory
	TArray<uint8_t>* emoteInventory = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<uint8_t>>(L"Emotes");
	if(!emoteInventory)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Emotes parameter of the game instance\n"));
		return false;
	}

	bool bIsEmoteInInventory = false;
	for(int32_t i = 0; i < emoteInventory->Num(); i++)
	{
		uint32_t emote = (*emoteInventory)[i];

		if(emote == *emoteEnum)
		{
			bIsEmoteInInventory = true;
			break;
		}
	}

	// The dialogue has been cancelled and the emote was not purchased
	if(!bIsEmoteInInventory)
	{
		return false;
	}


	const std::wstring statueName = Context->GetNamePrivate().ToString();

	// Match the statue name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromStatueName(statueName);
	if (!locationID.has_value())
	{
		Output::send<LogLevel::Error>(STR("Emote Statue {} not found in statue name mapping\n"), statueName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Emote Statue {} purchased, marking location ID {} as checked in Archipelago\n"), statueName, locationID.value());
	AP_SendItem(locationID.value());

	// TODO : remove item from inventory

	return false;
}

std::optional<uint32_t> LocationManager::GetLocationIDFromStatueName(const std::wstring& statueName) {
	auto it = StatueNameToLocationID.find(statueName);
	if (it != StatueNameToLocationID.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}


void LocationManager::OnNewItemCreated(const UObjectBase* object, int32 index)
{
	Output::send<LogLevel::Verbose>(STR("Hi from OnNewItemCreated!\n"));
}