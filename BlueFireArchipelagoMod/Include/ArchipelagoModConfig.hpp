#pragma once

#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

/*******************************************************************************
 * ArchipelagoModConfig
 *
 * @brief   Centralized configuration for the Archipelago mod.
 *
 *          Contains all hardcoded paths, magic numbers, property names, and
 *          UI strings used throughout the mod. Organizing all configuration
 *          in one place makes the codebase maintainable and allows easy
 *          adjustment of game object paths if they change in future updates.
 *
 *          Organized into logical namespaces:
 *          - Hooks: Unreal function paths for hooking
 *          - GameObjects: Game element names and identifiers
 *          - UnrealClasses: UMG (UI framework) class and function paths
 *          - Assets: Game resource paths (fonts, etc.)
 *          - UI: Configuration constants for UI styling
 *          - Strings: User-facing text for hints and labels
 *          - PropertyNames: Unreal property names for reflection
 */
namespace ArchipelagoModConfig
{
	// ============================================================
	// Hook Paths - Unreal functions we're hooking into
	// ============================================================
	namespace Hooks
	{
		constexpr auto PLAY_NEW_ITEM =
			STR("Function /Game/BlueFire/HUD/NewItem.NewItem_C:PlayNewItem");

		constexpr auto UP_KEY =
			STR("Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:UpKey");

		constexpr auto DOWN_KEY =
			STR("Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey");
	}

	// ============================================================
	// Game Object Names & Identifiers
	// ============================================================
	namespace GameObjects
	{
		// Menu state indicator - which menu index is the Archipelago menu
		constexpr uint32_t MENU_INDEX_ARCHIPELAGO = 8;  // File naming menu

		// Original textbox that needs to be hidden
		constexpr auto ORIGINAL_TEXTBOX_NAME =
			STR("VK_EditableTextBox_89");

		// Archipelago UI textbox names
		constexpr auto TEXTBOX_IP =
			STR("EditableTextBoxArchipelagoIP");

		constexpr auto TEXTBOX_USERNAME =
			STR("EditableTextBoxArchipelagoUsername");

		constexpr auto TEXTBOX_PASSWORD =
			STR("EditableTextBoxArchipelagoPassword");

		// Game menu controller
		constexpr auto GAME_MENU_CONTROLLER_NAME =
			STR("GameMenuController_2");

		// UI container and hierarchy objects
		constexpr auto WRAP_BOX_NAME =
			STR("SaveFileWrapBox_1");

		constexpr auto WIDGET_TREE_NAME =
			STR("WidgetTree_0");

		constexpr auto WIDGET_SWITCHER_NAME =
			STR("WidgetSwitcher_0");

		// Menu section title
		constexpr auto MENU_TITLE_FILE =
			STR("File_1");

		// Marker for identifying objects within GameMenu
		constexpr auto GAME_MENU_MARKER =
			STR("GameMenu_C");
	}

	// ============================================================
	// Unreal Engine Class & Function Paths
	// ============================================================
	namespace UnrealClasses
	{
		// UMG (Unreal Motion Graphics) widget classes
		constexpr auto CLASS_EDITABLE_TEXT_BOX =
			STR("/Script/UMG.EditableTextBox");

		constexpr auto CLASS_WRAP_BOX_SLOT =
			STR("/Script/UMG.WrapBoxSlot");

		// UMG widget methods
		constexpr auto FUNC_WIDGET_SET_FOCUS =
			STR("/Script/UMG.Widget:SetFocus");

		constexpr auto FUNC_WIDGET_REMOVE_FROM_PARENT =
			STR("/Script/UMG.Widget:RemoveFromParent");

		// DownKey function (used to simulate menu navigation)
		constexpr auto FUNC_DOWN_KEY =
			STR("/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey");
	}

	// ============================================================
	// Asset Paths - Game resources (fonts, etc.)
	// ============================================================
	namespace Assets
	{
		// Font used for Archipelago UI textboxes
		constexpr auto FONT_SPECTRAL_BOLD =
			STR("/Game/BlueFire/HUD/Font/BlueFire-Spectral/BlueFire-SpectralSC-Bold_Font.BlueFire-SpectralSC-Bold_Font");
	}

	// ============================================================
	// UI Configuration
	// ============================================================
	namespace UI
	{
		// Font size for Archipelago textboxes
		constexpr uint32_t TEXTBOX_FONT_SIZE = 40;

		// Text justification: 0x1 = ETextJustify::Center
		constexpr uint8_t TEXT_JUSTIFY_CENTER = 0x1;

		// WrapBox slot should fill empty space
		constexpr uint8_t FILL_EMPTY_SPACE = 0x1;

		// Key item type
		constexpr uint8_t KEY_ITEM_TYPE = 0x1;

		// Item amount
		constexpr uint32_t ITEM_AMOUNT = 0xF;
	}

	// ============================================================
	// UI Strings - Hint text and item information
	// ============================================================
	namespace Strings
	{
		// Textbox hint text
		constexpr auto HINT_ARCHIPELAGO_SERVER_IP = STR("Archipelago Server IP");
		constexpr auto HINT_ARCHIPELAGO_USERNAME = STR("Archipelago Username");
		constexpr auto HINT_ARCHIPELAGO_PASSWORD = STR("Archipelago Password");

		// Item display information
		constexpr auto ITEM_NAME = STR("Archipelago Item");
		constexpr auto ITEM_DESCRIPTION = STR("An item from the Archipelago Mod. Is used to unlock items in the game of the other players in your session.");
	}

	// ============================================================
	// Property Names - Used when reading/writing UObject properties
	// ============================================================
	namespace PropertyNames
	{
		// Widget properties
		constexpr auto PROP_TEXT = STR("Text");
		constexpr auto PROP_HINT_TEXT = STR("HintText");
		constexpr auto PROP_SLOT = STR("Slot");
		constexpr auto PROP_PARENT = STR("Parent");
		constexpr auto PROP_CONTENT = STR("Content");
		constexpr auto PROP_SLOTS = STR("Slots");
		constexpr auto PROP_FILL_EMPTY_SPACE = STR("bFillEmptySpace");
		constexpr auto PROP_JUSTIFICATION = STR("Justification");
		constexpr auto PROP_WIDGET_STYLE = STR("WidgetStyle");
		constexpr auto PROP_FONT = STR("Font");
		constexpr auto PROP_FONT_OBJECT = STR("FontObject");
		constexpr auto PROP_FONT_SIZE = STR("Size");
		constexpr auto PROP_ACTIVE_WIDGET_INDEX = STR("ActiveWidgetIndex");

		// Hook parameters
		constexpr auto PARAM_IN_TEXT = STR("InText");
		constexpr auto PARAM_DESCRIPTION = STR("Description");
		constexpr auto PARAM_KEY_ITEM = STR("KeyItem");
		constexpr auto PARAM_AMOUNT = STR("Amount");
	}

	// ============================================================
	// Chest Names - Name of all the chests in the game
	// ============================================================
	namespace ChestNames
	{
		namespace FireKeepIntro
		{
			constexpr auto AMBUSH_CHEST_1 = STR("Chest_A01_Keep_Shield");
			constexpr auto AMBUSH_CHEST_2 = STR("Chest_A02_Keep_Key_01");
		}

		namespace FireKeepHub
		{
			constexpr auto SPIN_ATTACK_CHEST = STR("Chest_A01_TempleGardens_Ability_SpinAttack2");
			constexpr auto LOOT_CHEST_1 = STR("Chest_A02_Keep_Loot_01");
			constexpr auto LOOT_CHEST_2 = STR("Chest_A02_Keep_Loot_02");
			constexpr auto LOOT_CHEST_3 = STR("Chest_A02_GameIntro_44");
			constexpr auto DIAMOND_WING_CHEST = STR("Chest_A02_Sword_DiamondWings");
		}

		namespace ArcaneTunnels
		{
			constexpr auto NORTH_LOOT_CHEST_1 = STR("Chest_A02_NorthArcane_Loot_01");
			constexpr auto NORTH_LOOT_CHEST_2 = STR("Chest_A02_NorthArcane_Loot_03");
			constexpr auto NORTH_LOOT_CHEST_3 = STR("Chest_A02_NorthArcane_Loot_04"); // TODO : This probably requires a special skill cuz its behind an iron gate
			constexpr auto EAST_LOOT_CHEST_1 = STR("Chest_A02_EastArcane_Loot_01");
			constexpr auto EAST_LOOT_CHEST_4 = STR("Chest_A02_EastArcane_Loot_04");
			constexpr auto EAST_LOOT_CHEST_5 = STR("Chest_A02_EastArcane_Loot_05");
			constexpr auto BLOODSTORM_CHEST = STR("Chest_A02_NorthArcane_Sword_Bloodstorm");
			constexpr auto ARCANE_CHEST = STR("Chest_A01_Arcane_Spell");
			constexpr auto SOUTH_KEY_CHEST_2 = STR("Chest_A02_SouthArcane_Key_02"); // Doesn't actually contain a key but a Ruby ?

			// Uncategorized yet
			// constexpr auto EAST_LOOT_CHEST_2 = STR("Chest_A02_EastArcane_Loot_02"); // Could not find
			// constexpr auto EAST_LOOT_CHEST_3 = STR("Chest_A02_EastArcane_Loot_03"); // Could not find
			// constexpr auto SOUTH_LOOT_CHEST = STR("Chest_A02_SouthArcane_Loot_01"); // Could not find
			// constexpr auto SOUTH_KEY_CHEST_1 = STR("Chest_A02_SouthArcane_Key_01"); // Could not find
		}

		namespace WaterWays
		{
			// Uncategorized yet
			constexpr auto PURE_SHADOW_CHEST = STR("Chest_A02_Tunic_PureShadow");
		}

		namespace StoneHeart
		{
			constexpr auto STONEHEART_CHEST_3 = STR("Chest_A01_Stoneheart_Loot_03");

			// Uncategorized yet
			constexpr auto STONEHEART_CHEST_1 = STR("Chest_A01_Stoneheart_Loot_01");
			constexpr auto STONEHEART_CHEST_2 = STR("Chest_A01_Stoneheart_Loot_02");
			constexpr auto STONEHEART_CHEST_4 = STR("Chest_A01_Stoneheart_Loot_3");
			constexpr auto MERCHANTS_ROBE_CHEST = STR("Chest_A01_Stoneheart_Tunic_MerchantsRobe");
			constexpr auto GRAVEYARD_KEY_CHEST_1 = STR("Chest_Master_A01_StoneheartCity_GraveyardKey");
			constexpr auto GRAVEYARD_KEY_CHEST_2 = STR("Chest_Master_A01_StoneheartCity_GraveyardKey2");
		}

		namespace CrossRoads
		{
			constexpr auto CROSSROADS_LOOT_CHEST_2 = STR("Chest_A01_CrossRoads_Loot_02");
			constexpr auto WELL_CROSSROADS_LOOT_CHEST_3 = STR("Chest_A01_CrossRoads_Loot_3");

			// Uncategorized yet
			constexpr auto CROSSROADS_CHEST_LOOT_1 = STR("Chest_A01_CrossRoads_Loot_01");
			constexpr auto CROSSROADS_CHEST_LOOT_3 = STR("Chest_A01_CrossRoads_Loot_03");

		}

		namespace TempleGardens
		{
			// Uncategorized yet
			constexpr auto LOOT_CHEST_1 = STR("Chest_A01_TempleGardens_Loot_01");
			constexpr auto LOOT_CHEST_2 = STR("Chest_A01_TempleGardens_Loot_02");
			constexpr auto LOOT_CHEST_3 = STR("Chest_A01_TempleGardens_Loot_03");
			constexpr auto LOOT_CHEST_4 = STR("Chest_A01_TempleGardens_Loot_04");
			constexpr auto LOOT_CHEST_5 = STR("Chest_A01_TempleGardens_Loot_05");
			constexpr auto LOOT_CHEST_6 = STR("Chest_A01_TempleGardens_Loot_06");
			constexpr auto LOOT_CHEST_7 = STR("Chest_A01_TempleGardens_Loot_07");
			constexpr auto LOOT_CHEST_8 = STR("Chest_A01_TempleGardens_Loot_08");
			constexpr auto RARE_SNOW_CHEST = STR("Chest_A01_TempleGardens_RareSnow");
			constexpr auto SHANKS_CHEST = STR("Chest_A01_TempleGardens_Sword_Shanks");
		}

		namespace ForestTempleWater
		{
			constexpr auto LOOT_CHEST_1 = STR("Chest_A01_Nuos_Loot_05");
			constexpr auto LOOT_CHEST_2 = STR("Chest_A01_Nuos_Loot_02");
			constexpr auto KEY_CHEST = STR("Chest_A01_Nuos_Key_2");
		}

		namespace ForestTreeAmbush1
		{
			constexpr auto KEY_CHEST = STR("Chest_A01_Nuos_Key_01");

			// constexpr auto LOOT_CHEST = STR("Chest_A01_Nuos_Loot_01"); // Not found
		}

		namespace ForestTreeAmbush2 // Requires 1 old key
		{
			constexpr auto KEY_CHEST = STR("Chest_A01_Nuos_Key_03");
		}

		namespace ForestTreeNuosClaw // Requires 1 old key
		{
			constexpr auto NUOS_CLAW_CHEST = STR("Chest_A01_Nuos_Ability_WallRun"); // Not a chest_master_c nor chest_master_child_c, no idea who tf is this guy
		}

		namespace ForestTempleCenterTree // Requires Nuos Claw
		{
			constexpr auto LOOT_CHEST_1 = STR("Chest_A01_Nuos_Loot_03");
			constexpr auto LOOT_CHEST_2 = STR("Chest_A01_Nuos_Loot_06");
			constexpr auto LOOT_CHEST_3 = STR("Chest_A01_Nuos_Loot_04");
			constexpr auto VOID_CHEST = STR("Chest_Master_2");
			constexpr auto SILVERBLADES_CHEST = STR("Chest_A01_TempleGardens_Sword_SilverBlades");
			constexpr auto KEY_CHEST = STR("Chest_A01_Nuos_Key_02");

			// constexpr auto LOOT_CHEST_1 = STR("Chest_A01_Nuos_Loot_45"); // Not found
		}

		namespace ForestTempleCenterTreeTrunk // Requires old key
		{
			constexpr auto LOOT_CHEST_1 = STR("Chest_A01_Nuos_MasterKey");
		}
	}

	// ============================================================
	// Statue Names - Name of all the statues in the game
	// ============================================================
	namespace EmoteNames
	{
		constexpr auto WINDMILL_EMOTE = STR("A02_Arcane_EmoteStatue_Windmill");
	}

	// ============================================================
	// Location IDs - Mapping of chest names to location IDs for Archipelago
	// ============================================================
	inline std::map<std::wstring, uint32_t> ChestNameToLocationID = {
		// Chests in Fire Keep - Intro
		{ ChestNames::FireKeepIntro::AMBUSH_CHEST_1, 1 },
		{ ChestNames::FireKeepIntro::AMBUSH_CHEST_2, 2 },

		// Chests in Fire Keep - Hub
		{ ChestNames::FireKeepHub::SPIN_ATTACK_CHEST, 3 },
		{ ChestNames::FireKeepHub::LOOT_CHEST_1, 4 },
		{ ChestNames::FireKeepHub::LOOT_CHEST_2, 5 },
		{ ChestNames::FireKeepHub::LOOT_CHEST_3, 6 },
		{ ChestNames::FireKeepHub::DIAMOND_WING_CHEST, 7 },

		// Chests in Arcane Tunnels
		{ ChestNames::ArcaneTunnels::NORTH_LOOT_CHEST_1, 8 },
		{ ChestNames::ArcaneTunnels::NORTH_LOOT_CHEST_2, 9 },
		{ ChestNames::ArcaneTunnels::NORTH_LOOT_CHEST_3, 10 },
		{ ChestNames::ArcaneTunnels::EAST_LOOT_CHEST_1, 11 },
		{ ChestNames::ArcaneTunnels::EAST_LOOT_CHEST_4, 12 },
		{ ChestNames::ArcaneTunnels::EAST_LOOT_CHEST_5, 13 },
		{ ChestNames::ArcaneTunnels::BLOODSTORM_CHEST, 14 },
		{ ChestNames::ArcaneTunnels::ARCANE_CHEST, 15 },
		{ ChestNames::ArcaneTunnels::SOUTH_KEY_CHEST_2, 16 },
		{ ChestNames::ArcaneTunnels::EAST_LOOT_CHEST_2, 17 },
		{ ChestNames::ArcaneTunnels::EAST_LOOT_CHEST_3, 18 },
		{ ChestNames::ArcaneTunnels::SOUTH_LOOT_CHEST, 19 },
		{ ChestNames::ArcaneTunnels::SOUTH_KEY_CHEST_1, 20 },

		// Chests in Water Ways
		{ ChestNames::WaterWays::PURE_SHADOW_CHEST, 21 },

		// Chests in Stone Heart
		{ ChestNames::WaterWays::STONEHEART_CHEST_3, 22 },
		{ ChestNames::WaterWays::STONEHEART_CHEST_1, 23 },
		{ ChestNames::WaterWays::STONEHEART_CHEST_2, 24 },
		{ ChestNames::WaterWays::STONEHEART_CHEST_4, 25 },
		{ ChestNames::WaterWays::MERCHANTS_ROBE_CHEST, 26 },
		{ ChestNames::WaterWays::GRAVEYARD_KEY_CHEST_1, 27 },
		{ ChestNames::WaterWays::GRAVEYARD_KEY_CHEST_2, 28 },

		// Chests in Cross Roads
		{ ChestNames::CrossRoads::CROSSROADS_LOOT_CHEST_2, 29 },
		{ ChestNames::CrossRoads::WELL_CROSSROADS_LOOT_CHEST_3, 30 },
		{ ChestNames::CrossRoads::CROSSROADS_CHEST_LOOT_1, 31 },
		{ ChestNames::CrossRoads::CROSSROADS_CHEST_LOOT_3, 32 },

		// Chests in Forest Temple - Water
		{ ChestNames::ForestTempleWater::LOOT_CHEST_1, 33 },
		{ ChestNames::ForestTempleWater::LOOT_CHEST_2, 34 },
		{ ChestNames::ForestTempleWater::KEY_CHEST, 35 },

		// Chests in Forest Temple - Ambush 1
		{ ChestNames::ForestTreeAmbush1::KEY_CHEST, 36 },

		// Chests in Forest Temple - Ambush 2
		{ ChestNames::ForestTreeAmbush2::KEY_CHEST, 37 },

		// Chests in Forest Temple - Nuos Claw
		{ ChestNames::ForestTreeNuosClaw::NUOS_CLAW_CHEST, 38 },

		// Chests in Forest Temple - Center Tree
		{ ChestNames::ForestTempleCenterTree::LOOT_CHEST_1, 39 },
		{ ChestNames::ForestTempleCenterTree::LOOT_CHEST_2, 40 },
		{ ChestNames::ForestTempleCenterTree::LOOT_CHEST_3, 41 },
		{ ChestNames::ForestTempleCenterTree::VOID_CHEST, 42 },
		{ ChestNames::ForestTempleCenterTree::SILVERBLADES_CHEST, 43 },
		{ ChestNames::ForestTempleCenterTree::KEY_CHEST, 44 },

		// Chests in Forest Temple - Center Tree
		{ ChestNames::ForestTempleCenterTreeTrunk::LOOT_CHEST_1, 45 },
	};

	// ============================================================
	// Location IDs - Mapping of statue names to location IDs for Archipelago
	// ============================================================
	inline std::map<std::wstring, uint32_t> StatueNameToLocationID = {
		// Chests in Fire Keep Intro
		{ EmoteNames::WINDMILL_EMOTE, 100 },
	};


	// ============================================================
	// Archipelago - Values for Archipelago integration
	// ============================================================
	namespace Archipelago
	{
		constexpr uint32_t BF_BASE_ID = 0xB70EF14E;
		constexpr char GAME_NAME[] = "Blue Fire";
	}
}
