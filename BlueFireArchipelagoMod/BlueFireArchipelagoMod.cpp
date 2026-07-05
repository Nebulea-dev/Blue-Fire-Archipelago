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
#include <LocationManager.hpp>
#include <DeathLinkManager.hpp>

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
    objectListener = new ObjectCreateListener();
    hookManager = new HookHelper();
    itemManager = new ItemManager();
    arcManager = new ArchipelagoManager();
    locationManager = new LocationManager();
    deathLinkManager = new DeathLinkManager();
    mainMenuManager = new MainMenuManager();

    Output::send<LogLevel::Verbose>(STR("All managers initialized\n"));

    // Hook the RETURN and TAB key for menu submission
    register_keydown_event(Input::Key::RETURN, {}, [this]() {
        mainMenuManager->OnReturnPressed();
    });

    // Hook the RETURN and TAB key for menu submission
    register_keydown_event(Input::Key::TAB, {}, [this]() {
        mainMenuManager->OnReturnPressed();
    });
}