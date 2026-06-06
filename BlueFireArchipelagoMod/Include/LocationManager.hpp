#pragma once

#include <Unreal/UObject.hpp>
#include <Helper/HookHelper.hpp>
#include <Helper/ObjectCreateListener.hpp>

using namespace RC;
using namespace Unreal;

class LocationManager {
public:
    LocationManager();

    std::optional<uint32_t> GetLocationIDFromChestName(const std::wstring& chestName);
    std::optional<uint32_t> GetLocationIDFromStatueName(const std::wstring& statueName);
    std::optional<uint32_t> GetLocationIDFromPickupName(const std::wstring& pickupName);


    static bool OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool OnPressButton(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnDialogueWithStatueEnded(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnItemPickup(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool OnItemPickupRemove(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnSpiritPickup(UObject* Context, FFrame& Stack, void* RESULT_DECL);


private:
    static void OnNewItemCreated(const UObjectBase* object, int32 index);
    static void logIncorrectMapping(const std::wstring locationName);
};