#pragma once

#include <optional>
#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

/// Centralized query helpers for finding Unreal game objects.
/// Eliminates repeated lambda patterns throughout GUIManager.
/// Reduces boilerplate and makes object-finding logic reusable.
class UnrealObjectQueries
{
public:
	/// Find an EditableTextBox by its name suffix (e.g., "EditableTextBoxArchipelagoIP")
	/// Pattern: Object starts with "EditableTextBox /Engine/" and name ends with boxName
	static std::optional<UObject*> FindArchipelagoTextbox(std::wstring_view boxName);

	/// Find the GameMenuController_2 instance used for menu navigation
	/// Pattern: Type is GameMenuController_C, name ends with "GameMenuController_2"
	static std::optional<UObject*> FindGameMenuController();

	/// Generic widget finder: locate by type, path prefix, and name suffix
	/// Useful for: EditableTextBox, WidgetSwitcher, GameMenuTitle_C, WrapBox, etc.
	///
	/// @param typeName - UObject type name (e.g., "EditableTextBox", "WidgetSwitcher")
	/// @param pathPrefix - Full path must start with this (e.g., "EditableTextBox /Engine/")
	/// @param nameSuffix - Object name must end with this (e.g., "EditableTextBoxArchipelagoIP")
	/// @return Optional containing the found object, or empty if not found
	static std::optional<UObject*> FindWidget(
		std::wstring_view typeName,
		std::wstring_view pathPrefix,
		std::wstring_view nameSuffix);

	/// Find a WidgetSwitcher in GameMenu used for menu page switching
	/// Pattern: Type is WidgetSwitcher, name ends with "WidgetSwitcher_0", unique occurrence
	static std::optional<UObject*> FindGameMenuWidgetSwitcher();

	/// Find a WrapBox in the GameMenu (used as parent for UI elements)
	/// Pattern: Type is WrapBox, name ends with "SaveFileWrapBox_1"
	static std::optional<UObject*> FindGameMenuWrapBox();

	/// Find a WidgetTree in GameMenu (manages widget hierarchy)
	/// Pattern: Type is WidgetTree, name ends with "WidgetTree_0", unique occurrence in GameMenu
	static std::optional<UObject*> FindGameMenuWidgetTree();

	/// Find a GameMenuTitle_C by name (used for section headers)
	/// Pattern: Type is GameMenuTitle_C, name ends with provided nameSuffix
	static std::optional<UObject*> FindGameMenuTitle(std::wstring_view nameSuffix);
};
