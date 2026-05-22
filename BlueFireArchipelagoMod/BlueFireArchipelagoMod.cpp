#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObject.hpp>
#include <UE4SSProgram.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/Class.hpp>

#include <BlueFireArchipelagoMod.hpp>
#include <Helper/ObjectCreateListener.hpp>
#include <Helper/ObjectFinder.hpp>
#include <Helper/UnrealObjectQueries.hpp>
#include <Helper/HookHelper.hpp>
#include <MainMenuManager.hpp>
#include <ItemManager.hpp>
#include <ArchipelagoManager.hpp>
#include <ArchipelagoModConfig.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

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

void BlueFireArchipelagoMod::on_unreal_init()
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from on_unreal_init ♥\n"));

    // Create and initialize all manager instances
    mainMenuManager = new MainMenuManager();
    hookManager = new HookHelper();
    itemManager = new ItemManager();
    arcManager = new ArchipelagoManager();

    // Initialize managers that have initialization logic
    hookManager->Init();
    arcManager->init();

    Output::send<LogLevel::Verbose>(STR("All managers initialized\n"));

    // Listen to object creation for ItemManager
    UObjectArray::AddUObjectCreateListener(&ObjectCreateListener);
    ObjectCreateListener.registerObjectCallback(std::wstring(L"NewItem_C"), ObjectCreatedNewItem);

    // Hook the RETURN key for menu submission
    register_keydown_event(Input::Key::RETURN, {}, [this]() {
        if (mainMenuManager)
        {
            mainMenuManager->OnReturnPressed();
        }
    });

    // Initialize MainMenuManager hooks and callbacks
    mainMenuManager->Init(hookManager, &ObjectCreateListener);

    // Register the ItemManager hook
    // TODO : Move to a NewItemDisplay class
    hookManager->registerPreHook(Hooks::PLAY_NEW_ITEM, [](UObject* Context, FFrame& Stack, void* RESULT_DECL) {
        if (BlueFireArchipelagoMod::itemManager)
        {
            return BlueFireArchipelagoMod::itemManager->PlayNewItemPreHook(Context, Stack, RESULT_DECL);
        }
        return false;
    });
}

void BlueFireArchipelagoMod::ObjectCreatedNewItem(const UObjectBase* object, int32 index)
{
    // Called when a NewItem_C object is created
    // This is where we could hook into item creation if needed
    Output::send<LogLevel::Verbose>(STR("NewItem_C object created\n"));
}