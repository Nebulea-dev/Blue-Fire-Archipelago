#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include <ObjectFinder.hpp>
#include <HookManager.hpp>
#include <ItemManager.hpp>
#include <ArchipelagoModConfig.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

// External global variables defined in BlueFireArchipelagoMod.cpp
extern FText* itemName;
extern FText* itemDescription;

// Static item reception callback - called when items are received from Archipelago server
void ItemManager::itemReceiveCb(int itemID, bool notify)
{
    // TODO: Implement item reception callback
    // This will be used to handle items received from the Archipelago server
    // std::string itemToRecieve = items[id - gdBaseID];
}

// Static hook called when a new item is about to be displayed
bool ItemManager::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    // Example of how to read and write function parameters from a hook
    // Useless here, but kept as a reference
    std::optional<FText> InText = HookManager::readParamValue<FText>(PropertyNames::PARAM_IN_TEXT, Stack);
    if (InText.has_value())
    {
        Output::send<LogLevel::Verbose>(STR("InText has value {} ♥\n"), InText.value().ToString());
    }

    HookManager::setParamValue<FText>(PropertyNames::PARAM_IN_TEXT, Stack, itemName);
    HookManager::setParamValue<FText>(PropertyNames::PARAM_DESCRIPTION, Stack, itemDescription);
    HookManager::setParamValue<uint8_t>(PropertyNames::PARAM_KEY_ITEM, Stack, UI::KEY_ITEM_TYPE);
    HookManager::setParamValue<uint32_t>(PropertyNames::PARAM_AMOUNT, Stack, UI::ITEM_AMOUNT);

    // Do not prevent the original function from being called
    return false;
}