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

    /**
     * Get the void gate name to location ID map
     * @return A map of void gate names to location IDs
     */
    static const std::map<std::wstring, uint32_t>& GetVoidGateNameToLocationIDMap()
    {
        return s_voidGateNameToLocationID;
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
        {L"Chest_A01_Stoneheart_Loot_03", 21},
        {L"Chest_A01_Stoneheart_Loot_02", 22},
        {L"Chest_A01_Stoneheart_Loot_3", 23},
        {L"Chest_A01_Stoneheart_Tunic_MerchantsRobe", 24},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey", 25},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey2", 26},
        {L"Chest_A01_Stoneheart_Loot_01", 27},
        {L"Chest_Master_Loot_04_V2", 28},
        {L"Chest_A02_Tunic_PureShadow", 29},
        {L"Chest_A01_CrossRoads_Loot_03", 30},
        {L"Chest_A01_Well_SpinAttack", 31},
        {L"Chest_A01_CrossRoads_Loot_3", 32},
        {L"Chest_A01_CrossRoads_Loot_01", 33},
        {L"Chest_A01_CrossRoads_Loot_02", 34},
        {L"Chest_A01_Nuos_Loot_05", 35},
        {L"Chest_A01_Nuos_Loot_02", 36},
        {L"Chest_A01_Nuos_Key_2", 37},
        {L"Chest_A01_Nuos_Key_01", 38},
        {L"Chest_A01_Nuos_Key_03", 39},
        {L"Chest_A01_Nuos_Ability_WallRun", 40},
        {L"Chest_A01_Nuos_Loot_03", 41},
        {L"Chest_A01_Nuos_Loot_06", 42},
        {L"Chest_A01_Nuos_Loot_04", 43},
        {L"Chest_Master_2", 44},
        {L"Chest_A01_TempleGardens_Sword_SilverBlades", 45},
        {L"Chest_A01_Nuos_Key_02", 46},
        {L"Chest_A01_Nuos_MasterKey", 47},
        {L"Chest_A01_Graveyard_Loot_03", 48},
        {L"Chest_A01_Graveyard_IceDestroyers", 49},
        {L"Chest_A02_Tunic_HolyAttire", 50},
        {L"Chest_A01_Graveyard_Loot_01", 51},
        {L"Chest_A01_Graveyard_Loot_02", 52},
        {L"Chest_A01_Uthas_Key_04", 53},
        {L"Chest_A01_Uthas_Loot_03", 54},
        {L"Chest_A01_Uthas_Loot_02", 55},
        {L"Chest_A01_Uthas_Loot_04", 56},
        {L"Chest_A01_Uthas_Key_01", 57},
        {L"Chest_A01_Uthas_Key_02_1", 58},
        {L"Chest_A01_Uthas_Loot_05", 59},
        {L"Chest_A01_Uthas_Loot_6", 60},
        {L"Chest_A01_Uthas_MasterKey", 61},
        {L"Chest_A01_Uthas_Loot_01", 62},
        {L"Chest_A01_Uthas_Key_03", 63},
        {L"Chest_A01_Uthas_Loot_06", 64},
        {L"Chest_A01_Uthas_Key_05", 65},
        {L"Chest_A01_Uthas_MasterKey2_5", 66},
        {L"Chest_A01_TempleGardens_Loot_04", 67},
        {L"Chest_A01_TempleGardens_Loot_08", 68},
        {L"Chest_A01_TempleGardens_Loot_03", 69},
        {L"Chest_A01_TempleGardens_RareSnow", 70},
        {L"Chest_A01_TempleGardens_Loot_02", 71},
        {L"Chest_A01_TempleGardens_Loot_01", 72},
        {L"Chest_A01_TempleGardens_Loot_07", 73},
        {L"Chest_A01_TempleGardens_Loot_06", 74},
        {L"Chest_A01_TempleGardens_Loot_05", 75},
        {L"Chest_A06_River_Loot_01", 76},
        {L"Chest_A06_River_Loot_03", 77},
        {L"Chest_Master_Void_Santi_03", 78},
        {L"Chest_Master_Void_Santi_02", 79},
        {L"Chest_A06_Lake_Loot_01_0", 80},
        {L"Chest_A06_Lake_Loot_03", 81},
        {L"Dance_Platform_Party_Chest_Spirit_HammerKing", 82},
        {L"Dance_Platform_Aggresive_Chest", 83},
        {L"Dance_Platform_No_Chest", 84},
    };

    static inline const std::map<std::wstring, uint32_t> s_statueNameToLocationID = {
        {L"A01_FireKeep_EmoteStatue_Levitation", 85},
        {L"A01_FireKeep_EmoteStatue_Techno", 86},
        {L"A02_FireKeep_EmoteStatue_Celebration", 87},
        {L"A02_Arcane_EmoteStatue_Windmill", 88},
        {L"A02_Arcane_EmoteStatue_HatKid", 89},
        {L"A01_Graveyard_EmoteStatue_No", 90},
        {L"A01_Graveyard_EmoteStatue_Aggressive", 91},
        {L"A01_Uthas_EmoteStatue_Photo", 92},
        {L"A01_Uthas_EmoteStatue_Party", 93},
        {L"A06_Firefall_EmoteStatue_KungFu", 94},
        {L"A06_Firefall_EmoteStatue_Triceps", 95},
        {L"A06_RustCity_EmoteStatue_Applause", 96},
    };

    static inline const std::map<std::wstring, uint32_t> s_pickupNameToLocationID = {
        {L"Spirit_A02_ToxicRat", 97},
        {L"Pickup_Rose", 98},
        {L"Pickup_Rose2", 99},
        {L"Pickup_Necklace", 100},
        {L"Pickup_BremurPicture", 101},
        {L"Pickup_Book", 102},
        {L"Pickup_Book2", 103},
        {L"Pickup_Book3", 104},
        {L"Pickup_Book4", 105},
        {L"Spirit_A01_FarasGrace", 106},
        {L"Pickup_Book5", 107},
        {L"TODO", 108},
        {L"Spirit_A01_AngryAmbusher", 109},
    };
    static inline const std::map<std::wstring, uint32_t> s_voidGateNameToLocationID = {
        {L"Void_Gate_Matias", 110},
        {L"Void_Gate_Ricardo", 111},
        {L"Void_Gate_Mariano", 112},
        {L"Void_Gate_Luis", 113},
        {L"Void_Gate_Gerardo", 114},
        {L"Void_Gate_Paul", 115},
        {L"Void_Gate_Esteban", 116},
        {L"Void_Gate_Rail", 117},
        {L"Void_Gate_04_V2", 118},
        {L"Void_Gate_Julian", 119},
        {L"Void_Gate_Flauta", 120},
        {L"Void_Gate_Saw", 121},
        {L"Void_Gate_Santi", 122},
        {L"Void_Gate_Augustina", 123},
        {L"Void_Gate_Jesica", 124},
    };
};
