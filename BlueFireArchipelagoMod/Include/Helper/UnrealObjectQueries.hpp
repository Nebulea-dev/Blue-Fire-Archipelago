#pragma once

#include <optional>
#include <Unreal/UObject.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

// Forward declaration to avoid circular dependency
struct inventoryItem;

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
	 * @fn      FindPlayerCharacter
	 *
	 * @brief   Find the player character instance.
	 *
	 * @return  optional containing character pointer, empty if not found
	 */
	static std::optional<UObject*> FindPlayerCharacter();

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

	/*******************************************************************************
	 * @fn      GetStructProperty
	 *
	 * @brief   Gets a struct property from an object by name.
	 *
	 * @param   owner          - Object to get property from
	 * @param   propertyName   - Name of the struct property
	 *
	 * @return  FStructProperty pointer or nullptr if not found
	 */
	static FStructProperty* GetStructProperty(UObject* owner, const wchar_t* propertyName);

	/*******************************************************************************
	 * @fn      GetStructPropertyValue
	 *
	 * @brief   Gets a value pointer from a struct property in one call.
	 *
	 *          Combines: GetProperty → GetStruct → ContainerPtrToValuePtr
	 *
	 * @param   owner          - Object to get property from
	 * @param   propertyName   - Name of the struct property
	 *
	 * @return  void pointer to struct value or nullptr if not found
	 */
	static void* GetStructPropertyValue(UObject* owner, const wchar_t* propertyName);

	/*******************************************************************************
	 * @fn      GetStructPropertyAndValue
	 *
	 * @brief   Gets both struct property and its value pointer in one call.
	 *
	 *          Combines all steps: Get property → check → Get struct → check → Get value → check
	 *          Eliminates need for multiple checks when both the property and value are needed.
	 *
	 * @param   owner          - Object to get property from
	 * @param   propertyName   - Name of the struct property
	 * @param   outProperty    - Output parameter for the FStructProperty pointer
	 * @param   outValue       - Output parameter for the struct value pointer
	 *
	 * @return  true if both property and value were found, false otherwise
	 */
	static bool GetStructPropertyAndValue(UObject* owner, const wchar_t* propertyName,
		FStructProperty*& outProperty, void*& outValue);

	/*******************************************************************************
	 * @fn      GetInventoryFromGameInstance
	 *
	 * @brief   Gets player inventory array from game instance.
	 *
	 *          Retrieves PlayerStats → Inventory chain from game instance.
	 *
	 * @return  TArray<inventoryItem> pointer or nullptr if not found
	 */
	static TArray<inventoryItem>* GetInventoryFromGameInstance();

	/*******************************************************************************
	 * @fn      GetPassiveInventoryFromGameInstance
	 *
	 * @brief   Gets passive inventory array from game instance.
	 *
	 *          Retrieves PlayerStats → PassiveInventory chain from game instance.
	 *
	 * @return  TArray<inventoryItem> pointer or nullptr if not found
	 */
	static TArray<inventoryItem>* GetPassiveInventoryFromGameInstance();

	/*******************************************************************************
	 * @fn      GetNestedPropertyValue (2-level)
	 *
	 * @brief   Gets a typed value from a nested struct property in one call.
	 *
	 *          Handles entire chain: Get parent property → Get struct → Get child property → Get value
	 *          Eliminates repetitive multi-step pattern for accessing nested properties.
	 *
	 * @tparam  T              - Type of the value to retrieve
	 * @param   owner          - Object containing the parent property
	 * @param   parentProperty - Name of the parent struct property (e.g., "PlayerAbilities")
	 * @param   childProperty  - Name of the child property within the struct (e.g., "Dash_8_...")
	 *
	 * @return  Typed pointer to the value, or nullptr if any step fails
	 */
	template<typename T>
	static T* GetNestedPropertyValue(UObject* owner, const wchar_t* parentProperty, const wchar_t* childProperty);

	/*******************************************************************************
	 * @fn      GetNestedPropertyValue (3-level)
	 *
	 * @brief   Gets a typed value from a deeply nested struct property in one call.
	 *
	 *          Handles entire chain: Get parent → Get struct → Get middle → Get struct → Get child → Get value
	 *          For accessing properties three levels deep (e.g., WidgetStyle.Font.FontObject).
	 *
	 * @tparam  T                 - Type of the value to retrieve
	 * @param   owner             - Object containing the parent property
	 * @param   parentProperty    - Name of the parent struct property (e.g., "WidgetStyle")
	 * @param   middleProperty    - Name of the middle struct property (e.g., "Font")
	 * @param   childProperty     - Name of the child property within the middle struct (e.g., "FontObject")
	 *
	 * @return  Typed pointer to the value, or nullptr if any step fails
	 */
	template<typename T>
	static T* GetNestedPropertyValue(UObject* owner, const wchar_t* parentProperty, const wchar_t* middleProperty, const wchar_t* childProperty);

	/*******************************************************************************
	 * @fn      StackItemInInventory
	 *
	 * @brief   Stacks an item in inventory or adds it if not found.
	 *
	 *          Searches inventory for item with matching ID and increments amount.
	 *          If not found, does nothing and returns false.
	 *
	 * @param   inventory  - Inventory array to search
	 * @param   itemID     - Item ID to find and stack
	 * @param   amount     - Amount to add (default 1)
	 *
	 * @return  true if item was found and stacked, false otherwise
	 */
	static bool StackItemInInventory(TArray<inventoryItem>* inventory, uint32_t itemID, uint32_t amount = 1);
};

template<typename T>
T* UnrealObjectQueries::GetNestedPropertyValue(UObject* owner, const wchar_t* parentProperty, const wchar_t* childProperty)
{
	if (!owner)
	{
		Output::send<LogLevel::Error>(STR("owner is null in GetNestedPropertyValue\n"));
		return nullptr;
	}

	FStructProperty* parentProp = GetStructProperty(owner, parentProperty);
	if (!parentProp)
		return nullptr;

	UStruct* parentStruct = parentProp->GetStruct();
	if (!parentStruct)
	{
		Output::send<LogLevel::Error>(STR("Could not get struct from {} property\n"), parentProperty);
		return nullptr;
	}

	void* parentValue = parentProp->template ContainerPtrToValuePtr<void>(owner);
	if (!parentValue)
	{
		Output::send<LogLevel::Error>(STR("Could not get {} value pointer\n"), parentProperty);
		return nullptr;
	}

	FProperty* childProp = parentStruct->GetPropertyByNameInChain(childProperty);
	if (!childProp)
	{
		Output::send<LogLevel::Error>(STR("Could not find {} property in {}\n"), childProperty, parentProperty);
		return nullptr;
	}

	T* childValue = childProp->template ContainerPtrToValuePtr<T>(parentValue);
	if (!childValue)
	{
		Output::send<LogLevel::Error>(STR("Could not get {} value pointer\n"), childProperty);
		return nullptr;
	}

	return childValue;
}

template<typename T>
T* UnrealObjectQueries::GetNestedPropertyValue(UObject* owner, const wchar_t* parentProperty, const wchar_t* middleProperty, const wchar_t* childProperty)
{
	if (!owner)
	{
		Output::send<LogLevel::Error>(STR("owner is null in GetNestedPropertyValue (3-level)\n"));
		return nullptr;
	}

	FStructProperty* parentProp = GetStructProperty(owner, parentProperty);
	if (!parentProp)
		return nullptr;

	UStruct* parentStruct = parentProp->GetStruct();
	if (!parentStruct)
	{
		Output::send<LogLevel::Error>(STR("Could not get struct from {} property\n"), parentProperty);
		return nullptr;
	}

	void* parentValue = parentProp->template ContainerPtrToValuePtr<void>(owner);
	if (!parentValue)
	{
		Output::send<LogLevel::Error>(STR("Could not get {} value pointer\n"), parentProperty);
		return nullptr;
	}

	FStructProperty* middleProp = static_cast<FStructProperty*>(parentStruct->GetPropertyByNameInChain(middleProperty));
	if (!middleProp)
	{
		Output::send<LogLevel::Error>(STR("Could not find {} property in {}\n"), middleProperty, parentProperty);
		return nullptr;
	}

	UStruct* middleStruct = middleProp->GetStruct();
	if (!middleStruct)
	{
		Output::send<LogLevel::Error>(STR("Could not get struct from {} property\n"), middleProperty);
		return nullptr;
	}

	void* middleValue = middleProp->template ContainerPtrToValuePtr<void>(parentValue);
	if (!middleValue)
	{
		Output::send<LogLevel::Error>(STR("Could not get {} value pointer\n"), middleProperty);
		return nullptr;
	}

	FProperty* childProp = middleStruct->GetPropertyByNameInChain(childProperty);
	if (!childProp)
	{
		Output::send<LogLevel::Error>(STR("Could not find {} property in {}\n"), childProperty, middleProperty);
		return nullptr;
	}

	T* childValue = childProp->template ContainerPtrToValuePtr<T>(middleValue);
	if (!childValue)
	{
		Output::send<LogLevel::Error>(STR("Could not get {} value pointer\n"), childProperty);
		return nullptr;
	}

	return childValue;
}
