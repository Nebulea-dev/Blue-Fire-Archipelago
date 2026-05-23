#pragma once

#include <Unreal/UObject.hpp>
#include <Helper/HookHelper.hpp>
#include <Helper/ObjectCreateListener.hpp>

using namespace RC;
using namespace Unreal;

class LocationManager {
public:
    LocationManager();
    void Init(HookHelper* hookManager, ObjectCreateListener* objectListener);
    std::optional<uint32_t> GetLocationIDFromChestName(const std::wstring& chestName);
    std::optional<uint32_t> GetLocationIDFromStatueName(const std::wstring& chestName);

    static bool OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool OnDialogueWithStatueEnded(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static void OnNewItemCreated(const UObjectBase* object, int32 index);

};