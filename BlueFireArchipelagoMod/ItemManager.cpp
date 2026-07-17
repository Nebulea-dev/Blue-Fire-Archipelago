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

    // Register the ItemManager hook
    BlueFireArchipelagoMod::hookManager->registerPreHook(Hooks::PLAY_NEW_ITEM, [](UObject* Context, FFrame& Stack, void* RESULT_DECL) {
        if (BlueFireArchipelagoMod::itemManager)
        {
            return BlueFireArchipelagoMod::itemManager->PlayNewItemPreHook(Context, Stack, RESULT_DECL);
        }
        return false;
    });
	BlueFireArchipelagoMod::hookManager->registerPostHook(STR("Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:StartNewGame"), StartNewGame);
}

void ItemManager::itemReceiveCb(int itemID)
{
    if(itemID < 0)
    {
        Output::send<LogLevel::Error>(STR("Item has an ID under the base ID of the game, cannot be mapped to a valid item\n"));
    }

    uint16_t itemCategory = (itemID - (itemID % 100)) / 100;

    switch(itemCategory)
    {
        case 0:
            givePlayerEmote(itemID);
            break;

        case 1:
            givePlayerWeapon(itemID - 100);
            break;

        case 2:
            givePlayerTunic(itemID - 200);
            break;

        case 3:
            givePlayerSpirit(itemID - 300);
            break;

        case 4:
            givePlayerAbility(itemID - 400);
            break;

        case 5:
            givePlayerItem(itemID - 500);
            break;

        case 6:
            givePlayerKeyItem(itemID - 600);
            break;

        case 7:
            givePlayerCustomItem(itemID - 700);
            break;

        default:
            Output::send<LogLevel::Error>(STR("Could not map item to a valid category of item\n"));
            break;
    }
}

bool ItemManager::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    if (!BlueFireArchipelagoMod::hookManager)
    {
        Output::send<LogLevel::Error>(STR("hookManager is null in PlayNewItemPreHook\n"));
        return false;
    }

    // TODO : Fix leak
    FText* itemName = new FText(Strings::ITEM_NAME);
    FText* itemDescription = new FText(Strings::ITEM_DESCRIPTION);
    uint8_t* keyItem = new uint8_t(UI::KEY_ITEM_TYPE);
    uint32_t* itemAmount = new uint32_t(UI::ITEM_AMOUNT);

    BlueFireArchipelagoMod::hookManager->setParamValue<FText>(PropertyNames::PARAM_IN_TEXT, Stack, itemName);
    BlueFireArchipelagoMod::hookManager->setParamValue<FText>(PropertyNames::PARAM_DESCRIPTION, Stack, itemDescription);
    BlueFireArchipelagoMod::hookManager->setParamValue<uint8_t>(PropertyNames::PARAM_KEY_ITEM, Stack, keyItem);
    BlueFireArchipelagoMod::hookManager->setParamValue<uint32_t>(PropertyNames::PARAM_AMOUNT, Stack, itemAmount);

    // Do not prevent the original function from being called
    return false;
}

bool ItemManager::StartNewGame(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return false;
    }

    TArray<uint8_t>* emoteInventory = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<uint8_t>>(L"Emotes");
	if(!emoteInventory)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Emotes parameter of the game instance\n"));
		return false;
	}

    emoteInventory->Pop(true);

    int32* currency = UnrealObjectQueries::GetNestedPropertyValue<int32>(gameInstance.value(), L"PlayerStats", L"Currency_10_C5BEBFCD4803BE8A33ADC7BB805F1659");
    if (!currency)
    {
        Output::send<LogLevel::Error>(STR("Could not get currency value in StartNewGame\n"));
        return false;
    }

    *currency = 200;

    return false;
}

void ItemManager::givePlayerEmote(int emoteID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player emote ID: {}\n"), emoteID);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return;
    }

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
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return;
    }

    TArray<uint8_t>* weapons = UnrealObjectQueries::GetNestedPropertyValue<TArray<uint8_t>>(gameInstance.value(), L"PlayerEquipment", L"Weapons_18_409D783242E4CBDA66AAB6A252C7A317");
    if (!weapons)
    {
        Output::send<LogLevel::Error>(STR("Could not get weapons array in givePlayerWeapon\n"));
        return;
    }

    weapons->Push(weaponID);
}

void ItemManager::givePlayerTunic(int tunicID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player tunic ID: {}\n"), tunicID);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return;
    }

    TArray<uint8_t>* tunics = UnrealObjectQueries::GetNestedPropertyValue<TArray<uint8_t>>(gameInstance.value(), L"PlayerEquipment", L"Tunics_19_8878CF744AF2806994F2E48778F1CC2D");
    if (!tunics)
    {
        Output::send<LogLevel::Error>(STR("Could not get tunics array in givePlayerTunic\n"));
        return;
    }

    tunics->Push(tunicID);
}

void ItemManager::givePlayerSpirit(int spiritID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player spirit ID: {}\n"), spiritID);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return;
    }

    TArray<uint8_t>* spirits = UnrealObjectQueries::GetNestedPropertyValue<TArray<uint8_t>>(gameInstance.value(), L"PlayerEquipment", L"SpecialEffects_6_F506303E4AEAD142AFC632B92A252F0A");
    if (!spirits)
    {
        Output::send<LogLevel::Error>(STR("Could not get spirits array in givePlayerSpirit\n"));
        return;
    }

    spirits->Push(spiritID);
}


void ItemManager::givePlayerAbility(int abilityID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player ability ID: {}\n"), abilityID);

    const wchar_t* abilityProperties[] = {
        L"Attack_10_351804CD4B3F2EFBDC0B2DAAA7ED7238",
        L"Dash_8_C5BEBFCD4803BE8A33ADC7BB805F1659",
        L"DoubleJump_9_9ACF69B4474D76AACA0E349806254782",
        L"WallJump_12_8CC261B848F97BE432C43FBFDFB65D1D",
        L"Sprint_21_A2EA9CA54248830C70D2A096307CA144",
        L"DownSmash_22_84DE6230457D45C1BBF111BBA6DDE737", // (NOTE: DownSmash ability does not actually do anything in-game)
        L"Spell_23_EFD583FD46ED9B47C8C80EBEEB3D9753",
        L"Grind_19_5D0328FB486C70BF86BFD58EAB4CE52D",     // (NOTE: Grind ability does not actually do anything in-game)
        L"Block_25_5710D9FB4D2A4FF88972508279869DF4",
        L"SpinAttack_27_19AE29114077C361BA4934AD401C4A0B",
    };

    const int32_t abilityPassiveItems[] = {
        -1,   // Attack -> no passive item
        -1,   // Dash -> no passive item
        47,   // DoubleJump -> DoubleJump passive item
        49,   // WallRun -> WallRun passive item
        69,   // Sprint -> Sprint passive item
        -1,   // DownSmash -> no passive item
        50,   // Spell -> FireBall passive item
        -1,   // Grind -> no passive item
        52,   // Block -> Shield passive item
        48,   // SpinAttack -> SpinAttack passive item
    };

    if (abilityID < 0 || abilityID >= 10)
    {
        Output::send<LogLevel::Error>(STR("Invalid ability ID: {}\n"), abilityID);
        return;
    }

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return;
    }

    uint8_t* ability = UnrealObjectQueries::GetNestedPropertyValue<uint8_t>(gameInstance.value(), L"PlayerAbilities", abilityProperties[abilityID]);
    if (!ability)
        return;

    *ability = true;

    // Also add the corresponding passive item
    int32_t passiveItemID = abilityPassiveItems[abilityID];
    if (passiveItemID >= 0)
    {
        TArray<inventoryItem>* passiveInventory = UnrealObjectQueries::GetPassiveInventoryFromGameInstance();
        if (passiveInventory)
        {
            if (!UnrealObjectQueries::StackItemInInventory(passiveInventory, passiveItemID))
            {
                inventoryItem newItem = {};
                newItem.item = passiveItemID;
                newItem.amount = 1;
                passiveInventory->Push(newItem);
                Output::send<LogLevel::Verbose>(STR("Added passive item ID {} for ability {}\n"), passiveItemID, abilityID);
            }
        }
    }
}

void ItemManager::givePlayerItem(int itemID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player item ID: {}\n"), itemID);

    TArray<inventoryItem>* inventory = UnrealObjectQueries::GetInventoryFromGameInstance();
    if (!inventory)
    {
        Output::send<LogLevel::Error>(STR("Could not get inventory in givePlayerItem\n"));
        return;
    }

    if (UnrealObjectQueries::StackItemInInventory(inventory, itemID))
    {
        Output::send<LogLevel::Verbose>(STR("Found item in inventory, increasing quantity\n"));
        return;
    }

    Output::send<LogLevel::Verbose>(STR("Item not found in inventory, creating it\n"));
    inventoryItem newItem = {};
    newItem.item = itemID;
    newItem.amount = 1;
    inventory->Push(newItem);
}

void ItemManager::givePlayerKeyItem(int itemID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player passive item ID: {}\n"), itemID);

    TArray<inventoryItem>* inventory = UnrealObjectQueries::GetPassiveInventoryFromGameInstance();
    if (!inventory)
    {
        Output::send<LogLevel::Error>(STR("Could not get passive inventory in givePlayerKeyItem\n"));
        return;
    }

    if (UnrealObjectQueries::StackItemInInventory(inventory, itemID))
        return;

    inventoryItem newItem = {};
    newItem.item = itemID;
    newItem.amount = 1;
    inventory->Push(newItem);
}

void ItemManager::givePlayerCustomItem(int itemID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player custom item ID: {}\n"), itemID);

    // Progressive Pouch (itemID = 0)
    if(itemID == 0)
    {
        givePlayerProgressivePouch();
    }
    // Progressive Weapon (itemID = 1)
    else if(itemID == 1)
    {
        givePlayerProgressiveWeapon();
    }
    // Fire Essence Slot (itemID = 2)
    else if(itemID == 2)
    {
        // TODO : Add also the essence slot to the player instance

        TArray<inventoryItem>* passiveInventory = UnrealObjectQueries::GetPassiveInventoryFromGameInstance();
        if (passiveInventory)
        {
            const uint32_t FIRE_ESSENCE_SLOT = 73;
            if (!UnrealObjectQueries::StackItemInInventory(passiveInventory, FIRE_ESSENCE_SLOT))
            {
                inventoryItem newItem = {};
                newItem.item = FIRE_ESSENCE_SLOT;
                newItem.amount = 1;
                passiveInventory->Push(newItem);
                Output::send<LogLevel::Verbose>(STR("Added Fire Essence Slot to passive inventory\n"));
            }
        }
    }
    // Spirit Slot (itemID = 3)
    else if(itemID == 3)
    {
        // TODO : Add also the spirit slot to the player instance

        TArray<inventoryItem>* passiveInventory = UnrealObjectQueries::GetPassiveInventoryFromGameInstance();
        if (passiveInventory)
        {
            const uint32_t SPIRIT_SLOT = 53;
            if (!UnrealObjectQueries::StackItemInInventory(passiveInventory, SPIRIT_SLOT))
            {
                inventoryItem newItem = {};
                newItem.item = SPIRIT_SLOT;
                newItem.amount = 1;
                passiveInventory->Push(newItem);
                Output::send<LogLevel::Verbose>(STR("Added Spirit Slot to passive inventory\n"));
            }
        }
    }
    else
    {
        Output::send<LogLevel::Error>(STR("Unknown custom item ID: {}\n"), itemID);
    }
}

void ItemManager::givePlayerProgressivePouch()
{
    Output::send<LogLevel::Verbose>(STR("Upgrading player pouch...\n"));

    TArray<inventoryItem>* inventory = UnrealObjectQueries::GetPassiveInventoryFromGameInstance();
    if (!inventory)
        return;

    const uint32_t BASIC_POUCH = 72;
    const uint32_t LARGE_POUCH = 0;
    const uint32_t EXTRA_LARGE_POUCH = 75;

    for(int32_t i = 0; i < inventory->Num(); i++)
    {
        inventoryItem item = (*inventory)[i];
        if(item.type != 0) continue;

        if(item.item == BASIC_POUCH)
        {
            Output::send<LogLevel::Verbose>(STR("Found Basic Pouch, upgrading to Large Pouch\n"));
            item.item = LARGE_POUCH;
            (*inventory)[i] = item;
            return;
        }
        if(item.item == LARGE_POUCH)
        {
            Output::send<LogLevel::Verbose>(STR("Found Large Pouch, upgrading to Extra Large Pouch\n"));
            item.item = EXTRA_LARGE_POUCH;
            (*inventory)[i] = item;
            return;
        }
    }

    Output::send<LogLevel::Error>(STR("No Pouch found in inventory, could not replace with better pouch\n"));
}

void ItemManager::givePlayerProgressiveWeapon()
{
    Output::send<LogLevel::Verbose>(STR("Upgrading player weapon...\n"));

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return;
    }

    TArray<uint8_t>* weapons = UnrealObjectQueries::GetNestedPropertyValue<TArray<uint8_t>>(gameInstance.value(), L"PlayerEquipment", L"Weapons_18_409D783242E4CBDA66AAB6A252C7A317");
    if (!weapons)
    {
        Output::send<LogLevel::Error>(STR("Could not get weapons array in givePlayerProgressiveWeapon\n"));
        return;
    }

    uint8_t highestWeaponOrderIndex = 0;
    for(int32_t i = 0; i < weapons->Num(); i++)
    {
        uint8_t weaponID = (*weapons)[i];
        uint8_t weaponOrderIndex = ArchipelagoModConfig::Weapons::orderFromWeapon.find(weaponID) != ArchipelagoModConfig::Weapons::orderFromWeapon.end() ? ArchipelagoModConfig::Weapons::orderFromWeapon.at(weaponID) : 0;
        Output::send<LogLevel::Verbose>(STR("Found weapon ID: {}, order index: {}\n"), weaponID, weaponOrderIndex);
        if(weaponOrderIndex > highestWeaponOrderIndex)
            highestWeaponOrderIndex = weaponOrderIndex;
    }

    uint8_t nextWeaponIndex = highestWeaponOrderIndex + 1;

    if(nextWeaponIndex < ArchipelagoModConfig::Weapons::weaponFromOrder.size() + 1)
    {
        uint8_t nextWeaponID = ArchipelagoModConfig::Weapons::weaponFromOrder.at(nextWeaponIndex);
        Output::send<LogLevel::Verbose>(STR("Found highest weapon order index: {}, adding weapon ID: {}\n"), highestWeaponOrderIndex, nextWeaponID);
        weapons->Push(nextWeaponID);
    }
    else
    {
        Output::send<LogLevel::Verbose>(STR("Cannot find the next weapon for order index: {}, max weapon already obtained\n"), highestWeaponOrderIndex);
    }
}