#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp> 
#include <Unreal/UObject.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/UFunction.hpp>

#include <BlueFireArchipelagoMod.hpp>
#include <ObjectCreateListener.hpp>
#include <HookManager.hpp>

using namespace RC;
using namespace Unreal;

bool bNewItemAlreadyHooked{ false };
FText* itemName;
FText* itemDescription;

BlueFireArchipelagoMod::BlueFireArchipelagoMod() : CppUserModBase()
{
    ModName = STR("BlueFireArchipelagoMod");
    ModVersion = STR("0.1");
    ModDescription = STR("Blue Fire mod to add Archipelago support");
    ModAuthors = STR("Nebulea");
    // Do not change this unless you want to target a UE4SS version
    // other than the one you're currently building with somehow.
    //ModIntendedSDKVersion = STR("2.6");

    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod initialized\n"));
}


// You are allowed to use the 'Unreal' namespace in this function and anywhere else after this function has fired
void BlueFireArchipelagoMod::on_unreal_init()
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from on_unreal_init ♥\n"));

    // Listen to object creation
    UObjectArray::AddUObjectCreateListener(&ObjectCreateListener);
    ObjectCreateListener.registerObjectCallback(std::wstring(L"NewItem_C"), ObjectCreatedNewItem);

    // Initialize the global hooks once we can now access UE objects
    HookManager.Init();

    itemName = new FText(STR("Archipelago Item"));
    itemDescription = new FText(STR("An item from the Archipelago Mod. Is used to unlock items in the game of the other players in your session."));
}


void BlueFireArchipelagoMod::ObjectCreatedNewItem(const UObjectBase* object, int32 index)
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from ObjectCreatedNewItem ♥\n"));

    // We only want to listen to the first object created, since we only want to hook once
    ObjectCreateListener.deleteObjectCallback(std::wstring(L"NewItem_C"));

    // Find the function to hook
    UObject* PlayNewItemFunc = UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Game/BlueFire/HUD/NewItem.NewItem_C:PlayNewItem"));
    static std::wstring PlayNewItemFuncName = PlayNewItemFunc->GetNamePrivate().ToString();

    // Hook the PlayNewItem function
    HookManager.registerHook(PlayNewItemFuncName, PlayNewItemPreHook);
    Output::send(STR("Registered hook for function: {}\n"), PlayNewItemFuncName);
}


void BlueFireArchipelagoMod::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from PlayNewItem ♥\n"));

    std::optional<FText> InText = HookManager::readParamValue<FText>(STR("InText"), Stack);
    if (InText.has_value())
    {
        Output::send<LogLevel::Verbose>(STR("InText has value {} ♥\n"), InText.value().ToString());
    }

    HookManager::setParamValue<FText>(STR("InText"), Stack, itemName);
    HookManager::setParamValue<FText>(STR("Description"), Stack, itemDescription);
    HookManager::setParamValue<uint8_t>(STR("KeyItem"), Stack, 0x1);
    HookManager::setParamValue<uint32_t>(STR("Amount"), Stack, 0xF);
}