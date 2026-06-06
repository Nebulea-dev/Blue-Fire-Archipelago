#include <Helper/UnrealObjectQueries.hpp>
#include <Helper/ObjectFinder.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

std::optional<UObject*> UnrealObjectQueries::FindArchipelagoTextbox(std::wstring_view boxName)
{
	return FindWidget(L"EditableTextBox", STR("EditableTextBox /Engine/"), boxName);

}

std::optional<UObject*> UnrealObjectQueries::FindGameMenuController()
{
	return FindWidget(L"GameMenuController_C", STR("GameMenuController_C"), L"GameMenuController_2");
}

std::optional<UObject*> UnrealObjectQueries::FindWidget(
	std::wstring_view typeName,
	std::wstring_view pathPrefix,
	std::wstring_view nameSuffix)
{
	// Use ObjectFinder's existing FindObject with a lambda that checks both path prefix and name suffix
	std::optional<UObject*> result = ObjectFinder::FindObject(std::wstring(typeName), [pathPrefix, nameSuffix](UObject* obj) {
		const std::wstring fullPath = obj->GetFullName();
		const std::wstring objectName = obj->GetNamePrivate().ToString();

		// Both conditions must be met:
		// 1. Full path starts with the prefix
		// 2. Object name ends with the suffix
		return fullPath.starts_with(pathPrefix) && objectName.ends_with(nameSuffix);
	});

	return result;
}

std::optional<UObject*> UnrealObjectQueries::FindGameMenuWidgetSwitcher()
{
	// Find the one WidgetSwitcher object that:
	// - has full path starting with /Engine/
	// - ends with WidgetSwitcher_0
	// - contains only the substring "WidgetSwitcher_0" once (ensures uniqueness)
	// - contains "GameMenu_C" in its full path
	std::optional<UObject*> result = ObjectFinder::FindObject(L"WidgetSwitcher", [](UObject* obj) {
		const std::wstring fullPath = obj->GetFullName();
		const std::wstring objectName = obj->GetNamePrivate().ToString();

		// Check all criteria
		bool startsWithEngine = fullPath.starts_with(STR("WidgetSwitcher /Engine/"));
		bool endsWithWidgetSwitcher = objectName.ends_with(STR("WidgetSwitcher_0"));
		bool uniqueOccurrence = fullPath.find(STR("WidgetSwitcher_0")) == fullPath.rfind(STR("WidgetSwitcher_0"));
		bool hasGameMenu = fullPath.find(STR("GameMenu_C")) != std::string::npos;

		return startsWithEngine && endsWithWidgetSwitcher && uniqueOccurrence && hasGameMenu;
	});

	return result;
}

std::optional<UObject*> UnrealObjectQueries::FindGameMenuWrapBox()
{
	// Find the one WrapBox object that:
	// - has full path starting with /Engine/
	// - ends with SaveFileWrapBox_1
	std::optional<UObject*> result = ObjectFinder::FindObject(L"WrapBox", [](UObject* obj) {
		const std::wstring fullPath = obj->GetFullName();
		const std::wstring objectName = obj->GetNamePrivate().ToString();

		return fullPath.starts_with(STR("WrapBox /Engine/")) && objectName.ends_with(STR("SaveFileWrapBox_1"));
	});

	return result;
}

std::optional<UObject*> UnrealObjectQueries::FindGameMenuWidgetTree()
{
	// Find the one WidgetTree object that:
	// - has full path starting with /Engine/
	// - ends with WidgetTree_0
	// - contains only the substring "WidgetTree_0" once (ensures uniqueness)
	// - contains "GameMenu_C" in its full path
	std::optional<UObject*> result = ObjectFinder::FindObject(L"WidgetTree", [](UObject* obj) {
		const std::wstring fullPath = obj->GetFullName();
		const std::wstring objectName = obj->GetNamePrivate().ToString();

		// Check all criteria
		bool startsWithEngine = fullPath.starts_with(STR("WidgetTree /Engine/"));
		bool endsWithWidgetTree = objectName.ends_with(STR("WidgetTree_0"));
		bool uniqueOccurrence = fullPath.find(STR("WidgetTree_0")) == fullPath.rfind(STR("WidgetTree_0"));
		bool hasGameMenu = fullPath.find(STR("GameMenu_C")) != std::string::npos;

		return startsWithEngine && endsWithWidgetTree && uniqueOccurrence && hasGameMenu;
	});

	return result;
}

std::optional<UObject*> UnrealObjectQueries::FindGameMenuTitle(std::wstring_view nameSuffix)
{
	return FindWidget(L"GameMenuTitle_C", L"GameMenuTitle_C /Engine/", nameSuffix);
}

std::optional<UObject*> UnrealObjectQueries::FindGameInstance()
{
	return FindWidget(L"BlueFire_Game_Instance_C", L"BlueFire_Game_Instance_C /Engine/", L"");
}

std::optional<UObject*> UnrealObjectQueries::FindGameMenu()
{
	return FindWidget(L"GameMenu_C", L"GameMenu_C /Engine/", L"");
}