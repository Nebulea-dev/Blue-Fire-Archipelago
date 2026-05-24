#pragma once

#include <optional>
#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

/*******************************************************************************
 * UnrealObjectQueries
 *
 * @brief   High-level helpers for finding Unreal game objects.
 *
 *          Provides domain-specific query methods for locating UI elements,
 *          controllers, and other game objects. Eliminates repeated search
 *          patterns and centralizes object discovery logic.
 */
class UnrealObjectQueries
{
public:
	/*******************************************************************************
	 * @fn      FindArchipelagoTextbox
	 *
	 * @brief   Find an Archipelago EditableTextBox by its name suffix.
	 *
	 *          Locates textboxes created for Archipelago connection menu.
	 *          Pattern: Object starts with "EditableTextBox /Engine/" and name
	 *          ends with the provided boxName.
	 *
	 * @param   boxName - Suffix of textbox name (e.g., "EditableTextBoxArchipelagoIP")
	 *
	 * @return  optional containing textbox pointer, empty if not found
	 */
	static std::optional<UObject*> FindArchipelagoTextbox(std::wstring_view boxName);

	/*******************************************************************************
	 * @fn      FindGameMenuController
	 *
	 * @brief   Find the GameMenuController instance used for menu navigation.
	 *
	 *          Locates the controller that handles menu input and navigation
	 *          events. Pattern: Type is GameMenuController_C, name is
	 *          "GameMenuController_2".
	 *
	 * @return  optional containing controller pointer, empty if not found
	 */
	static std::optional<UObject*> FindGameMenuController();

	/*******************************************************************************
	 * @fn      FindWidget
	 *
	 * @brief   Generic widget finder using type, path prefix, and name suffix.
	 *
	 *          Provides flexible searching for any Unreal widget type by
	 *          combining type name, path prefix, and name suffix matching.
	 *
	 * @param   typeName   - UObject type name (e.g., "EditableTextBox", "WidgetSwitcher")
	 * @param   pathPrefix - Full path must start with this prefix
	 * @param   nameSuffix - Object name must end with this suffix
	 *
	 * @return  optional containing widget pointer, empty if not found
	 */
	static std::optional<UObject*> FindWidget(
		std::wstring_view typeName,
		std::wstring_view pathPrefix,
		std::wstring_view nameSuffix);

	/*******************************************************************************
	 * @fn      FindGameMenuWidgetSwitcher
	 *
	 * @brief   Find the WidgetSwitcher that controls menu page switching.
	 *
	 *          Locates the switcher widget that determines which menu page
	 *          is currently visible. Pattern: Type is WidgetSwitcher, name
	 *          ends with "WidgetSwitcher_0".
	 *
	 * @return  optional containing switcher pointer, empty if not found
	 */
	static std::optional<UObject*> FindGameMenuWidgetSwitcher();

	/*******************************************************************************
	 * @fn      FindGameMenuWrapBox
	 *
	 * @brief   Find the WrapBox used as parent for menu UI elements.
	 *
	 *          Locates the layout container where new menu elements are added.
	 *          Pattern: Type is WrapBox, name ends with "SaveFileWrapBox_1".
	 *
	 * @return  optional containing wrapbox pointer, empty if not found
	 */
	static std::optional<UObject*> FindGameMenuWrapBox();

	/*******************************************************************************
	 * @fn      FindGameMenuWidgetTree
	 *
	 * @brief   Find the WidgetTree that manages the menu widget hierarchy.
	 *
	 *          Locates the tree manager for creating and organizing widgets
	 *          in the menu. Pattern: Type is WidgetTree, name ends with
	 *          "WidgetTree_0", unique in GameMenu.
	 *
	 * @return  optional containing widget tree pointer, empty if not found
	 */
	static std::optional<UObject*> FindGameMenuWidgetTree();

	/*******************************************************************************
	 * @fn      FindGameMenuTitle
	 *
	 * @brief   Find a GameMenuTitle_C widget by name suffix.
	 *
	 *          Locates menu section headers. Pattern: Type is GameMenuTitle_C,
	 *          name ends with the provided suffix.
	 *
	 * @param   nameSuffix - Suffix of the title widget name
	 *
	 * @return  optional containing title pointer, empty if not found
	 */
	static std::optional<UObject*> FindGameMenuTitle(std::wstring_view nameSuffix);

	// TODO : apply same documentation style
	static std::optional<UObject*> FindGameInstance();
	static std::optional<UObject*> FindGameMenu();
};
