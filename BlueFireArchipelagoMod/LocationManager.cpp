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

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Spirit/Spirit.Spirit_C:Used"), OnSpiritPickup);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Bounds/Void/Void_Gate.Void_Gate_C:Glow Eyes"), OnVoidGateCompleted);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/Player/Logic/FrameWork/Global_Controller.Global_Controller_C:LoadAllLevels"), OnLevelLoaded);
	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/HUD/Dialog/MainDialogWB.MainDialogWB_C:WriteShopArrayToGI"), OnItemBought);


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
	*chestType = 1;
	*chestWeapon = 0;

    return false;
}

// ============== Emote Statue related methods ==============

bool LocationManager::OnDialogueWithStatueEnded(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{

	// Get the emote of the statue
	uint8_t* emoteEnum = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Emote");
	if (!emoteEnum)
	{
		Output::send<LogLevel::Error>(STR("Could not find the Emote parameter of the statue\n"));
		return false;
	}

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

    return false;

	// Set the chest content to the default sword
	*pickupType = 1; // Item type
	*pickupItem = 17; // Book

    return false;
}

bool LocationManager::OnItemPickupRemove(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return false;
    }

    // Get the "PlayerStats" property
    FStructProperty* playerStatsProperty = static_cast<FStructProperty*>(gameInstance.value()->GetPropertyByNameInChain(L"PlayerStats"));
    if (!playerStatsProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PlayerStats property\n"));
        return false;
    }

    // Get the "PlayerEquipment.PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94" property
    auto playerStatsStruct = playerStatsProperty->GetStruct();
    if (!playerStatsStruct)
    {
        Output::send<LogLevel::Error>(STR("Could not get struct from PlayerStats property\n"));
        return false;
    }

    auto playerStats = playerStatsProperty->ContainerPtrToValuePtr<void>(gameInstance.value());
    if (!playerStats)
    {
        Output::send<LogLevel::Error>(STR("Could not get PlayerStats value pointer\n"));
        return false;
    }

    FStructProperty* inventoryProperty = static_cast<FStructProperty*>(playerStatsStruct->GetPropertyByNameInChain(L"PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94"));
    if (!inventoryProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94 property in PlayerStats\n"));
        return false;
    }

    TArray<inventoryItem>* inventory = inventoryProperty->ContainerPtrToValuePtr<TArray<inventoryItem>>(playerStats);
    if (!inventory)
    {
        Output::send<LogLevel::Error>(STR("Could not get inventory value pointer\n"));
        return false;
    }

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

// ============== Spirit related methods ==============

bool LocationManager::OnSpiritPickup(UObject* Context, FFrame& Stack, void* RESULT_DECL)
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
	uint8_t* spiritType = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Amulet");
	if(!spiritType)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Amulet parameter of the picked up spirit\n"));
		return false;
	}

	// Set the spirit type to an unused spirit
	*spiritType = 5;

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

// ============== Void Gate related methods ==============

bool LocationManager::OnVoidGateCompleted(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
	// Get the path of the gate
	const std::wstring gateName = Context->GetNamePrivate().ToString();

	// Match the gate name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromVoidGateName(gateName);
	if (!locationID.has_value())
	{
		logIncorrectMapping(gateName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Void Gate {} completed, marking location ID {} as checked in Archipelago\n"), gateName, locationID.value());
	AP_SendItem(locationID.value());

    return false;
}


std::optional<uint32_t> LocationManager::GetLocationIDFromVoidGateName(const std::wstring& voidGateName)
{
	const auto& voidGateMap = LocationsData::GetVoidGateNameToLocationIDMap();
	auto it = voidGateMap.find(voidGateName);
	if (it != voidGateMap.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}

// ============== Shop related methods ==============

bool LocationManager::OnLevelLoaded(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return false;
    }

	std::map<uint8_t, uint32_t>::const_iterator itemIndex;

    TArray<inventoryItem>* shopMork = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"Shop0-Mork");
    if(!shopMork)
    {
        Output::send<LogLevel::Error>(STR("Could not find Shop0-Mork property\n"));
        return false;
    }

    TArray<inventoryItem>* shopOnrom = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"Shop1-Onrom");
    if(!shopOnrom)
    {
        Output::send<LogLevel::Error>(STR("Could not find Shop1-Onrom property\n"));
        return false;
    }

    TArray<inventoryItem>* shopSpiritHunter = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"Shop2-SpiritHunter");
    if(!shopSpiritHunter)
    {
        Output::send<LogLevel::Error>(STR("Could not find Shop2-SpiritHunter property\n"));
        return false;
    }

    TArray<inventoryItem>* shopAri = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"Shop3-Ari");
    if(!shopAri)
    {
        Output::send<LogLevel::Error>(STR("Could not find Shop3-Ari property\n"));
        return false;
    }

    TArray<inventoryItem>* shopPoti = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"Shop4-Poti");
    if(!shopPoti)
    {
        Output::send<LogLevel::Error>(STR("Could not find Shop4-Poti property\n"));
        return false;
    }

    TArray<inventoryItem>* shopPOI = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"Shop5-POI");
    if(!shopPOI)
    {
        Output::send<LogLevel::Error>(STR("Could not find Shop5-POI property\n"));
        return false;
    }

	TArray<inventoryItem>* shops[6] = {shopMork, shopOnrom, shopSpiritHunter, shopAri, shopPoti, shopPOI};

	// Mork
	for(int32_t i = 0; i < shopMork->Num(); i++)
    {
		itemIndex = Shops::Mork::spiritInventory.end();
        inventoryItem item = (*shopMork)[i];

		if(item.type == 3)
		{
			itemIndex = Shops::Mork::spiritInventory.find(item.spirit);
		}

		if(item.type == 0)
		{
			itemIndex = Shops::Mork::itemInventory.find(item.item);
		}

		if (itemIndex == Shops::Mork::itemInventory.end())
		{
			Output::send<LogLevel::Error>(STR("Could not find the item in Mork's shop\n"));
			return false;
		}

		item.originalAmount = itemIndex->second;

        (*shopMork)[i] = item;
    }

	// Onrom
	for(int32_t i = 0; i < shopOnrom->Num(); i++)
    {
        inventoryItem item = (*shopOnrom)[i];

		itemIndex = Shops::Onrom::inventory.find(item.item);
		if (itemIndex == Shops::Onrom::inventory.end())
		{
			Output::send<LogLevel::Error>(STR("Could not find the item in Onrom's shop\n"));
			return false;
		}

		item.originalAmount = itemIndex->second;

        (*shopOnrom)[i] = item;
    }

	// Spirit Hunter
	for(int32_t i = 0; i < shopSpiritHunter->Num(); i++)
    {
        inventoryItem item = (*shopSpiritHunter)[i];

		itemIndex = Shops::SpiritHunter::inventory.find(item.spirit);
		if (itemIndex == Shops::SpiritHunter::inventory.end())
		{
			Output::send<LogLevel::Error>(STR("Could not find the item in Spirit Hunter's shop\n"));
			return false;
		}

		item.originalAmount = itemIndex->second;

        (*shopSpiritHunter)[i] = item;
    }

	// Ari
	for(int32_t i = 0; i < shopAri->Num(); i++)
    {
        inventoryItem item = (*shopAri)[i];

		itemIndex = Shops::Ari::inventory.find(item.tunic);
		if (itemIndex == Shops::Ari::inventory.end())
		{
			Output::send<LogLevel::Error>(STR("Could not find the item in Ari's shop\n"));
			return false;
		}

		item.originalAmount = itemIndex->second;

        (*shopAri)[i] = item;
    }

	// Poti
	for(int32_t i = 0; i < shopPoti->Num(); i++)
    {
		itemIndex = Shops::Mork::spiritInventory.end();
        inventoryItem item = (*shopPoti)[i];

		if(item.type == 0)
		{
			itemIndex = Shops::Poti::itemInventory.find(item.item);
		}

		if(item.type == 1)
		{
			itemIndex = Shops::Poti::weaponInventory.find(item.weapon);
		}

		if(item.type == 3)
		{
			itemIndex = Shops::Poti::spiritInventory.find(item.spirit);
		}

		if (itemIndex == Shops::Poti::itemInventory.end())
		{
			Output::send<LogLevel::Error>(STR("Could not find the item in Poti's shop\n"));
			return false;
		}

		Output::send<LogLevel::Verbose>(STR("Setting id {} for item {}\n"), itemIndex->second, i);

		item.originalAmount = itemIndex->second;

        (*shopPoti)[i] = item;
    }

	// Poi
	for(int32_t i = 0; i < shopPOI->Num(); i++)
    {
        inventoryItem item = (*shopPOI)[i];

		itemIndex = Shops::Poi::inventory.find(item.item);
		if (itemIndex == Shops::Poi::inventory.end())
		{
			Output::send<LogLevel::Error>(STR("Could not find the item in Ari's shop\n"));
			return false;
		}

		item.originalAmount = itemIndex->second;

        (*shopPOI)[i] = item;
    }

	// Set all items to the default weapon
	for(TArray<inventoryItem>* inventoryPointer : shops)
	{
		for(int32_t i = 0; i < inventoryPointer->Num(); i++)
		{
			inventoryItem item = (*inventoryPointer)[i];

			item.amount = 1;
			item.type = 1;
			item.weapon = 0;
			item.price = 100;

			(*inventoryPointer)[i] = item;
		}
	}

    return false;
}

bool LocationManager::OnItemBought(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
	uint32_t* SelectedShopItem = Context->GetValuePtrByPropertyNameInChain<uint32_t>(L"SelectedShopItem");
	if(!SelectedShopItem)
	{
        Output::send<LogLevel::Error>(STR("Could not find the SelectedShopItem parameter of the MainDialogWB_C object\n"));
		return false;
	}

	uint32_t* WorldShop = Context->GetValuePtrByPropertyNameInChain<uint32_t>(L"WorldShop");
	if(!WorldShop)
	{
        Output::send<LogLevel::Error>(STR("Could not find the SelectedShopItem parameter of the MainDialogWB_C object\n"));
		return false;
	}

    TArray<inventoryItem>* ShopInventory = Context->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"ShopInventory");
	if(!ShopInventory)
	{
        Output::send<LogLevel::Error>(STR("Could not find the SelectedShopItem parameter of the MainDialogWB_C object\n"));
		return false;
	}

	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromShopID(*WorldShop);
	if (!locationID.has_value())
	{
		logIncorrectMapping(std::to_wstring(*WorldShop));
		return false;
	}

	inventoryItem itemBought = (*ShopInventory)[*SelectedShopItem];
	uint32_t archipelagoShopLocationID = locationID.value() + itemBought.originalAmount;

	Output::send<LogLevel::Verbose>(STR("Item bought in shop {}, marking location ID {} as checked in Archipelago\n"), *WorldShop, archipelagoShopLocationID);
	AP_SendItem(archipelagoShopLocationID);

    return false;
}

std::optional<uint32_t> LocationManager::GetLocationIDFromShopID(const uint32_t shopID)
{
	const auto& shopMap = LocationsData::GetShopIDToLocationIDMap();
	auto it = shopMap.find(shopID);
	if (it != shopMap.end())
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