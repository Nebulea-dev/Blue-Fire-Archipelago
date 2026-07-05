#pragma once

#include <Unreal/UObject.hpp>
#include <Helper/HookHelper.hpp>
#include <Helper/ObjectCreateListener.hpp>
#include <functional>
#include <map>

using namespace RC;
using namespace Unreal;

struct inventoryItem;

class LocationManager {
public:
    LocationManager();

    std::optional<uint32_t> GetLocationIDFromChestName(const std::wstring& chestName);
    std::optional<uint32_t> GetLocationIDFromStatueName(const std::wstring& statueName);
    std::optional<uint32_t> GetLocationIDFromPickupName(const std::wstring& pickupName);
    std::optional<uint32_t> GetLocationIDFromVoidGateName(const std::wstring& voidGateName);
    std::optional<uint32_t> GetLocationIDFromShopID(const uint32_t shopID);

    int GetItemPrice() const { return itemPrice; }
    void SetItemPrice(int price) { itemPrice = price; }

    static bool OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool OnPressButton(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnDialogueWithStatueEnded(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnItemPickup(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool OnItemPickupRemove(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnSpiritPickup(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnVoidGateCompleted(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnLevelLoaded(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool OnItemBought(UObject* Context, FFrame& Stack, void* RESULT_DECL);

    static bool OnKillGoddess(UObject* Context, FFrame& Stack, void* RESULT_DECL);


private:
    int itemPrice = 100;

    static bool ProcessShopInventory(TArray<inventoryItem>* shopInventory, const wchar_t* shopName,
        std::function<std::map<uint8_t, uint32_t>::const_iterator(uint8_t, uint8_t, uint8_t)> inventoryLookup);

    static void OnNewItemCreated(const UObjectBase* object, int32 index);
    static void logIncorrectMapping(const std::wstring locationName);
};