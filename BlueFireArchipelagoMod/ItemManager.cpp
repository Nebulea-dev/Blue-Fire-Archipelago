#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include <Helper/ObjectFinder.hpp>
#include <Helper/HookHelper.hpp>
#include <Helper/UnrealObjectQueries.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <ArchipelagoModConfig.hpp>
#include <ItemManager.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

ItemManager::ItemManager()
{
	Output::send<LogLevel::Verbose>(STR("ItemManager instance created\n"));

    itemName = new FText(Strings::ITEM_NAME);
    itemDescription = new FText(Strings::ITEM_DESCRIPTION);

    // Register the ItemManager hook
    BlueFireArchipelagoMod::hookManager->registerPreHook(Hooks::PLAY_NEW_ITEM, [](UObject* Context, FFrame& Stack, void* RESULT_DECL) {
        if (BlueFireArchipelagoMod::itemManager)
        {
            return BlueFireArchipelagoMod::itemManager->PlayNewItemPreHook(Context, Stack, RESULT_DECL);
        }
        return false;
    });
}

void ItemManager::itemReceiveCb(int itemID, bool notify)
{
    int rebasedItemID = itemID - Archipelago::BF_BASE_ID;
    if(rebasedItemID < 0)
    {
        Output::send<LogLevel::Error>(STR("Item has an ID under the base ID of the game, cannot be mapped to a valid item\n"));
    }

    uint16_t itemCategory = (rebasedItemID - (rebasedItemID % 100)) / 100;

    switch(itemCategory)
    {
        case 0:
            givePlayerEmote(rebasedItemID);
            break;

        case 1:
            givePlayerWeapon(rebasedItemID - 100);
            break;

        case 2:
            givePlayerTunic(rebasedItemID - 200);
            break;

        case 3:
            givePlayerSpirit(rebasedItemID - 300);
            break;

        case 4:
            givePlayerAbility(rebasedItemID - 400);
            break;

        case 5:
            givePlayerItem(rebasedItemID - 500);
            break;

        case 6:
            givePlayerPassiveItem(rebasedItemID - 600);
            break;

        default:
            Output::send<LogLevel::Error>(STR("Could not map item to a valid category of item\n"));
            break;
    }
}

bool ItemManager::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    // Currently causes a crash for big blue chests (Chest_Master_Child_C) , no idea why.
    HookHelper::setParamValue<FText>(PropertyNames::PARAM_IN_TEXT, Stack, itemName);
    HookHelper::setParamValue<FText>(PropertyNames::PARAM_DESCRIPTION, Stack, itemDescription);
    HookHelper::setParamValue<uint8_t>(PropertyNames::PARAM_KEY_ITEM, Stack, UI::KEY_ITEM_TYPE);
    BlueFireArchipelagoMod::hookManager->setParamValue<uint32_t>(PropertyNames::PARAM_AMOUNT, Stack, UI::ITEM_AMOUNT);

    // Do not prevent the original function from being called
    return false;
}

void ItemManager::givePlayerEmote(int emoteID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player emote ID: {}\n"), emoteID);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();

    TArray<uint8_t>* emoteInventory = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<uint8_t>>(L"Emotes");
	if(!emoteInventory)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Emotes parameter of the game instance\n"));
		return;
	}

    emoteInventory->Push(emoteID);
}

void ItemManager::givePlayerWeapon(int weaponID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player weapon ID: {}\n"), weaponID);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();

    // Get the "PlayerEquipment" property
    FStructProperty* playerEquipmentProperty = static_cast<FStructProperty*>(gameInstance.value()->GetPropertyByNameInChain(L"PlayerEquipment"));
    if (!playerEquipmentProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PlayerEquipment property\n"));
        return;
    }

    // Get the "PlayerEquipment.Weapons_18_409D783242E4CBDA66AAB6A252C7A317" property
    auto playerEquipmentStruct = playerEquipmentProperty->GetStruct();
    auto playerEquipment = playerEquipmentProperty->ContainerPtrToValuePtr<void>(gameInstance.value());
    FStructProperty* weaponProperty = static_cast<FStructProperty*>(playerEquipmentStruct->GetPropertyByNameInChain(L"Weapons_18_409D783242E4CBDA66AAB6A252C7A317"));
    if (!weaponProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find Weapons_18_409D783242E4CBDA66AAB6A252C7A317 property in PlayerEquipment\n"));
        return;
    }

    TArray<uint8_t>* weapons = weaponProperty->ContainerPtrToValuePtr<TArray<uint8_t>>(playerEquipment);

    weapons->Push(weaponID);
}

void ItemManager::givePlayerTunic(int tunicID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player tunic ID: {}\n"), tunicID);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();

    // Get the "PlayerEquipment" property
    FStructProperty* playerEquipmentProperty = static_cast<FStructProperty*>(gameInstance.value()->GetPropertyByNameInChain(L"PlayerEquipment"));
    if (!playerEquipmentProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PlayerEquipment property\n"));
        return;
    }

    // Get the "PlayerEquipment.Tunics_19_8878CF744AF2806994F2E48778F1CC2D" property
    auto playerEquipmentStruct = playerEquipmentProperty->GetStruct();
    auto playerEquipment = playerEquipmentProperty->ContainerPtrToValuePtr<void>(gameInstance.value());
    FStructProperty* tunicProperty = static_cast<FStructProperty*>(playerEquipmentStruct->GetPropertyByNameInChain(L"Tunics_19_8878CF744AF2806994F2E48778F1CC2D"));
    if (!tunicProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find Tunics_19_8878CF744AF2806994F2E48778F1CC2D property in PlayerEquipment\n"));
        return;
    }

    TArray<uint8_t>* tunics = tunicProperty->ContainerPtrToValuePtr<TArray<uint8_t>>(playerEquipment);

    tunics->Push(tunicID);
}

void ItemManager::givePlayerSpirit(int spiritID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player spirit ID: {}\n"), spiritID);
    // TODO: Implement spirit giving logic
}


void ItemManager::givePlayerAbility(int abilityID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player ability ID: {}\n"), abilityID);

    wchar_t propertyName[50];
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();

    // Get the "PlayerAbilities" property
    FStructProperty* playerAbilitiesProperty = static_cast<FStructProperty*>(gameInstance.value()->GetPropertyByNameInChain(L"PlayerAbilities"));
    if (!playerAbilitiesProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PlayerAbilities property\n"));
        return;
    }


    switch(abilityID)
    {
        case 0:
            wcsncpy_s(propertyName, 50, L"Attack_10_351804CD4B3F2EFBDC0B2DAAA7ED7238", 50);
            break;
        case 1:
            wcsncpy_s(propertyName, 50, L"Dash_8_C5BEBFCD4803BE8A33ADC7BB805F1659", 50);
            break;
        case 2:
            wcsncpy_s(propertyName, 50, L"DoubleJump_9_9ACF69B4474D76AACA0E349806254782", 50);
            break;
        case 3:
            wcsncpy_s(propertyName, 50, L"WallJump_12_8CC261B848F97BE432C43FBFDFB65D1D", 50);
            break;
        case 4:
            wcsncpy_s(propertyName, 50, L"Sprint_21_A2EA9CA54248830C70D2A096307CA144", 50);
            break;
        case 5:
            wcsncpy_s(propertyName, 50, L"DownSmash_22_84DE6230457D45C1BBF111BBA6DDE737", 50);
            break;
        case 6:
            wcsncpy_s(propertyName, 50, L"Spell_23_EFD583FD46ED9B47C8C80EBEEB3D9753", 50);
            break;
        case 7:
            wcsncpy_s(propertyName, 50, L"Grind_19_5D0328FB486C70BF86BFD58EAB4CE52D", 50);
            break;
        case 8:
            wcsncpy_s(propertyName, 50, L"Block_25_5710D9FB4D2A4FF88972508279869DF4", 50);
            break;
        case 9:
            wcsncpy_s(propertyName, 50, L"SpinAttack_27_19AE29114077C361BA4934AD401C4A0B", 50);
            break;
    }


    auto playerAbilitiesStruct = playerAbilitiesProperty->GetStruct();
    auto playerAbilities = playerAbilitiesProperty->ContainerPtrToValuePtr<void>(gameInstance.value());
    FStructProperty* abilityProperty = static_cast<FStructProperty*>(playerAbilitiesStruct->GetPropertyByNameInChain(propertyName));
    if (!abilityProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find property in PlayerAbilities\n"));
        return;
    }

    uint8_t* ability = abilityProperty->ContainerPtrToValuePtr<uint8_t>(playerAbilities);

    *ability = true;
}

void ItemManager::givePlayerItem(int itemID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player item ID: {}\n"), itemID);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();

    // Get the "PlayerStats" property
    FStructProperty* playerStatsProperty = static_cast<FStructProperty*>(gameInstance.value()->GetPropertyByNameInChain(L"PlayerStats"));
    if (!playerStatsProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PlayerStats property\n"));
        return;
    }

    // Get the "PlayerEquipment.Inventory_23_288399C5416269F828550FB7376E7942" property
    auto playerStatsStruct = playerStatsProperty->GetStruct();
    auto playerStats = playerStatsProperty->ContainerPtrToValuePtr<void>(gameInstance.value());
    FStructProperty* inventoryProperty = static_cast<FStructProperty*>(playerStatsStruct->GetPropertyByNameInChain(L"Inventory_23_288399C5416269F828550FB7376E7942"));
    if (!inventoryProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find Inventory_23_288399C5416269F828550FB7376E7942 property in PlayerStats\n"));
        return;
    }

    TArray<inventoryItem>* inventory = inventoryProperty->ContainerPtrToValuePtr<TArray<inventoryItem>>(playerStats);

    // Go through all items already in inventory, to stack it instead of creating a new item if possible
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
        if(item.item != itemID)
        {
            Output::send<LogLevel::Verbose>(STR("Item is of id {}, skipping\n"), item.item);
            continue;
        }

        Output::send<LogLevel::Verbose>(STR("Found item in inventory, increasing quantity ..\n"), itemID);

        item.amount += 1;
        (*inventory)[i] = item;

        return;
    }

    Output::send<LogLevel::Verbose>(STR("Item not found in inventory, creating it ..\n"), itemID);

    // If item is not already in inventory, add it
    inventoryItem newItem = {};
    newItem.item = itemID;
    newItem.amount = 1;
    inventory->Push(newItem);
}

void ItemManager::givePlayerPassiveItem(int itemID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player passive item ID: {}\n"), itemID);

        std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();

    // Get the "PlayerStats" property
    FStructProperty* playerStatsProperty = static_cast<FStructProperty*>(gameInstance.value()->GetPropertyByNameInChain(L"PlayerStats"));
    if (!playerStatsProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find PlayerStats property\n"));
        return;
    }

    // Get the "PlayerEquipment.PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94" property
    auto playerStatsStruct = playerStatsProperty->GetStruct();
    auto playerStats = playerStatsProperty->ContainerPtrToValuePtr<void>(gameInstance.value());
    FStructProperty* inventoryProperty = static_cast<FStructProperty*>(playerStatsStruct->GetPropertyByNameInChain(L"PassiveInventory_48_636C916F4A37F051CF9B14A1402B4C94"));
    if (!inventoryProperty)
    {
        Output::send<LogLevel::Error>(STR("Could not find Tunics_19_8878CF744AF2806994F2E48778F1CC2D property in PlayerStats\n"));
        return;
    }

    TArray<inventoryItem>* inventory = inventoryProperty->ContainerPtrToValuePtr<TArray<inventoryItem>>(playerStats);

    // Go through all items already in inventory, to stack it instead of creating a new item if possible
    for(int32_t i = 0; i < inventory->Num(); i++)
    {
        inventoryItem item = (*inventory)[i];

        // If the inventory item isn't of type "item"
        if(item.type != 0)
        {
            continue;
        }

        // If not the correct item
        if(item.item != itemID)
        {
            continue;
        }

        item.amount += 1;
        (*inventory)[i] = item;

        return;
    }

    // If item is not already in inventory, add it
    inventoryItem newItem = {};
    newItem.item = itemID;
    newItem.amount = 1;
    inventory->Push(newItem);
}