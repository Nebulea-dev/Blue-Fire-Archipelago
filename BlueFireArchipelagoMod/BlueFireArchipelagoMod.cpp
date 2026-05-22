#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObject.hpp>
#include <UE4SSProgram.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/Class.hpp>

#include <BlueFireArchipelagoMod.hpp>
#include <ObjectCreateListener.hpp>
#include <ObjectFinder.hpp>
#include <UnrealObjectQueries.hpp>
#include <HookManager.hpp>
#include <GUIManager.hpp>
#include <ItemManager.hpp>
#include <ArchipelagoLogic.hpp>
#include <ArchipelagoModConfig.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

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
    ObjectCreateListener.registerObjectCallback(std::wstring(L"EditableTextBox"), ActivateControlPreHook);

    // Hook the RETURN key for the menus
    register_keydown_event(Input::Key::RETURN, {}, [&]() {
        onReturnPressed();
    });

    // Initialize archipelago callbacks
    ArchipelagoLogic::init();

    // Initialize the global hooks once we can now access UE objects
    HookManager.Init();

    // Register the hooks we want
    HookManager.registerPreHook(Hooks::PLAY_NEW_ITEM, ItemManager::PlayNewItemPreHook);
    HookManager.registerPreHook(Hooks::UP_KEY, UpKeyPreHook);
    HookManager.registerPreHook(Hooks::DOWN_KEY, DownKeyPreHook);

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



    // Find the game menu controller
    std::optional<UObject*> GameMenuController = UnrealObjectQueries::FindGameMenuController();

    if (!GameMenuController.has_value())
    {
        return;
    }

    // Find the "DownKey" function
    UFunction* DownKeyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, UnrealClasses::FUNC_DOWN_KEY);
    if(!DownKeyFunc)
    {
        Output::send<LogLevel::Error>(STR("Could not find DownKey function\n"));
        return;
    }

    struct dummyStruct{} params{};
    GameMenuController.value()->ProcessEvent(DownKeyFunc, &params);

    return;
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

void BlueFireArchipelagoMod::ObjectCreatedNewItem(const UObjectBase* object, int32 index)
{
    // Called when a NewItem_C object is created
    // This is where we could hook into item creation if needed
    Output::send<LogLevel::Verbose>(STR("NewItem_C object created\n"));
}

void BlueFireArchipelagoMod::ActivateControlPreHook(const UObjectBase* object, int32 index)
{
    // Cast to UObject*
    UObject* uobject = (UObject*)object;

    if(uobject->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
	   uobject->GetNamePrivate().ToString().ends_with(GameObjects::ORIGINAL_TEXTBOX_NAME))
    {
        GUIManager::CreateTextboxes();
    }
}