#pragma once

#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

/// Centralized configuration for the Archipelago Mod
/// All hardcoded paths, magic numbers, and UI strings are defined here
/// Makes the codebase maintainable and easy to adjust game paths if they change
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
	// UI Strings - Hint text for textboxes
	// ============================================================
	namespace Strings
	{
		inline const FText HINT_ARCHIPELAGO_SERVER_IP(STR("Archipelago Server IP"));
		inline const FText HINT_ARCHIPELAGO_USERNAME(STR("Archipelago Username"));
		inline const FText HINT_ARCHIPELAGO_PASSWORD(STR("Archipelago Password"));
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
}
