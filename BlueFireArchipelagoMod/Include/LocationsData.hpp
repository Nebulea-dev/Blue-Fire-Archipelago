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

    /**
     * Get the shop IDs to location ID map
     * @return A map of shop IDs to location IDs
     */
    static const std::map<uint32_t, uint32_t>& GetShopIDToLocationIDMap()
    {
        return s_shopIDToLocationID;
    }

public:
    // Shop location IDs (not in the map, but reserved in ID space)

private:
    static inline const std::map<std::wstring, uint32_t> s_chestNameToLocationID = {
        {L"Chest_A01_Keep_Shield", 0},
        {L"Chest_A02_Keep_Key_01", 1},
        {L"Chest_A01_TempleGardens_Ability_SpinAttack2", 2},
        {L"Chest_A02_Keep_Loot_02", 3},
        {L"Chest_A02_Keep_Loot_01", 4},
        {L"Chest_A02_GameIntro_44", 5},
        {L"Chest_A02_Sword_DiamondWings", 6},
        {L"Chest_A02_NorthArcane_Loot_01", 12},
        {L"Chest_A02_NorthArcane_Loot_03", 13},
        {L"Chest_A02_NorthArcane_Loot_04", 14},
        {L"Chest_A02_EastArcane_Loot_01", 15},
        {L"Chest_A02_EastArcane_Loot_05", 16},
        {L"Chest_A02_NorthArcane_Sword_Bloodstorm", 17},
        {L"Chest_A01_Arcane_Spell", 18},
        {L"Chest_A02_SouthArcane_Key_02", 19},
        {L"Chest_A02_EastArcane_Loot_02", 20},
        {L"Chest_A02_EastArcane_Loot_03", 21},
        {L"Chest_A02_SouthArcane_Loot_01", 22},
        {L"Chest_A02_EastArcane_Loot_04", 26},
        {L"Chest_A02_SouthArcane_Key_01", 27},
        {L"Dance_Platform_Celebration_Chest", 28},
        {L"Chest_A01_Stoneheart_Loot_03", 30},
        {L"Chest_A01_Stoneheart_Loot_02", 31},
        {L"Chest_A01_Stoneheart_Loot_3", 32},
        {L"Chest_A01_Stoneheart_Tunic_MerchantsRobe", 33},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey", 34},
        {L"Chest_Master_A01_StoneheartCity_GraveyardKey2", 35},
        {L"Chest_A01_Stoneheart_Loot_01", 36},
        {L"Chest_A01_Tunic_AlphaTunic", 54},
        {L"Chest_A02_Tunic_PureShadow", 57},
        {L"Chest_A01_CrossRoads_Loot_03", 59},
        {L"Chest_A01_Well_SpinAttack", 60},
        {L"Chest_A01_CrossRoads_Loot_3", 61},
        {L"Chest_A01_CrossRoads_Loot_01", 62},
        {L"Chest_A01_CrossRoads_Loot_02", 63},
        {L"Chest_A01_Nuos_Loot_05", 64},
        {L"Chest_A01_Nuos_Loot_02", 65},
        {L"Chest_A01_Nuos_Key_2", 66},
        {L"Chest_A01_Nuos_Key_01", 67},
        {L"Chest_A01_Nuos_Key_03", 68},
        {L"Chest_A01_Nuos_Ability_WallRun", 69},
        {L"Chest_A01_Nuos_Loot_03", 70},
        {L"Chest_A01_Nuos_Loot_06", 71},
        {L"Chest_A01_Nuos_Loot_04", 72},
        {L"Chest_Master_2", 73},
        {L"Chest_A01_TempleGardens_Sword_SilverBlades", 74},
        {L"Chest_A01_Nuos_Key_02", 75},
        {L"Dance_Platform_Hello2_Chest", 76},
        {L"Chest_A01_Nuos_MasterKey", 78},
        {L"Chest_A01_Graveyard_Loot_03", 84},
        {L"Chest_A01_Graveyard_IceDestroyers", 87},
        {L"Chest_A02_Tunic_HolyAttire", 88},
        {L"Chest_A01_Graveyard_Loot_01", 89},
        {L"Chest_A01_Graveyard_Loot_02", 90},
        {L"Chest_A01_Uthas_Key_04", 94},
        {L"Chest_A01_Uthas_Loot_03", 95},
        {L"Chest_A01_Uthas_Loot_02", 96},
        {L"Chest_A01_Uthas_Loot_04", 97},
        {L"Chest_A01_Uthas_Key_01", 98},
        {L"Chest_A01_Uthas_Key_02_1", 99},
        {L"Chest_A01_Uthas_Loot_05", 100},
        {L"Chest_A01_Uthas_Loot_6", 101},
        {L"Chest_A01_Uthas_MasterKey", 104},
        {L"Chest_A01_Uthas_Loot_01", 105},
        {L"Chest_A01_Uthas_Key_03", 106},
        {L"Chest_A01_Uthas_Loot_06", 107},
        {L"Chest_A01_Uthas_Key_05", 108},
        {L"Chest_A01_Uthas_MasterKey2_5", 109},
        {L"Chest_A01_TempleGardens_Loot_04", 111},
        {L"Chest_A01_TempleGardens_Loot_08", 112},
        {L"Chest_A01_TempleGardens_Loot_03", 113},
        {L"Chest_A01_TempleGardens_RareSnow", 114},
        {L"Chest_A01_TempleGardens_Loot_02", 115},
        {L"Chest_A01_TempleGardens_Loot_01", 116},
        {L"Chest_A01_TempleGardens_Loot_07", 117},
        {L"Chest_A01_TempleGardens_Loot_06", 118},
        {L"Chest_A01_TempleGardens_Loot_05", 119},
        {L"Chest_A06_River_Loot_01", 122},
        {L"Chest_A06_River_Loot_03", 123},
        {L"Chest_Master_Void_Santi_03", 124},
        {L"Chest_Master_Void_Santi_02", 125},
        {L"Chest_A06_Lake_Loot_01_0", 126},
        {L"Chest_A06_Lake_Loot_03", 127},
        {L"Dance_Platform_Party_Chest_Spirit_HammerKing", 128},
        {L"Dance_Platform_Aggresive_Chest", 134},
        {L"Dance_Platform_No_Chest", 135},
    };

    static inline const std::map<std::wstring, uint32_t> s_statueNameToLocationID = {
        {L"A01_FireKeep_EmoteStatue_Levitation", 7},
        {L"A01_FireKeep_EmoteStatue_Techno", 8},
        {L"A02_FireKeep_EmoteStatue_Celebration", 9},
        {L"A02_Arcane_EmoteStatue_Windmill", 23},
        {L"A02_Arcane_EmoteStatue_HatKid", 58},
        {L"A01_Graveyard_EmoteStatue_No", 79},
        {L"A01_Graveyard_EmoteStatue_Aggressive", 85},
        {L"A01_Uthas_EmoteStatue_Photo", 102},
        {L"A01_Uthas_EmoteStatue_Party", 103},
        {L"A06_Firefall_EmoteStatue_KungFu", 129},
        {L"A06_Firefall_EmoteStatue_Triceps", 130},
        {L"A06_RustCity_EmoteStatue_Applause", 136},
    };

    static inline const std::map<std::wstring, uint32_t> s_pickupNameToLocationID = {
        {L"Spirit_A02_ToxicRat", 29},
        {L"Pickup_Book", 37},
        {L"Pickup_Book2", 38},
        {L"Pickup_Book3", 39},
        {L"Pickup_Book4", 40},
        {L"Pickup_Book5", 41},
        {L"Spirit_A01_FarasGrace", 55},
        {L"Pickup_Rose", 80},
        {L"Pickup_Rose2", 81},
        {L"Pickup_Necklace", 82},
        {L"Pickup_BremurPicture", 83},
        {L"Spirit_A01_AngryAmbusher", 91},
        {L"TODO", 110},
    };

    static inline const std::map<std::wstring, uint32_t> s_voidGateNameToLocationID = {
        {L"Void_Gate_Matias", 10},
        {L"Void_Gate_Ricardo", 11},
        {L"Void_Gate_Paul", 24},
        {L"Void_Gate_Esteban", 25},
        {L"Void_Gate_Rail", 42},
        {L"Void_Gate_04_V2", 56},
        {L"Void_Gate_Easy", 77},
        {L"Void_Gate_Mariano", 86},
        {L"Void_Gate_Gerardo", 92},
        {L"Void_Gate_Luis", 93},
        {L"Void_Gate_Julian", 120},
        {L"Void_Gate_Flauta", 121},
        {L"Void_Gate_Saw", 131},
        {L"Void_Gate_Santi", 132},
        {L"Void_Gate_Augustina", 133},
        {L"Void_Gate_Jesica", 137},
    };
    static inline const std::map<uint32_t, uint32_t> s_shopIDToLocationID = {
        {1, 43},
    };
};
