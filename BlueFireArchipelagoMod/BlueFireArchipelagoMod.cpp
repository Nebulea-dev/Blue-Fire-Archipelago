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

    register_keydown_event(Input::Key::F5, {}, [this]() {
        // Print all obtained keys
        if (BlueFireArchipelagoMod::arcManager)
        {
            const KeyTracker& keys = BlueFireArchipelagoMod::arcManager->getKeyTracker();
            Output::send<LogLevel::Warning>(STR("=== KEYS OBTAINED ===\n"));

            if (keys.oldKeyFireKeep) Output::send<LogLevel::Warning>(STR("Old Key - Fire Keep\n"));
            if (keys.oldKeyForestTempleAmbush) Output::send<LogLevel::Warning>(STR("Old Key - Forest Temple Ambush\n"));
            if (keys.oldKeyForestTempleCenterRoom) Output::send<LogLevel::Warning>(STR("Old Key - Forest Temple Center Room\n"));
            if (keys.oldKeyForestTempleAmbush2) Output::send<LogLevel::Warning>(STR("Old Key - Forest Temple Ambush 2\n"));
            if (keys.oldKeyUthasTempleMainRoom) Output::send<LogLevel::Warning>(STR("Old Key - Uthas Temple Main Room\n"));
            if (keys.oldKeyUthasTempleAmbush) Output::send<LogLevel::Warning>(STR("Old Key - Uthas Temple Ambush\n"));
            if (keys.oldKeyUthasTemple2ndSide) Output::send<LogLevel::Warning>(STR("Old Key - Uthas Temple 2nd Side\n"));
            if (keys.oldKeyUthasTempleFinalFloor) Output::send<LogLevel::Warning>(STR("Old Key - Uthas Temple Final Floor\n"));

            if (keys.holyKeyForestTempleBoss) Output::send<LogLevel::Warning>(STR("Holy Key - Forest Temple Boss\n"));
            if (keys.holyKeyForestTempleNuosClaw) Output::send<LogLevel::Warning>(STR("Holy Key - Forest Temple Nuos Claw\n"));
            if (keys.holyKeyUthasTempleHolyTower) Output::send<LogLevel::Warning>(STR("Holy Key - Uthas Temple Holy Tower\n"));

            if (keys.uthasTempleKey) Output::send<LogLevel::Warning>(STR("Uthas Temple Key\n"));
            if (keys.templeOfGodsKey) Output::send<LogLevel::Warning>(STR("Temple of Gods Key\n"));
            if (keys.steamKey) Output::send<LogLevel::Warning>(STR("Steam Key\n"));
            if (keys.keyOfEmber) Output::send<LogLevel::Warning>(STR("Key of Ember\n"));
            if (keys.graveyardKey) Output::send<LogLevel::Warning>(STR("Graveyard Key\n"));

            Output::send<LogLevel::Warning>(STR("====================\n"));
        }
    });



}

BlueFireArchipelagoMod::~BlueFireArchipelagoMod()
{
    delete objectListener;
    delete hookManager;
    delete itemManager;
    delete arcManager;
    delete locationManager;
    delete deathLinkManager;
    delete mainMenuManager;

    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod destroyed\n"));
}