#pragma once

#include <map>
#include <string>
#include <cstdint>

/**
 * LocationsData
 *
 * @brief Compiled-in location data for the Archipelago mod.
 *        Generated at build time from locations.json.
 */
class LocationsData
{
public:
    /**
     * Get the chest name to location ID map
     * @return A map of chest names to location IDs
     */
    static const std::map<std::wstring, uint32_t>& GetChestNameToLocationIDMap()
    {
        return s_chestNameToLocationID;
    }

    /**
     * Get the statue name to location ID map
     * @return A map of statue names to location IDs
     */
    static const std::map<std::wstring, uint32_t>& GetStatueNameToLocationIDMap()
    {
        return s_statueNameToLocationID;
    }

    /**
     * Get the pickup name to location ID map
     * @return A map of pickup names to location IDs
     */
    static const std::map<std::wstring, uint32_t>& GetPickupNameToLocationIDMap()
    {
        return s_pickupNameToLocationID;
    }

private:
    static inline const std::map<std::wstring, uint32_t> s_chestNameToLocationID = {
        {L"Chest_A01_Keep_Shield", 0},
        {L"Chest_A02_Keep_Key_01", 1},
        {L"Chest_A01_TempleGardens_Ability_SpinAttack2", 2},
        {L"Chest_A02_Keep_Loot_01", 3},
        {L"Chest_A02_Keep_Loot_02", 4},
        {L"Chest_A02_GameIntro_44", 5},
        {L"Chest_A02_Sword_DiamondWings", 6},
        {L"Chest_A02_NorthArcane_Loot_01", 7},
        {L"Chest_A02_NorthArcane_Loot_03", 8},
        {L"Chest_A02_NorthArcane_Loot_04", 9},
        {L"Chest_A02_EastArcane_Loot_01", 10},
        {L"Chest_A02_EastArcane_Loot_04", 11},
        {L"Chest_A02_EastArcane_Loot_05", 12},
        {L"Chest_A02_NorthArcane_Sword_Bloodstorm", 13},
        {L"Chest_A01_Arcane_Spell", 14},
        {L"Chest_A02_SouthArcane_Key_02", 15},
        {L"Chest_A02_EastArcane_Loot_02", 16},
        {L"Chest_A02_EastArcane_Loot_03", 17},
        {L"Chest_A02_SouthArcane_Loot_01", 18},
        {L"Chest_A02_SouthArcane_Key_01", 19},
        {L"Chest_A02_Tunic_PureShadow", 20},
        {L"Chest_A01_Stoneheart_Loot_03", 21},
        {L"Chest_A01_Stoneheart_Loot_01", 22},
        {L"Chest_A01_Stoneheart_Loot_02", 23},
        {L"Chest_A01_Stoneheart_Loot_3", 24},
        {L"Chest_A01_Stoneheart_Tunic_MerchantsRobe", 25},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey", 26},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey2", 27},
        {L"Chest_A01_CrossRoads_Loot_3", 28},
        {L"Chest_A01_CrossRoads_Loot_01", 29},
        {L"Chest_A01_CrossRoads_Loot_02", 30},
        {L"Chest_A01_CrossRoads_Loot_03", 31},
        {L"Chest_A01_Nuos_Loot_05", 32},
        {L"Chest_A01_Nuos_Loot_02", 33},
        {L"Chest_A01_Nuos_Key_2", 34},
        {L"Chest_A01_Nuos_Key_01", 35},
        {L"Chest_A01_Nuos_Key_03", 36},
        {L"Chest_A01_Nuos_Ability_WallRun", 37},
        {L"Chest_A01_Nuos_Loot_03", 38},
        {L"Chest_A01_Nuos_Loot_06", 39},
        {L"Chest_A01_Nuos_Loot_04", 40},
        {L"Chest_Master_2", 41},
        {L"Chest_A01_TempleGardens_Sword_SilverBlades", 42},
        {L"Chest_A01_Nuos_Key_02", 43},
        {L"Chest_A01_Nuos_MasterKey", 44},
        {L"Chest_A02_Tunic_HolyAttire", 45},
        {L"Chest_A01_Graveyard_Loot_01", 46},
        {L"Chest_A01_Graveyard_Loot_02", 47},
        {L"Chest_A01_Graveyard_Loot_03", 48},
        {L"Chest_A01_Graveyard_IceDestroyers", 49},
    };

    static inline const std::map<std::wstring, uint32_t> s_statueNameToLocationID = {
        {L"A02_Arcane_EmoteStatue_Windmill", 50},
    };

    static inline const std::map<std::wstring, uint32_t> s_pickupNameToLocationID = {
        {L"Pickup_Rose", 51},
        {L"Pickup_Rose2", 52},
        {L"Pickup_Necklace", 53},
        {L"Pickup_BremurPicture", 54},
        {L"Pickup_Book", 55},
        {L"Pickup_Book2", 56},
        {L"Pickup_Book3", 57},
        {L"Pickup_Book4", 58},
        {L"Pickup_Book5", 59},
    };
};
