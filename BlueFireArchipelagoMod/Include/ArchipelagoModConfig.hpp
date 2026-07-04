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
		constexpr uint8_t KEY_ITEM_TYPE = 0x0;

		// Item amount
		constexpr uint32_t ITEM_AMOUNT = 0x1;
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
	// Statue Names - Name of all the statues in the game
	// ============================================================
	namespace EmoteNames
	{
		constexpr auto WINDMILL_EMOTE = STR("A02_Arcane_EmoteStatue_Windmill");
	}

	// ============================================================
	// Location IDs - Compiled from Locations.yaml at build time
	// ============================================================
	// NOTE: Location data is now embedded in the binary via LocationsData.hpp
	// Access maps using LocationsData::GetChestNameToLocationIDMap()
	// and LocationsData::GetStatueNameToLocationIDMap()
	//
	// The Rust generator tool converts Locations.yaml into C++ static inline maps
	// during the CMake build process. This eliminates runtime JSON parsing and
	// external file dependencies.


	// ============================================================
	// Archipelago - Values for Archipelago integration
	// ============================================================
	namespace Archipelago
	{
		constexpr uint32_t BF_BASE_ID = 437000;
		constexpr char GAME_NAME[] = "Blue Fire";
	}

	namespace Weapons
	{
		constexpr uint8_t BLOODSTORM_BLADES = 1;
		constexpr uint8_t DIAMOND_WINGS = 2;
		constexpr uint8_t SHADOW_CASTERS = 3;
		constexpr uint8_t EMBER_TWINS = 4;
		constexpr uint8_t IRON_JUSTICE = 5;
		constexpr uint8_t ICE_DESTROYERS = 6;
		constexpr uint8_t PEACE_KEEPERS = 7;
		constexpr uint8_t STEEL_SHANKS = 8;
		constexpr uint8_t BREEMUR_FAMILY_SWORDS = 9;
		constexpr uint8_t SILVER_BLADES = 10;
		constexpr uint8_t KINA_DEFENDERS = 11;
		constexpr uint8_t VOID_BLADES = 13;

		const std::map<uint8_t, uint32_t> weaponFromOrder =
		{
			{1, DIAMOND_WINGS},
			{2, BLOODSTORM_BLADES},
			{3, SILVER_BLADES},
			{4, PEACE_KEEPERS},
			{5, BREEMUR_FAMILY_SWORDS},
			{6, ICE_DESTROYERS},
			{7, EMBER_TWINS},
			{8, IRON_JUSTICE},
			{9, STEEL_SHANKS},
			{10, KINA_DEFENDERS},
			{11, SHADOW_CASTERS},
			{12, VOID_BLADES},
		};

		const std::map<uint8_t, uint32_t> orderFromWeapon =
		{
			{DIAMOND_WINGS, 1},
			{BLOODSTORM_BLADES, 2},
			{SILVER_BLADES, 3},
			{PEACE_KEEPERS, 4},
			{BREEMUR_FAMILY_SWORDS, 5},
			{ICE_DESTROYERS, 6},
			{EMBER_TWINS, 7},
			{IRON_JUSTICE, 8},
			{STEEL_SHANKS, 9},
			{KINA_DEFENDERS, 10},
			{SHADOW_CASTERS, 11},
			{VOID_BLADES, 12},
		};
	}



	namespace Shops
	{
		constexpr uint8_t MORK = 0;
		constexpr uint8_t ONROM = 1;
		constexpr uint8_t SPIRIT_HUNTER = 2;
		constexpr uint8_t ARI = 3;
		constexpr uint8_t POTI = 4;
		constexpr uint8_t POI = 5;

		namespace Mork
		{
			constexpr uint8_t POSSESED_BOOK = 12;
			constexpr uint8_t GOLDEN_LUST = 16;
			constexpr uint8_t LIFE_STEAL = 25;
			constexpr uint8_t LARGE_POUCH = 0;
			constexpr uint8_t RARE_SNOW = 67;

			const std::map<uint8_t, uint32_t> spiritInventory =
			{
				{POSSESED_BOOK, 0},
				{GOLDEN_LUST, 1},
				{LIFE_STEAL, 2},
			};

			const std::map<uint8_t, uint32_t> itemInventory =
			{
				{LARGE_POUCH, 3},
				{RARE_SNOW, 4},
			};
		}

		namespace Onrom
		{
			constexpr uint8_t RARE_GLASSES = 66;
			constexpr uint8_t MANDOLINE = 65;
			constexpr uint8_t BOOT = 18;

			const std::map<uint8_t, uint32_t> inventory =
			{
				{RARE_GLASSES, 0},
				{MANDOLINE, 1},
				{BOOT, 2},
			};
		}

		namespace SpiritHunter
		{
			constexpr uint8_t STORM_CENTRY = 10;
			constexpr uint8_t BLOOD_PHANTOM = 11;
			constexpr uint8_t FROZEN_SOUL = 7;
			constexpr uint8_t SHADOW_GRU = 27;

			const std::map<uint8_t, uint32_t> inventory =
			{
				{STORM_CENTRY, 0},
				{BLOOD_PHANTOM, 1},
				{FROZEN_SOUL, 2},
				{SHADOW_GRU, 3},
			};
		}

		namespace Ari
		{
			constexpr uint8_t ORANGE = 27;
			constexpr uint8_t AQUA = 26;
			constexpr uint8_t ROYAL = 25;
			constexpr uint8_t LILA = 24;
			constexpr uint8_t RAINBOW = 23;
			constexpr uint8_t LIGHT_BLUE = 22;
			constexpr uint8_t VIOLET = 21;
			constexpr uint8_t GREY = 20;
			constexpr uint8_t GREEN = 19;
			constexpr uint8_t YELLOW = 18;
			constexpr uint8_t RED = 17;

			const std::map<uint8_t, uint32_t> inventory =
			{
				{ORANGE, 0},
				{AQUA, 1},
				{ROYAL, 2},
				{LILA, 3},
				{RAINBOW, 4},
				{LIGHT_BLUE, 5},
				{VIOLET, 6},
				{GREY, 7},
				{GREEN, 8},
				{YELLOW, 9},
				{RED, 10},
			};
		}

		namespace Poti
		{
			constexpr uint8_t ONOP_SIBLINGS = 18;
			constexpr uint8_t MOI_THE_DREADFUL = 14;
			constexpr uint8_t SHADOW_CASTERS = 3;
			constexpr uint8_t EXTRA_LARGE_POUCH = 75;

			const std::map<uint8_t, uint32_t> spiritInventory =
			{
				{ONOP_SIBLINGS, 0},
				{MOI_THE_DREADFUL, 1},
			};

			const std::map<uint8_t, uint32_t> weaponInventory =
			{
				{SHADOW_CASTERS, 2},
			};

			const std::map<uint8_t, uint32_t> itemInventory =
			{
				{EXTRA_LARGE_POUCH, 3},
			};
		}

		namespace Poi
		{
			constexpr uint8_t FIRE_ESSENCE = 16;
			constexpr uint8_t FIRE_ESSENCE_SLOT = 73;

			const std::map<uint8_t, uint32_t> inventory =
			{
				{FIRE_ESSENCE, 0},
				{FIRE_ESSENCE_SLOT, 1},
			};
		}
	}
}
