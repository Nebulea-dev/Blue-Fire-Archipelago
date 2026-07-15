#include <LocationManager.hpp>
#include <ItemManager.hpp>
#include <ArchipelagoManager.hpp>
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

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/EmoteStatue/EmoteStatue_BP.EmoteStatue_BP_C:CustomEvent_0"), OnDialogueWithStatueEnded);
	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/EmoteStatue/EmoteStatue_BP.EmoteStatue_BP_C:CustomEvent_5"), OnDialogueWithStatueEnded);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Pickup/Pickup.Pickup_C:Used"), OnItemPickup);
	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Pickup/Pickup.Pickup_C:Remove"), OnItemPickupRemove);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Spirit/Spirit.Spirit_C:Used"), OnSpiritPickup);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Bounds/Void/Void_Gate.Void_Gate_C:Glow Eyes"), OnVoidGateCompleted);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/Player/Logic/FrameWork/Global_Controller.Global_Controller_C:LoadAllLevels"), OnLevelLoaded);
	BlueFireArchipelagoMod::hookManager->registerPreHook(STR("Function /Game/BlueFire/HUD/Dialog/MainDialogWB.MainDialogWB_C:WriteShopArrayToGI"), OnItemBought);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/Enemies/Master/BP_BossDoor_Queen.BP_BossDoor_Queen_C:Kill Goddess"), OnKillGoddess);

	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/Bounds/Checkpoint/CheckPoint.CheckPoint_C:Get Mana"), OnManaUpgrade);
}

// ============== Chest related methods ==============

bool LocationManager::OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
	// Get the short name (private name) and full name of the chest being opened
	const std::wstring chestName = Context->GetNamePrivate().ToString();
	const std::wstring chestFullName = Context->GetFullName();

	// Match the chest name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromChestName(chestName, chestFullName);
	if (!locationID.has_value())
	{
		logIncorrectMapping(chestName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Chest {} opened, marking location ID {} as checked in Archipelago\n"), chestName, locationID.value());
	AP_SendItem(locationID.value());


    return false;
}

std::optional<uint32_t> LocationManager::GetLocationIDFromChestName(const std::wstring& chestName, const std::wstring& chestFullName)
{
	// First try to find by private name
	const auto& chestMap = LocationsData::GetChestNameToLocationIDMap();
	auto it = chestMap.find(chestName);
	if (it != chestMap.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}

	// If not found by short name, try full name
	if (!chestFullName.empty())
	{
		const auto& chestFullMap = LocationsData::GetChestFullNameToLocationIDMap();
		auto fullIt = chestFullMap.find(chestFullName);
		if (fullIt != chestFullMap.end())
		{
			return fullIt->second + Archipelago::BF_BASE_ID;
		}
	}

	return std::nullopt;
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

    TArray<inventoryItem>* inventory = UnrealObjectQueries::GetNestedPropertyValue<TArray<inventoryItem>>(gameInstance.value(), L"PlayerStats", L"PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94");
    if (!inventory)
    {
        Output::send<LogLevel::Error>(STR("Could not get passive inventory in OnItemPickupRemove\n"));
        return false;
    }

    // Go through all items already in inventory
    for(int32_t i = 0; i < inventory->Num(); i++)
    {
        inventoryItem item = (*inventory)[i];

        // If the inventory item isn't of type "item"
        if(item.type != 0)
        {
            continue;
        }

        // If not the correct item
        if(item.item != 17)
        {
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

bool LocationManager::ProcessShopInventory(TArray<inventoryItem>* shopInventory, const wchar_t* shopName,
	std::function<std::map<uint8_t, uint32_t>::const_iterator(uint8_t, uint8_t, uint8_t)> inventoryLookup)
{
	if (!shopInventory)
		return true;

	std::map<uint8_t, uint32_t>::const_iterator itemIndex;

	for (int32_t i = 0; i < shopInventory->Num(); i++)
	{
		inventoryItem item = (*shopInventory)[i];

		if (item.type == 1 && item.weapon == 0) continue;
		if (item.type == 0 && item.item == 43) continue;

		itemIndex = inventoryLookup(item.type, item.item, item.spirit);
		if (itemIndex == std::map<uint8_t, uint32_t>::const_iterator())
		{
			Output::send<LogLevel::Error>(STR("Could not find the item in {}'s shop\n"), shopName);
			return false;
		}

		item.originalAmount = itemIndex->second;
		(*shopInventory)[i] = item;
	}

	return true;
}

bool LocationManager::OnLevelLoaded(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return false;
    }

	const wchar_t* shopNames[] = {L"Shop0-Mork", L"Shop1-Onrom", L"Shop2-SpiritHunter", L"Shop3-Ari", L"Shop4-Poti", L"Shop5-POI"};
	TArray<inventoryItem>* shops[6] = {};

	for (int i = 0; i < 6; i++)
	{
		shops[i] = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(shopNames[i]);
		if (!shops[i])
		{
			Output::send<LogLevel::Error>(STR("Could not find {} property\n"), shopNames[i]);
			return false;
		}
	}

	// Process each shop with its specific inventory lookup
	if (!ProcessShopInventory(shops[0], STR("Mork"), [](uint8_t type, uint8_t item, uint8_t spirit) {
		if (type == 0) return Shops::Mork::itemInventory.find(item);
		if (type == 3) return Shops::Mork::spiritInventory.find(spirit);
		return Shops::Mork::spiritInventory.end();
	})) return false;

	if (!ProcessShopInventory(shops[1], STR("Onrom"), [](uint8_t type, uint8_t item, uint8_t spirit) {
		return Shops::Onrom::inventory.find(item);
	})) return false;

	if (!ProcessShopInventory(shops[2], STR("Spirit Hunter"), [](uint8_t type, uint8_t item, uint8_t spirit) {
		return Shops::SpiritHunter::inventory.find(spirit);
	})) return false;

	if (!ProcessShopInventory(shops[3], STR("Ari"), [](uint8_t type, uint8_t item, uint8_t spirit) {
		return Shops::Ari::inventory.find(item);
	})) return false;

	if (!ProcessShopInventory(shops[4], STR("Poti"), [](uint8_t type, uint8_t item, uint8_t spirit) {
		if (type == 0) return Shops::Poti::itemInventory.find(item);
		if (type == 1) return Shops::Poti::weaponInventory.find(item);
		if (type == 3) return Shops::Poti::spiritInventory.find(spirit);
		return Shops::Poti::spiritInventory.end();
	})) return false;

	if (!ProcessShopInventory(shops[5], STR("POI"), [](uint8_t type, uint8_t item, uint8_t spirit) {
		return Shops::Poi::inventory.find(item);
	})) return false;

	for (TArray<inventoryItem>* shop : shops)
	{
		for (int32_t i = 0; i < shop->Num(); i++)
		{
			inventoryItem item = (*shop)[i];
			item.amount = 1;
			item.type = 1;
			item.weapon = 0;
			item.price = BlueFireArchipelagoMod::locationManager->GetItemPrice();
			(*shop)[i] = item;
		}
	}

    return false;
}

bool LocationManager::OnItemBought(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return false;
    }

	const wchar_t* shopNames[] = {L"Shop0-Mork", L"Shop1-Onrom", L"Shop2-SpiritHunter", L"Shop3-Ari", L"Shop4-Poti", L"Shop5-POI"};
	TArray<inventoryItem>* shops[6] = {};

	for (int i = 0; i < 6; i++)
	{
		shops[i] = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(shopNames[i]);
		if (!shops[i])
		{
			Output::send<LogLevel::Error>(STR("Could not find {} property\n"), shopNames[i]);
			return false;
		}
	}

	int32_t* WorldShop = Context->GetValuePtrByPropertyNameInChain<int32_t>(L"WorldShop");
	if(!WorldShop)
	{
        Output::send<LogLevel::Error>(STR("Could not find the WorldShop parameter of the MainDialogWB_C object\n"));
		return false;
	}

    TArray<inventoryItem>* ShopInventory = Context->GetValuePtrByPropertyNameInChain<TArray<inventoryItem>>(L"ShopInventory");
	if(!ShopInventory)
	{
        Output::send<LogLevel::Error>(STR("Could not find the ShopInventory parameter of the MainDialogWB_C object\n"));
		return false;
	}

	for(int32_t i = 0; i < (shops[*WorldShop])->Num(); i++)
    {
        inventoryItem globalItem = (*shops[*WorldShop])[i];
		bool match = false;

		for(int32_t j = 0; j < ShopInventory->Num(); j++)
		{
			if(globalItem.originalAmount == (*ShopInventory)[j].originalAmount)
			{
				match = true;
				break;
			}
		}

		if(!match)
		{
			std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromShopID(*WorldShop);
			if (!locationID.has_value())
			{
				logIncorrectMapping(std::to_wstring(*WorldShop));
				return false;
			}

			uint32_t archipelagoShopLocationID = locationID.value() + globalItem.originalAmount;
			Output::send<LogLevel::Verbose>(STR("Item bought in shop {}, marking location ID {} as checked in Archipelago\n"), *WorldShop, archipelagoShopLocationID);
			AP_SendItem(archipelagoShopLocationID);
		}
    }

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


// ============== Release methods ==============


bool LocationManager::OnKillGoddess(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
	if(!BlueFireArchipelagoMod::arcManager)
	{
		Output::send<LogLevel::Error>(STR("Could not find the Archipelago manager"));
		return false;
	}

	BlueFireArchipelagoMod::arcManager->ReleaseWorld();

	return false;
}


// ============== Mana Upgrade methods ==============


bool LocationManager::OnManaUpgrade(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return false;
    }

    uint32_t* manaUpgradeLevel = UnrealObjectQueries::GetNestedPropertyValue<uint32_t>(gameInstance.value(), L"PlayerStats", L"Levels_85_3996A743428F6DEC04E741B8F0E2B7C4");
    if (!manaUpgradeLevel)
    {
        Output::send<LogLevel::Error>(STR("Could not get mana upgrade level in OnManaUpgrade\n"));
        return false;
    }

    float* maxStamina = UnrealObjectQueries::GetNestedPropertyValue<float>(gameInstance.value(), L"PlayerStats", L"MaxStamina_8_A0DAE7564A0F01238D8F6BBC0D00D733");
    if (!maxStamina)
    {
        Output::send<LogLevel::Error>(STR("Could not get max stamina in OnManaUpgrade\n"));
        return false;
    }

    float* currentStamina = UnrealObjectQueries::GetNestedPropertyValue<float>(gameInstance.value(), L"PlayerStats", L"Stamina_13_63486BD942500C84BEF7BDB51839CDCF");
    if (!currentStamina)
    {
        Output::send<LogLevel::Error>(STR("Could not get current stamina in OnManaUpgrade\n"));
        return false;
    }


	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetBaseLocationIDForManaUpgrade(*manaUpgradeLevel);
	if (!locationID.has_value())
	{
		Output::send<LogLevel::Error>(STR("Could not find the location ID for the mana upgrade"));
		return false;
	}

	uint32_t archipelagoManaLocationID = locationID.value();
	Output::send<LogLevel::Verbose>(STR("Mana upgrade level {} obtained, marking location ID {} as checked in Archipelago\n"), *manaUpgradeLevel, archipelagoManaLocationID);
	AP_SendItem(archipelagoManaLocationID);

	// Set stamina back to its previous level
	*maxStamina = *maxStamina - 11.0f;
	*currentStamina = *maxStamina;

	return false;
}

std::optional<uint32_t> LocationManager::GetBaseLocationIDForManaUpgrade(uint32_t manaUpgradeID)
{

	const auto& manaUpgradeMap = LocationsData::GetManaUpgradeIDToLocationIDMap();
	auto it = manaUpgradeMap.find(manaUpgradeID);
	if (it != manaUpgradeMap.end())
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
	Output::send<LogLevel::Error>(STR("Hi ! It looks like the location you just checked was not correctly registered in the location mapping in	LocationsData.hpp."));
	Output::send<LogLevel::Error>(STR("This mod is still very new, and some chests or other locations might be missing."));
	Output::send<LogLevel::Error>(STR("Please open an issue on Github or contact nebulea__ on discord to get this added."));
	Output::send<LogLevel::Error>(STR("Be sure to include a description of where you found this location, and include the following location name in your message :"));
	Output::send<LogLevel::Error>(STR("{}"), locationName);
}