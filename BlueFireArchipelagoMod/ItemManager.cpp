#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include <Archipelago.h>

#include <Helper/ObjectFinder.hpp>
#include <Helper/HookHelper.hpp>
#include <Helper/UnrealObjectQueries.hpp>
#include <ArchipelagoManager.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <ArchipelagoModConfig.hpp>
#include <ItemManager.hpp>

#include <fstream>
#include <filesystem>
#include <json/json.h>
#include <windows.h>

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
    Output::send<LogLevel::Verbose>(STR("Giving item with ID {}\n"), itemID);

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
            givePlayerImportantItem(itemID - 600);
            break;

        case 7:
            givePlayerKeyItem(itemID - 700);
            break;

        case 8:
            givePlayerCustomItem(itemID - 800);
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
    uint8_t* importantItem = new uint8_t(UI::IMPORTANT_ITEM_TYPE);
    uint32_t* itemAmount = new uint32_t(UI::ITEM_AMOUNT);

    BlueFireArchipelagoMod::hookManager->setParamValue<FText>(PropertyNames::PARAM_IN_TEXT, Stack, itemName);
    BlueFireArchipelagoMod::hookManager->setParamValue<FText>(PropertyNames::PARAM_DESCRIPTION, Stack, itemDescription);
    BlueFireArchipelagoMod::hookManager->setParamValue<uint8_t>(PropertyNames::PARAM_IMPORTANT_ITEM, Stack, importantItem);
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

    // Remove the initial emote to randomize it
    emoteInventory->Pop(true);

    // The price of the emote is free if there's exactly 1 emote in the inventory
    // Therefore there must always be at least 2 emotes, which initially are the empty emote (ID 14)
    emoteInventory->Push(14);
    emoteInventory->Push(14);

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

    // The price of the emote is free if there's exactly 1 emote in the inventory
    // Therefore there must always be at least 2 emotes, which initially are the empty emote (ID 14)

    // If both empty emotes are still there
    if(emoteInventory->Num() >=2 && (*emoteInventory)[0] == 14)
    {
        (*emoteInventory)[0] = emoteID;
        return;
    }
    if(emoteInventory->Num() >=2 && (*emoteInventory)[1] == 14)
    {
        (*emoteInventory)[1] = emoteID;
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

void ItemManager::givePlayerImportantItem(int itemID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player passive item ID: {}\n"), itemID);

    TArray<inventoryItem>* inventory = UnrealObjectQueries::GetPassiveInventoryFromGameInstance();
    if (!inventory)
    {
        Output::send<LogLevel::Error>(STR("Could not get passive inventory in givePlayerImportantItem\n"));
        return;
    }

    if (UnrealObjectQueries::StackItemInInventory(inventory, itemID))
        return;

    inventoryItem newItem = {};
    newItem.item = itemID;
    newItem.amount = 1;
    inventory->Push(newItem);
}

void ItemManager::givePlayerKeyItem(int itemID)
{
    Output::send<LogLevel::Verbose>(STR("Giving player key item ID: {}\n"), itemID);

    uint32_t inGameItemID = 0;

    switch(itemID)
    {
        // Old Keys (indices 0-7)
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            inGameItemID = 2;
            break;
        // Holy Keys (indices 8-10)
        case 8: case 9: case 10:
            inGameItemID = 57;
            break;
        case 11:  // Uthas Temple Key
            inGameItemID = 60;
            break;
        case 12:  // Temple of Gods Key
            inGameItemID = 61;
            break;
        case 13:  // Steam Key
            inGameItemID = 62;
            break;
        case 14:  // Key of Ember
            inGameItemID = 56;
            break;
        case 15:  // Graveyard Key
            inGameItemID = 63;
            break;
        default:
            Output::send<LogLevel::Error>(STR("Unknown key item ID: {}\n"), itemID);
            return;
    }

    TArray<inventoryItem>* inventory = UnrealObjectQueries::GetPassiveInventoryFromGameInstance();
    if (!inventory)
    {
        Output::send<LogLevel::Error>(STR("Could not get passive inventory in givePlayerKeyItem\n"));
        return;
    }

    if (UnrealObjectQueries::StackItemInInventory(inventory, inGameItemID))
        return;

    inventoryItem newItem = {};
    newItem.item = inGameItemID;
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
        std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
        if (!gameInstance.has_value())
        {
            Output::send<LogLevel::Error>(STR("Could not find game instance\n"));
            return;
        }

        uint32_t* FireOrbs = UnrealObjectQueries::GetNestedPropertyValue<uint32_t>(gameInstance.value(), L"PlayerStats", L"FireOrbs_31_254F6FA346D4ECAF6F2E4B86D1E9BAFC");
        if (!FireOrbs)
        {
            Output::send<LogLevel::Error>(STR("Could not get fire orbs from game instance\n"));
            return;
        }
        (*FireOrbs)++;

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
        std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
        if (!gameInstance.has_value())
        {
            Output::send<LogLevel::Error>(STR("Could not find game instance\n"));
            return;
        }

        uint32_t* spiritSlots = UnrealObjectQueries::GetNestedPropertyValue<uint32_t>(gameInstance.value(), L"PlayerStats", L"SpiritSlots_55_CB306EB747E6D8A63CE10DBF4E9B69B5");
        if (!spiritSlots)
        {
            Output::send<LogLevel::Error>(STR("Could not get spirit slots from game instance\n"));
            return;
        }
        (*spiritSlots)++;

        // This code gives the spirit slot item to the passive inventory
        // However, this doesn't exist in the real game, and might just be a bit ugly

        /*
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
        */
    }
    // x1000 Currency (itemID = 4)
    else if(itemID == 4)
    {
        givePlayerCurrency(1000);
    }
    // x2000 Currency (itemID = 5)
    else if(itemID == 5)
    {
        givePlayerCurrency(2000);
    }
    // x3000 Currency (itemID = 6)
    else if(itemID == 6)
    {
        givePlayerCurrency(3000);
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

void ItemManager::givePlayerCurrency(int32_t amount)
{
    Output::send<LogLevel::Verbose>(STR("Adding {} currency to player...\n"), amount);

    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if(!gameInstance.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
        return;
    }

    int32* currency = UnrealObjectQueries::GetNestedPropertyValue<int32>(gameInstance.value(), L"PlayerStats", L"Currency_10_C5BEBFCD4803BE8A33ADC7BB805F1659");
    if (!currency)
    {
        Output::send<LogLevel::Error>(STR("Could not get currency value in givePlayerCurrency\n"));
        return;
    }

    *currency += amount;
    Output::send<LogLevel::Verbose>(STR("Updated currency to: {}\n"), *currency);
}

std::string ReceivedItemQueue::getReceivedItemQueueFilePath()
{
    wchar_t dllPath[MAX_PATH] = {0};
    HMODULE moduleHandle = GetModuleHandle(nullptr);
    if (!moduleHandle)
    {
        Output::send<LogLevel::Error>(STR("Could not get module handle for DLL path\n"));
        return "";
    }

    GetModuleFileNameW(moduleHandle, dllPath, MAX_PATH);
    std::filesystem::path modPath = std::filesystem::path(dllPath).parent_path();

    int32_t saveFileIndex = 0;
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if (gameInstance.has_value())
    {
        int32_t* saveIndex = UnrealObjectQueries::GetNestedPropertyValue<int32_t>(gameInstance.value(), L"System", L"SaveFileIndex_7_9ACF69B4474D76AACA0E349806254782");
        if (saveIndex)
        {
            saveFileIndex = *saveIndex;
        }
    }

    std::string filename = "received_item_queue_" + std::to_string(saveFileIndex) + ".json";
    std::string queuePath = (modPath / filename).string();

    Output::send<LogLevel::Verbose>(STR("Received item queue file path set for save index {}\n"), saveFileIndex);
    return queuePath;
}

bool ReceivedItemQueue::receivedItemQueueFileExists()
{
    std::string filePath = getReceivedItemQueueFilePath();
    if (filePath.empty())
        return false;

    return std::filesystem::exists(filePath);
}

void ReceivedItemQueue::appendReceivedItem(int itemID)
{
    std::string filePath = getReceivedItemQueueFilePath();
    if (filePath.empty())
    {
        Output::send<LogLevel::Error>(STR("Could not determine queue file path\n"));
        return;
    }

    try
    {
        Json::Value root;
        Json::Reader reader;

        if (std::filesystem::exists(filePath))
        {
            std::ifstream inFile(filePath);
            if (!reader.parse(inFile, root) || !root.isMember("items"))
            {
                root["items"] = Json::Value(Json::arrayValue);
            }
            inFile.close();
        }
        else
        {
            root["items"] = Json::Value(Json::arrayValue);
        }

        root["items"].append(itemID);

        std::ofstream outFile(filePath);
        Json::FastWriter writer;
        outFile << writer.write(root);
        outFile.close();

        Output::send<LogLevel::Verbose>(STR("Saved item {} to queue (total: {})\n"), itemID, root["items"].size());
    }
    catch (const std::exception&)
    {
        Output::send<LogLevel::Verbose>(STR("Exception while saving item to queue\n"));
    }
}

std::vector<int> ReceivedItemQueue::flushReceivedItems()
{
    std::vector<int> items;
    std::string filePath = getReceivedItemQueueFilePath();

    if (filePath.empty() || !std::filesystem::exists(filePath))
    {
        return items;
    }

    try
    {
        Json::Value root;
        Json::Reader reader;

        std::ifstream inFile(filePath);
        if (!reader.parse(inFile, root) || !root.isMember("items"))
        {
            inFile.close();
            std::filesystem::remove(filePath);
            return items;
        }
        inFile.close();

        const Json::Value& itemArray = root["items"];
        for (const auto& item : itemArray)
        {
            if (item.isInt())
            {
                items.push_back(item.asInt());
            }
        }

        std::filesystem::remove(filePath);
        Output::send<LogLevel::Verbose>(STR("Loaded and cleared {} items from queue\n"), items.size());
    }
    catch (const std::exception&)
    {
        Output::send<LogLevel::Verbose>(STR("Exception while loading queue\n"));
    }

    return items;
}

void ReceivedItemQueue::deleteReceivedItemQueue()
{
    std::string filePath = getReceivedItemQueueFilePath();
    if (filePath.empty())
        return;

    try
    {
        if (std::filesystem::exists(filePath))
        {
            std::filesystem::remove(filePath);

            std::wstring wFilePath(filePath.begin(), filePath.end());
            Output::send<LogLevel::Verbose>(STR("Deleted {} file\n"), wFilePath);
        }
    }
    catch (const std::exception&)
    {
        Output::send<LogLevel::Verbose>(STR("Exception while deleting item queue\n"));
    }
}

std::string CheckedLocationQueue::getCheckedLocationQueueFilePath()
{
    wchar_t dllPath[MAX_PATH] = {0};
    HMODULE moduleHandle = GetModuleHandle(nullptr);
    if (!moduleHandle)
    {
        Output::send<LogLevel::Error>(STR("Could not get module handle for DLL path\n"));
        return "";
    }

    GetModuleFileNameW(moduleHandle, dllPath, MAX_PATH);
    std::filesystem::path modPath = std::filesystem::path(dllPath).parent_path();

    int32_t saveFileIndex = 0;
    std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
    if (gameInstance.has_value())
    {
        int32_t* saveIndex = UnrealObjectQueries::GetNestedPropertyValue<int32_t>(gameInstance.value(), L"System", L"SaveFileIndex_7_9ACF69B4474D76AACA0E349806254782");
        if (saveIndex)
        {
            saveFileIndex = *saveIndex;
        }
    }

    std::string filename = "checked_location_queue_" + std::to_string(saveFileIndex) + ".json";
    std::string queuePath = (modPath / filename).string();

    Output::send<LogLevel::Verbose>(STR("Checked location queue file path set for save index {}\n"), saveFileIndex);
    return queuePath;
}

bool CheckedLocationQueue::checkedLocationQueueFileExists()
{
    std::string filePath = getCheckedLocationQueueFilePath();
    if (filePath.empty())
        return false;

    return std::filesystem::exists(filePath);
}

int CheckedLocationQueue::flushUnsentCheckedLocations()
{
    std::string filePath = getCheckedLocationQueueFilePath();
    int sentCount = 0;

    if (filePath.empty() || !std::filesystem::exists(filePath))
    {
        return sentCount;
    }

    try
    {
        Json::Value root;
        Json::Reader reader;

        std::ifstream inFile(filePath);
        if (!reader.parse(inFile, root))
        {
            inFile.close();
            return sentCount;
        }
        inFile.close();

        if (!root.isMember("locations") || !root.isMember("lastSentIndex"))
        {
            return sentCount;
        }

        const Json::Value& locationArray = root["locations"];
        int lastSentIndex = root["lastSentIndex"].asInt();

        for (int i = lastSentIndex; i < (int)locationArray.size(); ++i)
        {
            if (locationArray[i].isInt64())
            {
                int64_t locationID = locationArray[i].asInt64();
                AP_SendItem(locationID);
                sentCount++;
            }
        }

        if (sentCount > 0)
        {
            root["lastSentIndex"] = lastSentIndex + sentCount;

            std::ofstream outFile(filePath);
            Json::FastWriter writer;
            outFile << writer.write(root);
            outFile.close();

            Output::send<LogLevel::Verbose>(STR("Flushed {} queued locations\n"), sentCount);
        }
    }
    catch (const std::exception&)
    {
        Output::send<LogLevel::Verbose>(STR("Exception while flushing send queue\n"));
    }

    return sentCount;
}

void CheckedLocationQueue::appendCheckedLocation(int64_t locationID)
{
    std::string filePath = getCheckedLocationQueueFilePath();
    if (filePath.empty())
    {
        Output::send<LogLevel::Error>(STR("Could not determine checked location queue file path\n"));
        return;
    }

    // Read current state from managers
	bool bIsAuthenticated = (AP_GetConnectionStatus() == AP_ConnectionStatus::Authenticated);
    bool bIsGameLoaded = BlueFireArchipelagoMod::arcManager ? BlueFireArchipelagoMod::arcManager->isGameLoaded() : false;


    // If already authenticated and in game, send immediately
    if (bIsAuthenticated && bIsGameLoaded)
    {
        AP_SendItem(locationID);
        Output::send<LogLevel::Verbose>(STR("Sent location {} immediately\n"), (int64_t)locationID);
        return;
    }
    else
    {
        Output::send<LogLevel::Error>(STR("Couldn't send item to server\n"));
        Output::send<LogLevel::Error>(STR("Value of bIsAuthenticated : {}\n"), bIsAuthenticated);
        Output::send<LogLevel::Error>(STR("Value of bIsGameLoaded : {}\n"), bIsGameLoaded);
        Output::send<LogLevel::Error>(STR("Value of isGameLoaded : {}\n"), BlueFireArchipelagoMod::arcManager->isGameLoaded());
        Output::send<LogLevel::Error>(STR("Value of BlueFireArchipelagoMod::arcManager : {}\n"), (void*)BlueFireArchipelagoMod::arcManager);
    }

    try
    {
        Json::Value root;
        Json::Reader reader;

        if (std::filesystem::exists(filePath))
        {
            std::ifstream inFile(filePath);
            if (!reader.parse(inFile, root) || !root.isMember("locations"))
            {
                root["locations"] = Json::Value(Json::arrayValue);
                root["lastSentIndex"] = 0;
            }
            inFile.close();
        }
        else
        {
            root["locations"] = Json::Value(Json::arrayValue);
            root["lastSentIndex"] = 0;
        }

        root["locations"].append((Json::Value::Int64)locationID);

        std::ofstream outFile(filePath);
        Json::FastWriter writer;
        outFile << writer.write(root);
        outFile.close();

        Output::send<LogLevel::Verbose>(STR("Appended location {} to queue (total: {})\n"), (int64_t)locationID, root["locations"].size());
    }
    catch (const std::exception&)
    {
        Output::send<LogLevel::Verbose>(STR("Exception while appending to checked location queue\n"));
    }
}

void CheckedLocationQueue::deleteCheckedLocationQueue()
{
    std::string filePath = getCheckedLocationQueueFilePath();
    if (filePath.empty())
        return;

    try
    {
        if (std::filesystem::exists(filePath))
        {
            std::filesystem::remove(filePath);

            std::wstring wFilePath(filePath.begin(), filePath.end());
            Output::send<LogLevel::Verbose>(STR("Deleted {} file\n"), wFilePath);
        }
    }
    catch (const std::exception&)
    {
        Output::send<LogLevel::Verbose>(STR("Exception while deleting send queue\n"));
    }
}