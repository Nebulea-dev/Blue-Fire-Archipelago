#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp> 
#include <Unreal/UObject.hpp>
#include <UE4SSProgram.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/Class.hpp>

#include <BlueFireArchipelagoMod.hpp>
#include <ObjectCreateListener.hpp>
#include <ObjectFinder.hpp>
#include <HookManager.hpp>
#include <GUIManager.hpp>
#include <ArchipelagoLogic.hpp>

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

void itemCleared()
{
    Output::send<LogLevel::Verbose>(STR("Clearing items received from Archipelago\n"));
}

void itemReceived(int64_t item, bool notifyPlayer)
{
    Output::send<LogLevel::Verbose>(STR("Received item with id {} from Archipelago, notifyPlayer = {}\n"), item, notifyPlayer);
}

void locationChecked(int64_t location)
{
    Output::send<LogLevel::Verbose>(STR("Checked location with id {} from Archipelago\n"), location);
}

// You are allowed to use the 'Unreal' namespace in this function and anywhere else after this function has fired
void BlueFireArchipelagoMod::on_unreal_init()
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from on_unreal_init ♥\n"));

    // Listen to object creation
    UObjectArray::AddUObjectCreateListener(&ObjectCreateListener);
    ObjectCreateListener.registerObjectCallback(std::wstring(L"NewItem_C"), ObjectCreatedNewItem);
    ObjectCreateListener.registerObjectCallback(std::wstring(L"EditableTextBox"), ActivateControlPreHook);

    // Hook the RETURN key for the menus
    register_keydown_event(Input::Key::RETURN, {}, [&]() {
        onReturnPressed();
    });

    ArchipelagoLogic::init();

    // Initialize the global hooks once we can now access UE objects
    HookManager.Init();

    itemName = new FText(STR("Archipelago Item"));
    itemDescription = new FText(STR("An item from the Archipelago Mod. Is used to unlock items in the game of the other players in your session."));
}

void BlueFireArchipelagoMod::onReturnPressed()
{
    Output::send<LogLevel::Verbose>(STR("RETURN was pressed\n"));

    GUIManager::deleteOldTextbox();

    if(!GUIManager::isUserInArchipelagoMenu())
    {
        GUIManager::fileNamingActiveWidgetIndex = 0;
        return;
    }



    // Find the one GameMenuController_C object that has a full path that
    // - ends with "GameMenuController_2"
    std::optional<UObject*> GameMenuController = ObjectFinder::FindObject(L"GameMenuController_C", [](UObject* obj) {
        return obj->GetFullName().starts_with(STR("GameMenuController_C")) &&
                obj->GetNamePrivate().ToString().ends_with(STR("GameMenuController_2"));
    });

    if (!GameMenuController.has_value())
    {
        return;
    }


    // Find the "DownKey" function
    UFunction* DownKeyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey"));
    if(!DownKeyFunc)
    {
        Output::send<LogLevel::Error>(STR("Could not find DownKey function\n"));
        return;
    }

    struct dummyStruct{} params{};
    GameMenuController.value()->ProcessEvent(DownKeyFunc, &params);

    return;
}


void BlueFireArchipelagoMod::ObjectCreatedNewItem(const UObjectBase* object, int32 index)
{
    // We only want to listen to the first object created, since we only want to hook once
    ObjectCreateListener.deleteObjectCallback(std::wstring(L"NewItem_C"));

    // Hook the functions
    HookManager.registerPreHook(STR("Function /Game/BlueFire/HUD/NewItem.NewItem_C:PlayNewItem"), PlayNewItemPreHook);
    HookManager.registerPreHook(STR("Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:UpKey"), UpKeyPreHook);
    HookManager.registerPreHook(STR("Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey"), DownKeyPreHook);

    Output::send<LogLevel::Verbose>(STR("Created hooks for key functions ♥\n"));
}

bool BlueFireArchipelagoMod::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from PlayNewItem ♥\n"));

    // Example of how to read and write function parameters from a hook
    // Useless here, but kept as a reference
    std::optional<FText> InText = HookManager::readParamValue<FText>(STR("InText"), Stack);
    if (InText.has_value())
    {
        Output::send<LogLevel::Verbose>(STR("InText has value {} ♥\n"), InText.value().ToString());
    }

    HookManager::setParamValue<FText>(STR("InText"), Stack, itemName);
    HookManager::setParamValue<FText>(STR("Description"), Stack, itemDescription);
    HookManager::setParamValue<uint8_t>(STR("KeyItem"), Stack, 0x1);
    HookManager::setParamValue<uint32_t>(STR("Amount"), Stack, 0xF);

    // Do not prevent the original function from being called
    return false;
}

bool BlueFireArchipelagoMod::UpKeyPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("Upkey pressed!\n"));

    if(!GUIManager::isUserInArchipelagoMenu())
    {
        return false;
    }

    // Update fileNamingActiveWidgetIndex
    GUIManager::fileNamingActiveWidgetIndex = std::clamp(GUIManager::fileNamingActiveWidgetIndex - 1, 1, 4);

    return GUIManager::UpdateFileNamingFocus();
}

bool BlueFireArchipelagoMod::DownKeyPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("Downkey pressed!\n"));

    if(!GUIManager::isUserInArchipelagoMenu() || GUIManager::fileNamingActiveWidgetIndex == 4)
    {
        return false;
    }

    // Update fileNamingActiveWidgetIndex
    GUIManager::fileNamingActiveWidgetIndex = std::clamp(GUIManager::fileNamingActiveWidgetIndex + 1, 1, 4);

    return GUIManager::UpdateFileNamingFocus();
}

void BlueFireArchipelagoMod::ActivateControlPreHook(const UObjectBase* object, int32 index)
{
    // Cast to UObject*
    UObject* uobject = (UObject*)object;

    if(uobject->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
	   uobject->GetNamePrivate().ToString().ends_with(STR("VK_EditableTextBox_89")))
    {
        GUIManager::CreateTextboxes();
    }
}