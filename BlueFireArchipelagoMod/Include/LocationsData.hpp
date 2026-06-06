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
        {L"Chest_A02_Keep_Loot_02", 3},
        {L"Chest_A02_Keep_Loot_01", 4},
        {L"Chest_A02_GameIntro_44", 5},
        {L"Chest_A02_Sword_DiamondWings", 6},
        {L"Chest_A02_NorthArcane_Loot_01", 7},
        {L"Chest_A02_NorthArcane_Loot_03", 8},
        {L"Chest_A02_NorthArcane_Loot_04", 9},
        {L"Chest_A02_EastArcane_Loot_01", 10},
        {L"Chest_A02_EastArcane_Loot_05", 11},
        {L"Chest_A02_NorthArcane_Sword_Bloodstorm", 12},
        {L"Chest_A01_Arcane_Spell", 13},
        {L"Chest_A02_SouthArcane_Key_02", 14},
        {L"Chest_A02_EastArcane_Loot_02", 15},
        {L"Chest_A02_EastArcane_Loot_03", 16},
        {L"Chest_A02_SouthArcane_Loot_01", 17},
        {L"Chest_A02_EastArcane_Loot_04", 18},
        {L"Chest_A02_SouthArcane_Key_01", 19},
        {L"Dance_Platform_Celebration_Chest", 20},
        {L"Chest_A02_Tunic_PureShadow", 21},
        {L"Chest_A01_Stoneheart_Loot_03", 22},
        {L"Chest_A01_Stoneheart_Loot_01", 23},
        {L"Chest_A01_Stoneheart_Loot_02", 24},
        {L"Chest_A01_Stoneheart_Loot_3", 25},
        {L"Chest_A01_Stoneheart_Tunic_MerchantsRobe", 26},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey", 27},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey2", 28},
        {L"Chest_A01_CrossRoads_Loot_3", 29},
        {L"Chest_A01_CrossRoads_Loot_01", 30},
        {L"Chest_A01_CrossRoads_Loot_02", 31},
        {L"Chest_A01_CrossRoads_Loot_03", 32},
        {L"Chest_A01_Nuos_Loot_05", 33},
        {L"Chest_A01_Nuos_Loot_02", 34},
        {L"Chest_A01_Nuos_Key_2", 35},
        {L"Chest_A01_Nuos_Key_01", 36},
        {L"Chest_A01_Nuos_Key_03", 37},
        {L"Chest_A01_Nuos_Ability_WallRun", 38},
        {L"Chest_A01_Nuos_Loot_03", 39},
        {L"Chest_A01_Nuos_Loot_06", 40},
        {L"Chest_A01_Nuos_Loot_04", 41},
        {L"Chest_Master_2", 42},
        {L"Chest_A01_TempleGardens_Sword_SilverBlades", 43},
        {L"Chest_A01_Nuos_Key_02", 44},
        {L"Chest_A01_Nuos_MasterKey", 45},
        {L"Chest_A01_Graveyard_Loot_03", 46},
        {L"Chest_A01_Graveyard_IceDestroyers", 47},
        {L"Chest_A02_Tunic_HolyAttire", 48},
        {L"Chest_A01_Graveyard_Loot_01", 49},
        {L"Chest_A01_Graveyard_Loot_02", 50},
        {L"Chest_A01_Uthas_Key_04", 51},
        {L"Chest_A01_Uthas_Loot_03", 52},
        {L"Chest_A01_Uthas_Loot_02", 53},
        {L"Chest_A01_Uthas_Loot_04", 54},
        {L"Chest_A01_Uthas_Key_01", 55},
        {L"Chest_A01_Uthas_Key_02_1", 56},
        {L"Chest_A01_Uthas_Loot_05", 57},
        {L"Chest_A01_Uthas_Loot_6", 58},
        {L"Chest_A01_Uthas_MasterKey", 59},
        {L"Chest_A01_Uthas_Loot_01", 60},
        {L"Chest_A01_Uthas_Key_03", 61},
        {L"Chest_A01_Uthas_Loot_06", 62},
        {L"Chest_A01_Uthas_Key_05", 63},
        {L"Chest_A01_Uthas_MasterKey2_5", 64},
        {L"Chest_A01_TempleGardens_Loot_04", 65},
        {L"Chest_A01_TempleGardens_Loot_08", 66},
        {L"Chest_A01_TempleGardens_Loot_03", 67},
        {L"Chest_A01_TempleGardens_RareSnow", 68},
        {L"Chest_A01_TempleGardens_Loot_02", 69},
        {L"Chest_A01_TempleGardens_Loot_01", 70},
        {L"Chest_A01_TempleGardens_Loot_07", 71},
        {L"Chest_A01_TempleGardens_Loot_06", 72},
    };

    static inline const std::map<std::wstring, uint32_t> s_statueNameToLocationID = {
        {L"A02_Arcane_EmoteStatue_Windmill", 73},
        {L"A02_Arcane_EmoteStatue_HatKid", 74},
        {L"A01_Graveyard_EmoteStatue_No", 75},
        {L"A01_Graveyard_EmoteStatue_Aggressive", 76},
        {L"A01_Uthas_EmoteStatue_Photo", 77},
        {L"A01_Uthas_EmoteStatue_Party", 78},
    };

    static inline const std::map<std::wstring, uint32_t> s_pickupNameToLocationID = {
        {L"Spirit_A02_ToxicRat", 79},
        {L"Pickup_Rose", 80},
        {L"Pickup_Rose2", 81},
        {L"Pickup_Necklace", 82},
        {L"Pickup_BremurPicture", 83},
        {L"Pickup_Book", 84},
        {L"Pickup_Book2", 85},
        {L"Pickup_Book3", 86},
        {L"Pickup_Book4", 87},
        {L"Pickup_Book5", 88},
        {L"TODO", 89},
    };
};
