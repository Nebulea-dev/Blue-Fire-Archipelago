#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include <Helper/ObjectFinder.hpp>
#include <Helper/HookHelper.hpp>
#include <ItemManager.hpp>
#include <ArchipelagoModConfig.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

ItemManager::ItemManager()
{
	Output::send<LogLevel::Verbose>(STR("ItemManager instance created\n"));

    itemName = new FText(Strings::ITEM_NAME);
    itemDescription = new FText(Strings::ITEM_DESCRIPTION);
}

void ItemManager::itemReceiveCb(int itemID, bool notify)
{
    // TODO: Implement item reception callback
    // This will be used to handle items received from the Archipelago server
    // std::string itemToRecieve = items[id - gdBaseID];
}

bool ItemManager::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    // Do not prevent the original function from being called
    return false;

    // Currently causes a crash for big blue chests (Chest_Master_Child_C) , no idea why.
    HookHelper::setParamValue<FText>(PropertyNames::PARAM_IN_TEXT, Stack, itemName);
    HookHelper::setParamValue<FText>(PropertyNames::PARAM_DESCRIPTION, Stack, itemDescription);
    HookHelper::setParamValue<uint8_t>(PropertyNames::PARAM_KEY_ITEM, Stack, UI::KEY_ITEM_TYPE);
    HookHelper::setParamValue<uint32_t>(PropertyNames::PARAM_AMOUNT, Stack, UI::ITEM_AMOUNT);

    // Do not prevent the original function from being called
    return false;
}