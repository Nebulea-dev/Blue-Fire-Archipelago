#pragma once

#include <Unreal/UObject.hpp>
#include <Helper/HookHelper.hpp>

using namespace RC;
using namespace Unreal;

class LocationManager {
public:
    LocationManager();
    void Init(HookHelper* hookManager);
    std::optional<uint32_t> GetLocationIDFromChestName(const std::wstring& chestName);

    static bool OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL);
};