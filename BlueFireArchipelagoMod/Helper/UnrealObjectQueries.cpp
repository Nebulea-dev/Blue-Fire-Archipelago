#include <Helper/UnrealObjectQueries.hpp>
#include <Helper/ObjectFinder.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <ItemManager.hpp>

using namespace RC;
using namespace Unreal;

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

std::optional<UObject*> UnrealObjectQueries::FindPlayerCharacter()
{
	std::vector<UObject*> foundObjects;

	UObjectGlobals::FindAllOf(L"Player_Character_BP_C", foundObjects);

	for(UObject* foundObj : foundObjects)
	{
		// Find the one Player_Character_BP_C object that:
		// - has full path starting with /Game/
		// - contains "Master_DLC_VoidMaster" in its full path
		const std::wstring fullPath = foundObj->GetFullName();

		// Check all criteria
		bool startsWithEngine = fullPath.starts_with(STR("Player_Character_BP_C /Game/"));
		bool hasVoidDLC = fullPath.find(STR("Master_DLC_VoidMaster")) != std::string::npos;

		if (startsWithEngine && hasVoidDLC)
		{
			return foundObj;
		}
	}

	return std::nullopt;
}

std::optional<UObject*> UnrealObjectQueries::FindGameMenuWidgetSwitcher()
{
	// Find the one GameMenuController_C object that:
	// - ends with GameMenuController_2
	std::optional<UObject*> gameMenuController = ObjectFinder::FindObject(L"GameMenuController_C", [](UObject* obj) {
		const std::wstring objectName = obj->GetNamePrivate().ToString();

		return objectName.ends_with(STR("GameMenuController_2"));
	});

	// Couldn't find the GameMenuController_C
	if(!gameMenuController.has_value())
	{
		return std::nullopt;
	}

	// We CANNOT find the WidgetSwitcher object directly.
	// The reason for this is that upon loading a game and exiting to main menu,
	// Blue Fire will re-create a WidgetSwitcher but with different numbers, see :
	// WidgetSwitcher /Engine/Transient.GameEngine_2147482624:BlueFire_Game_Instance_C_2147482618.GameMenu_C_2147482108.WidgetTree_0.WidgetSwitcher_0
    // WidgetSwitcher /Engine/Transient.GameEngine_2147482624:BlueFire_Game_Instance_C_2147482618.GameMenu_C_2147482538.WidgetTree_0.WidgetSwitcher_0

	// The correct way is to look for the GameMenuController_C object and look
	// for the GameMenu property

	// Get the "GameMenu" property of the GameMenuController_C
	UObject** gameMenu = gameMenuController.value()->GetValuePtrByPropertyNameInChain<UObject*>(L"GameMenu");
	if (gameMenu == nullptr)
	{
		Output::send<LogLevel::Error>(STR("Could not find GameMenu property in GameMenuController_C\n"));
		return std::nullopt;
	}

	// Get the "WidgetSwitcher_0" property of the GameMenu
	UObject** widgetSwitcher = (*gameMenu)->GetValuePtrByPropertyNameInChain<UObject*>(L"WidgetSwitcher_0");
	if (widgetSwitcher == nullptr)
	{
		Output::send<LogLevel::Error>(STR("Could not find WidgetSwitcher_0 property in GameMenu\n"));
		return std::nullopt;
	}

	return *widgetSwitcher;
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

FStructProperty* UnrealObjectQueries::GetStructProperty(UObject* owner, const wchar_t* propertyName)
{
	if (!owner)
	{
		Output::send<LogLevel::Error>(STR("owner is null in GetStructProperty\n"));
		return nullptr;
	}

	FStructProperty* property = static_cast<FStructProperty*>(owner->GetPropertyByNameInChain(propertyName));
	if (!property)
	{
		Output::send<LogLevel::Error>(STR("Could not find {} property\n"), propertyName);
		return nullptr;
	}

	return property;
}

void* UnrealObjectQueries::GetStructPropertyValue(UObject* owner, const wchar_t* propertyName)
{
	if (!owner)
	{
		Output::send<LogLevel::Error>(STR("owner is null in GetStructPropertyValue\n"));
		return nullptr;
	}

	FStructProperty* property = GetStructProperty(owner, propertyName);
	if (!property)
		return nullptr;

	UStruct* structDef = property->GetStruct();
	if (!structDef)
	{
		Output::send<LogLevel::Error>(STR("Could not get struct from {} property\n"), propertyName);
		return nullptr;
	}

	void* value = property->ContainerPtrToValuePtr<void>(owner);
	if (!value)
	{
		Output::send<LogLevel::Error>(STR("Could not get {} value pointer\n"), propertyName);
		return nullptr;
	}

	return value;
}

TArray<inventoryItem>* UnrealObjectQueries::GetInventoryFromGameInstance()
{
	std::optional<UObject*> gameInstance = FindGameInstance();
	if (!gameInstance.has_value())
	{
		Output::send<LogLevel::Error>(STR("Could not find game instance\n"));
		return nullptr;
	}

	FStructProperty* playerStatsProperty = GetStructProperty(gameInstance.value(), L"PlayerStats");
	if (!playerStatsProperty)
		return nullptr;

	void* playerStats = GetStructPropertyValue(gameInstance.value(), L"PlayerStats");
	if (!playerStats)
		return nullptr;

	UStruct* playerStatsStruct = playerStatsProperty->GetStruct();
	if (!playerStatsStruct)
	{
		Output::send<LogLevel::Error>(STR("Could not get struct from PlayerStats property\n"));
		return nullptr;
	}

	FStructProperty* inventoryProperty = static_cast<FStructProperty*>(playerStatsStruct->GetPropertyByNameInChain(L"Inventory"));
	if (!inventoryProperty)
	{
		Output::send<LogLevel::Error>(STR("Could not find Inventory property in PlayerStats\n"));
		return nullptr;
	}

	TArray<inventoryItem>* inventory = inventoryProperty->ContainerPtrToValuePtr<TArray<inventoryItem>>(playerStats);
	if (!inventory)
	{
		Output::send<LogLevel::Error>(STR("Could not get Inventory value pointer\n"));
		return nullptr;
	}

	return inventory;
}

TArray<inventoryItem>* UnrealObjectQueries::GetPassiveInventoryFromGameInstance()
{
	std::optional<UObject*> gameInstance = FindGameInstance();
	if (!gameInstance.has_value())
	{
		Output::send<LogLevel::Error>(STR("Could not find game instance\n"));
		return nullptr;
	}

	FStructProperty* playerStatsProperty = GetStructProperty(gameInstance.value(), L"PlayerStats");
	if (!playerStatsProperty)
		return nullptr;

	void* playerStats = GetStructPropertyValue(gameInstance.value(), L"PlayerStats");
	if (!playerStats)
		return nullptr;

	UStruct* playerStatsStruct = playerStatsProperty->GetStruct();
	if (!playerStatsStruct)
	{
		Output::send<LogLevel::Error>(STR("Could not get struct from PlayerStats property\n"));
		return nullptr;
	}

	FStructProperty* passiveInventoryProperty = static_cast<FStructProperty*>(playerStatsStruct->GetPropertyByNameInChain(L"PassiveInventory"));
	if (!passiveInventoryProperty)
	{
		Output::send<LogLevel::Error>(STR("Could not find PassiveInventory property in PlayerStats\n"));
		return nullptr;
	}

	TArray<inventoryItem>* passiveInventory = passiveInventoryProperty->ContainerPtrToValuePtr<TArray<inventoryItem>>(playerStats);
	if (!passiveInventory)
	{
		Output::send<LogLevel::Error>(STR("Could not get PassiveInventory value pointer\n"));
		return nullptr;
	}

	return passiveInventory;
}

bool UnrealObjectQueries::StackItemInInventory(TArray<inventoryItem>* inventory, uint32_t itemID, uint32_t amount)
{
	if (!inventory)
	{
		Output::send<LogLevel::Error>(STR("inventory is null in StackItemInInventory\n"));
		return false;
	}

	for (int32_t i = 0; i < inventory->Num(); i++)
	{
		inventoryItem item = (*inventory)[i];
		if (item.type != 0) continue;
		if (item.item != itemID) continue;

		item.amount += amount;
		(*inventory)[i] = item;
		return true;
	}

	return false;
}

bool UnrealObjectQueries::GetStructPropertyAndValue(UObject* owner, const wchar_t* propertyName,
	FStructProperty*& outProperty, void*& outValue)
{
	if (!owner)
	{
		Output::send<LogLevel::Error>(STR("owner is null in GetStructPropertyAndValue\n"));
		return false;
	}

	outProperty = GetStructProperty(owner, propertyName);
	if (!outProperty)
		return false;

	UStruct* structDef = outProperty->GetStruct();
	if (!structDef)
	{
		Output::send<LogLevel::Error>(STR("Could not get struct from {} property\n"), propertyName);
		return false;
	}

	outValue = outProperty->ContainerPtrToValuePtr<void>(owner);
	if (!outValue)
	{
		Output::send<LogLevel::Error>(STR("Could not get {} value pointer\n"), propertyName);
		return false;
	}

	return true;
}

