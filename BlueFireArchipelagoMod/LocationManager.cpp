#include <LocationManager.hpp>
#include <ItemManager.hpp>
#include <ArchipelagoModConfig.hpp>
#include <LocationsData.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <Helper/UnrealObjectQueries.hpp>

#include <Archipelago.h>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

LocationManager::LocationManager()
{
	BlueFireArchipelagoMod::hookManager->registerPreHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Chest/Chest_Master.Chest_Master_C:Set Used Chest"), OnChestOpened);
	BlueFireArchipelagoMod::hookManager->registerPreHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Chest/Chest_Master.Chest_Master_C:PressButton"), OnPressButton);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/EmoteStatue/EmoteStatue_BP.EmoteStatue_BP_C:CustomEvent_5"), OnDialogueWithStatueEnded);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Pickup/Pickup.Pickup_C:Used"), OnItemPickup);
	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Pickup/Pickup.Pickup_C:Remove"), OnItemPickupRemove);

	// Listen to object creation for EditableTextBox controls
	// objectListener->registerObjectCallback(std::wstring(L"NewItem_C"), OnNewItemCreated);
}

// ============== Chest related methods ==============

bool LocationManager::OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
	// Get the path of the chest being opened
	const std::wstring chestName = Context->GetNamePrivate().ToString();

	// Match the chest name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromChestName(chestName);
	if (!locationID.has_value())
	{
		logIncorrectMapping(chestName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Chest {} opened, marking location ID {} as checked in Archipelago\n"), chestName, locationID.value());
	AP_SendItem(locationID.value());


    return false;
}

std::optional<uint32_t> LocationManager::GetLocationIDFromChestName(const std::wstring& chestName)
{
	const auto& chestMap = LocationsData::GetChestNameToLocationIDMap();
	auto it = chestMap.find(chestName);
	if (it != chestMap.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}


bool LocationManager::OnPressButton(UObject* Context, FFrame& Stack, void* RESULT_DECL) {
	// Get the Type parameter
	uint8_t* chestType = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Type");
	if(!chestType)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Type parameter of the opened chest\n"));
		return false;
	}

	// Get the Weapon parameter
	uint8_t* chestWeapon = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Weapon");
	if(!chestWeapon)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Weapon parameter of the opened chest\n"));
		return false;
	}

	// Set the chest content to the default sword
	//*chestType = 1;
	//*chestWeapon = 0;

    return false;
}

// ============== Emote Statue related methods ==============

bool LocationManager::OnDialogueWithStatueEnded(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{

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

	// The dialogue has been cancelled and the emote was not purchased
	if(emoteInventory->Top() != *emoteEnum)
	{
		return false;
	}


	const std::wstring statueName = Context->GetNamePrivate().ToString();

	// Match the statue name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromStatueName(statueName);
	if (!locationID.has_value())
	{
		logIncorrectMapping(statueName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Emote Statue {} purchased, marking location ID {} as checked in Archipelago\n"), statueName, locationID.value());
	AP_SendItem(locationID.value());

	// TODO : remove item from inventory
	emoteInventory->Pop(true);


	return false;
}


std::optional<uint32_t> LocationManager::GetLocationIDFromStatueName(const std::wstring& statueName)
{
	const auto& statueMap = LocationsData::GetStatueNameToLocationIDMap();
	auto it = statueMap.find(statueName);
	if (it != statueMap.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}

// ============== Pickup related methods ==============

bool LocationManager::OnItemPickup(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{

	// Get the path of the pickup
	const std::wstring pickupName = Context->GetNamePrivate().ToString();

	// Match the pickup name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromPickupName(pickupName);
	if (!locationID.has_value())
	{
		logIncorrectMapping(pickupName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Pickup {} picked up, marking location ID {} as checked in Archipelago\n"), pickupName, locationID.value());
	AP_SendItem(locationID.value());

	// Get the Type parameter
	uint8_t* pickupType = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Type");
	if(!pickupType)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Type parameter of the picked up item\n"));
		return false;
	}

	// Get the Weapon parameter
	uint32_t* pickupItem = Context->GetValuePtrByPropertyNameInChain<uint32_t>(L"Item");
	if(!pickupItem)
	{
		Output::send<LogLevel::Error>(STR("Could not find the Item parameter of the picked up item\n"));
		return false;
	}

	// Set the chest content to the default sword
	//*pickupType = 1; // Item type
	//*pickupItem = 17; // Book

    return false;
}

bool LocationManager::OnItemPickupRemove(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();

    // Get the "PlayerStats" property
    FStructProperty* playerStatsProperty = static_cast<FStructProperty*>(gameInstance.value()->GetPropertyByNameInChain(L"PlayerStats"));
    if (!playerStatsProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PlayerStats property\n"));
        return false;
    }

    // Get the "PlayerEquipment.PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94" property
    auto playerStatsStruct = playerStatsProperty->GetStruct();
    auto playerStats = playerStatsProperty->ContainerPtrToValuePtr<void>(gameInstance.value());
    FStructProperty* inventoryProperty = static_cast<FStructProperty*>(playerStatsStruct->GetPropertyByNameInChain(L"PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94"));
    if (!inventoryProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94 property in PlayerStats\n"));
        return false;
    }

    TArray<inventoryItem>* inventory = inventoryProperty->ContainerPtrToValuePtr<TArray<inventoryItem>>(playerStats);

    // Go through all items already in inventory
    for(int32_t i = 0; i < inventory->Num(); i++)
    {
        inventoryItem item = (*inventory)[i];

        // If the inventory item isn't of type "item"
        if(item.type != 0)
        {
            Output::send<LogLevel::Verbose>(STR("Item is of type {}, skipping\n"), item.type);
            continue;
        }

        // If not the correct item
        if(item.item != 17)
        {
            Output::send<LogLevel::Verbose>(STR("Item is of id {}, skipping\n"), item.item);
            continue;
        }

		// If item is found in inventory, decrease quantity or remove it
		if(item.amount > 1)
		{
			item.amount -= 1;
			(*inventory)[i] = item;

			return false;
		}
		else
		{
			inventory->RemoveAt(i);
			return false;
		}
    }

    return false;
}


std::optional<uint32_t> LocationManager::GetLocationIDFromPickupName(const std::wstring& pickupName)
{
	const auto& pickupMap = LocationsData::GetPickupNameToLocationIDMap();
	auto it = pickupMap.find(pickupName);
	if (it != pickupMap.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}

// ============== Logs methods ==============

void LocationManager::OnNewItemCreated(const UObjectBase* object, int32 index)
{
	Output::send<LogLevel::Verbose>(STR("Hi from OnNewItemCreated!\n"));
}

void LocationManager::logIncorrectMapping(const std::wstring locationName)
{
	Output::send<LogLevel::Error>(STR("Hi ! It looks like the location you just checked was not correctly registered in the location mapping in	ArchipelagoModConfig.hpp."));
	Output::send<LogLevel::Error>(STR("This mod is still very new, and some chests or other locations might be missing."));
	Output::send<LogLevel::Error>(STR("Please open an issue on Github or contact nebulea__ on discord to get this added."));
	Output::send<LogLevel::Error>(STR("Be sure to include a description of where you found this location, and include the following location name in your message :"));
	Output::send<LogLevel::Error>(STR("{}"), locationName);
}