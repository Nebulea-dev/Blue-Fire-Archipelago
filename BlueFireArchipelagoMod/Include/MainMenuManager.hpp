#pragma once

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

using namespace RC;
using namespace Unreal;

// Forward declarations
class HookHelper;
struct ObjectCreateListener;

/*******************************************************************************
 * MainMenuManager
 *
 * @brief   Manages the Archipelago connection menu UI.
 *
 *          Responsible for creating, updating, and managing the main menu
 *          interface for Archipelago server connection (IP, username,
 *          password inputs). Handles user navigation, textbox creation,
 *          and menu submission.
 */
class MainMenuManager
{
	public:
	/*******************************************************************************
	 * @fn      MainMenuManager
	 *
	 * @brief   Constructor - initializes menu hooks and callbacks.
	 *
	 *          Registers menu navigation hooks (UP_KEY, DOWN_KEY) and object
	 *          creation callbacks (EditableTextBox). Must be called after
	 *          HookHelper is initialized.
	 *
	 * @return  none
	 */
	MainMenuManager();

	// ============================================================
	// Menu Navigation - Handle user input and menu navigation
	// ============================================================

	/*******************************************************************************
	 * @fn      OnReturnPressed
	 *
	 * @brief   Handle RETURN key press in the Archipelago menu.
	 *
	 *          Processes either menu submission (when focused on submit button)
	 *          or menu exit (when not in menu). Triggers Archipelago server
	 *          connection when submit is activated.
	 *
	 * @return  none
	 */
	void OnReturnPressed();

	/*******************************************************************************
	 * @fn      UpKeyHook
	 *
	 * @brief   Static hook callback for UP key press in menu.
	 *
	 *          Moves focus up through menu items (IP → Username → Password).
	 *          This is a static callback required by Unreal's hook system.
	 *
	 * @param   Context     - Unreal object context
	 * @param   Stack       - Function stack frame
	 * @param   RESULT_DECL - Result declaration pointer
	 *
	 * @return  true to prevent original function, false to allow it
	 */
	static bool UpKeyHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	/*******************************************************************************
	 * @fn      DownKeyHook
	 *
	 * @brief   Static hook callback for DOWN key press in menu.
	 *
	 *          Moves focus down through menu items (Submit button is last).
	 *          This is a static callback required by Unreal's hook system.
	 *
	 * @param   Context     - Unreal object context
	 * @param   Stack       - Function stack frame
	 * @param   RESULT_DECL - Result declaration pointer
	 *
	 * @return  true to prevent original function, false to allow it
	 */
	static bool DownKeyHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	/*******************************************************************************
	 * @fn      OnEditableTextBoxCreated
	 *
	 * @brief   Static callback when an EditableTextBox is created.
	 *
	 *          Detects when the original menu textbox is created and triggers
	 *          creation of Archipelago UI elements. This is a static callback
	 *          required by the object creation listener.
	 *
	 * @param   object - Pointer to the created object
	 * @param   index  - Index in the object array
	 *
	 * @return  none
	 */
	static void OnEditableTextBoxCreated(const UObjectBase* object, int32 index);

	//  ============================================================
	//  Menu State - Check and manage menu state
	//  ============================================================

	/*******************************************************************************
	 * @fn      IsUserInArchipelagoMenu
	 *
	 * @brief   Check if the user is currently in the Archipelago menu.
	 *
	 *          Queries the WidgetSwitcher to determine if the active menu
	 *          page is the Archipelago connection menu.
	 *
	 * @return  true if in Archipelago menu, false otherwise
	 */
	bool IsUserInArchipelagoMenu();

	/*******************************************************************************
	 * @fn      GetMenuFocusIndex
	 *
	 * @brief   Get the current menu item index that has focus.
	 *
	 * @return  Menu index (1=IP, 2=Username, 3=Password, 4=Submit, 0=none)
	 */
	int GetMenuFocusIndex() const { return menuFocusIndex; }

	/*******************************************************************************
	 * @fn      SetMenuFocusIndex
	 *
	 * @brief   Set the menu item index that should have focus.
	 *
	 * @param   index - Menu index (1=IP, 2=Username, 3=Password, 4=Submit)
	 *
	 * @return  none
	 */
	void SetMenuFocusIndex(int index) { menuFocusIndex = index; }

	// ============================================================
	// Menu UI Creation - Create and manage textbox elements
	// ============================================================

	/*******************************************************************************
	 * @fn      CreateMenuTextboxes
	 *
	 * @brief   Create the Archipelago connection menu textbox UI elements.
	 *
	 *          Orchestrates the creation of three textboxes (IP, Username,
	 *          Password) with proper styling, hierarchy, and layout.
	 *          Only creates once - subsequent calls are ignored.
	 *
	 * @return  none
	 */
	void CreateMenuTextboxes();

	/*******************************************************************************
	 * @fn      DeleteOriginalTextbox
	 *
	 * @brief   Delete the original textbox that was replaced.
	 *
	 *          Removes the original single textbox from the menu before
	 *          the Archipelago textboxes are created, keeping the UI clean.
	 *
	 * @return  none
	 */
	void DeleteOriginalTextbox();

	private:
	/*******************************************************************************
	 * @brief   Member Variables
	 */

	/// Menu state: Whether textboxes have been created
	bool bMenuTextboxesCreated;

	/// Menu state: Current menu item index with focus (1-4, 0 for none)
	int menuFocusIndex;

	/*******************************************************************************
	 * @fn      HandleUpKeyPress
	 *
	 * @brief   Handle UP key press in the menu.
	 *
	 *          Moves focus up through menu items. Wraps around from first to
	 *          last item. Delegates to UpdateMenuFocus() to apply changes.
	 *
	 * @return  true if focus was updated successfully, false otherwise
	 */
	bool HandleUpKeyPress();

	/*******************************************************************************
	 * @fn      HandleDownKeyPress
	 *
	 * @brief   Handle DOWN key press in the menu.
	 *
	 *          Moves focus down through menu items. Wraps around from last to
	 *          first item. Delegates to UpdateMenuFocus() to apply changes.
	 *
	 * @return  true if focus was updated successfully, false otherwise
	 */
	bool HandleDownKeyPress();

	/*******************************************************************************
	 * @fn      HandleEditableTextBoxCreated
	 *
	 * @brief   Handle EditableTextBox object creation event.
	 *
	 *          Called when an EditableTextBox is created. Used to detect the
	 *          original menu textbox and trigger creation of Archipelago UI.
	 *
	 * @param   object - Pointer to the created object
	 * @param   index  - Index in the object array
	 *
	 * @return  none
	 */
	void HandleEditableTextBoxCreated(const UObjectBase* object, int32 index);

	/*******************************************************************************
	 * @fn      UpdateMenuFocus
	 *
	 * @brief   Update focus on the correct menu item based on menuFocusIndex.
	 *
	 *          Internal helper that:
	 *          1. Finds all required UI widgets (textboxes, controller, title)
	 *          2. Finds required Unreal functions (SetFocus, DownKey)
	 *          3. Calls SetFocus on the appropriate widget based on menuFocusIndex
	 *          4. Handles submit action (case 4)
	 *
	 * @return  true if focus was successfully updated, false if any widget/function not found
	 */
	bool UpdateMenuFocus();

	/*******************************************************************************
	 * @fn      FindUIContainers
	 *
	 * @brief   Find and validate all required UI container objects.
	 *
	 *          Step 1 of CreateMenuTextboxes() orchestration. Locates the
	 *          WrapBox, WidgetTree, and Font object needed for UI creation.
	 *
	 * @param   outWrapBox     - Output: Reference to WrapBox pointer
	 * @param   outWidgetTree  - Output: Reference to WidgetTree pointer
	 * @param   outFontObject  - Output: Reference to Font object pointer
	 *
	 * @return  true if all containers found, false if any is missing
	 */
	bool FindUIContainers(UObject*& outWrapBox, UObject*& outWidgetTree, UObject*& outFontObject);

	/*******************************************************************************
	 * @fn      CreateWrapBoxSlots
	 *
	 * @brief   Create WrapBoxSlot objects and add them to the parent WrapBox.
	 *
	 *          Step 2 of CreateMenuTextboxes() orchestration. Creates three
	 *          slot containers that will hold the textbox widgets.
	 *
	 * @param   wrapBox   - The parent WrapBox to add slots to
	 * @param   outSlots  - Output: Array of created slot pointers
	 *
	 * @return  true if slots created successfully, false if creation failed
	 */
	bool CreateWrapBoxSlots(UObject* wrapBox, TArray<UObject*>& outSlots);

	/*******************************************************************************
	 * @fn      CreateEditableTextBoxes
	 *
	 * @brief   Create EditableTextBox objects and prepare hint text array.
	 *
	 *          Step 3 of CreateMenuTextboxes() orchestration. Creates three
	 *          textbox widgets for IP, Username, and Password input.
	 *
	 * @param   widgetTree       - The WidgetTree to add textboxes to
	 * @param   outTextboxes     - Output: Array of created textbox pointers
	 *
	 * @return  true if textboxes created successfully, false if creation failed
	 */
	bool CreateEditableTextBoxes(UObject* widgetTree, TArray<UObject*>& outTextboxes);

	/*******************************************************************************
	 * @fn      SetupWidgetHierarchy
	 *
	 * @brief   Link parent/child relationships between slots and textboxes.
	 *
	 *          Step 4 of CreateMenuTextboxes() orchestration. Establishes the
	 *          parent-child relationships needed for proper widget layout.
	 *
	 * @param   slots       - Array of WrapBoxSlot objects
	 * @param   textboxes   - Array of EditableTextBox objects
	 * @param   wrapBox     - The parent WrapBox container
	 *
	 * @return  true if hierarchy established successfully, false otherwise
	 */
	bool SetupWidgetHierarchy(const TArray<UObject*>& slots, const TArray<UObject*>& textboxes, UObject* wrapBox);

	/*******************************************************************************
	 * @fn      ConfigureTextboxStyles
	 *
	 * @brief   Configure all styling properties (fonts, colors, justification).
	 *
	 *          Step 5 of CreateMenuTextboxes() orchestration. Sets font size,
	 *          color, text justification, and other visual properties.
	 *
	 * @param   textboxes   - Array of EditableTextBox objects to style
	 * @param   fontObject  - The Font object to apply
	 *
	 * @return  true if styling applied successfully, false otherwise
	 */
	bool ConfigureTextboxStyles(const TArray<UObject*>& textboxes, UObject* fontObject);

	/*******************************************************************************
	 * @fn      SubmitMenuConnection
	 *
	 * @brief   Submit the menu and send connection request to Archipelago.
	 *
	 *          Called when the user presses submit (menu index 4). Gathers
	 *          credentials from textboxes and delegates to ArchipelagoManager
	 *          for server connection.
	 *
	 * @return  none
	 */
	void SubmitMenuConnection();

	//TODO : Make this pretty docs
	static bool StartGameHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);
	static bool CancelWriteHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);

	bool bNoHookGameStartOnce{false};
};