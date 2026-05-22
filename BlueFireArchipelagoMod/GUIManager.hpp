#pragma once

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

using namespace RC;
using namespace Unreal;

class GUIManager
{
	public:
	static bool UpdateFileNamingFocus();
	static bool isUserInArchipelagoMenu();
	static void CreateTextboxes();
	static void deleteOldTextbox();
	static inline bool bTextboxesCreated{false};
	static inline int fileNamingActiveWidgetIndex{0};

	private:
	/// Helper methods for CreateTextboxes() - each handles one concern

	/// Step 1: Find and validate all required UI container objects
	/// Returns false if any container cannot be found
	static bool FindUIContainers(UObject*& outWrapBox, UObject*& outWidgetTree, UObject*& outFontObject);

	/// Step 2: Create WrapBoxSlot objects and add them to the parent WrapBox
	/// Returns false if slots cannot be created
	static bool CreateWrapBoxSlots(UObject* wrapBox, TArray<UObject*>& outSlots);

	/// Step 3: Create EditableTextBox objects and set hint text on them
	/// Returns false if textboxes cannot be created
	static bool CreateEditableTextBoxes(UObject* widgetTree, TArray<UObject*>& outTextboxes);

	/// Step 4: Link parent/child relationships between slots and textboxes
	/// Returns false if hierarchy cannot be established
	static bool SetupWidgetHierarchy(const TArray<UObject*>& slots, const TArray<UObject*>& textboxes, UObject* wrapBox);

	/// Step 5: Configure all styling properties (fonts, colors, justification, etc.)
	/// Returns false if styling cannot be applied
	static bool ConfigureTextboxStyles(const TArray<UObject*>& textboxes, UObject* fontObject);
};