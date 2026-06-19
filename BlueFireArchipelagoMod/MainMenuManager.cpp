#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include <Helper/HookHelper.hpp>
#include <Helper/ObjectCreateListener.hpp>
#include <Helper/ObjectFinder.hpp>
#include <Helper/UnrealObjectQueries.hpp>
#include <MainMenuManager.hpp>
#include <ArchipelagoManager.hpp>
#include <ArchipelagoModConfig.hpp>
#include <BlueFireArchipelagoMod.hpp>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

MainMenuManager::MainMenuManager()
	: bMenuTextboxesCreated(false), bMainMenuLoaded(false), menuFocusIndex(0), mainMenuLoadingState(0)
{
	if (!BlueFireArchipelagoMod::hookManager || !BlueFireArchipelagoMod::objectListener)
	{
		Output::send<LogLevel::Error>(STR("MainMenuManager::Init - Invalid hookManager or objectListener\n"));
		return;
	}

	// Listen to object creation for EditableTextBox controls
	BlueFireArchipelagoMod::objectListener->registerObjectCallback(std::wstring(L"EditableTextBox"), OnEditableTextBoxCreated);

	// Register menu navigation hooks
	BlueFireArchipelagoMod::hookManager->registerPreHook(Hooks::UP_KEY, UpKeyHook);
	BlueFireArchipelagoMod::hookManager->registerPreHook(Hooks::DOWN_KEY, DownKeyHook);
	BlueFireArchipelagoMod::hookManager->registerPreHook(L"Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:StartGame", StartGameHook);
	BlueFireArchipelagoMod::hookManager->registerPreHook(L"Function /Game/BlueFire/HUD/Menu/GameMenu.GameMenu_C:CancelWrite", CancelWriteHook);
	BlueFireArchipelagoMod::hookManager->registerPreHook(L"Function /Game/BlueFire/Maps/Menu/MainMenuSub.MainMenuSub_C:OpenSettingsSave", MainMenuDoneLoading);

	Output::send<LogLevel::Verbose>(STR("MainMenuManager instance created\n"));
}


bool MainMenuManager::UpdateMenuFocus()
{
    // Step 1 - Get all the references to the widgets that we will need.
    // This includes the 3 text boxes, the game controller and the start button


    // Find the three Archipelago textboxes using helpers
    std::optional<UObject*> IPEditableTextBox = UnrealObjectQueries::FindArchipelagoTextbox(GameObjects::TEXTBOX_IP);
    if (!IPEditableTextBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the IP textbox\n"));
        return false;
    }

    std::optional<UObject*> UsernameEditableTextBox = UnrealObjectQueries::FindArchipelagoTextbox(GameObjects::TEXTBOX_USERNAME);
    if (!UsernameEditableTextBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the username textbox\n"));
        return false;
    }

    std::optional<UObject*> PasswordEditableTextBox = UnrealObjectQueries::FindArchipelagoTextbox(GameObjects::TEXTBOX_PASSWORD);
    if (!PasswordEditableTextBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the password textbox\n"));
        return false;
    }


    // Step 2 - Find the functions that we will need to call


    // Find the "SetFocus" and "DownKey" functions
    UFunction* SetFocusFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, UnrealClasses::FUNC_WIDGET_SET_FOCUS);
    if(!SetFocusFunc)
    {
        Output::send<LogLevel::Error>(STR("Could not find the SetFocus function\n"));
        return false;
    }


    // Step 3 - Call the method on the right element depending on what is currently focused


    Output::send<LogLevel::Verbose>(STR("Setting focus on menu item with index {} ♥\n"), menuFocusIndex);
    switch(menuFocusIndex)
    {
        case 1:
            IPEditableTextBox.value()->ProcessEvent(SetFocusFunc, nullptr);
            break;
        case 2:
            UsernameEditableTextBox.value()->ProcessEvent(SetFocusFunc, nullptr);
            break;
        case 3:
            PasswordEditableTextBox.value()->ProcessEvent(SetFocusFunc, nullptr);
            break;
    }

    return true;
}


// Helper: construct a UObject of a given UClass with a given outer
static UObject* ConstructWidget(UObject* classObj, UObject* outer, const wchar_t* name)
{
    auto* cls = static_cast<UClass*>(classObj);
    FStaticConstructObjectParameters params(cls);
    params.Outer = outer;
    params.Name  = FName(name, FNAME_Add);
    params.SetFlags = (EObjectFlags)(RF_Transactional);
    return UObjectGlobals::StaticConstructObject(params);
}


/// Step 1: Find and validate all required UI container objects
bool MainMenuManager::FindUIContainers(UObject*& outWrapBox, UObject*& outWidgetTree, UObject*& outFontObject)
{
    // Find the game menu wrap box and widget tree
    std::optional<UObject*> wrapBox = UnrealObjectQueries::FindGameMenuWrapBox();
    if(!wrapBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find WrapBox object\n"));
        return false;
    }

    std::optional<UObject*> widgetTree = UnrealObjectQueries::FindGameMenuWidgetTree();
    if(!widgetTree.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find WidgetTree object\n"));
        return false;
    }

    // Find font object for the text boxes
    UObject* fontObject = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, Assets::FONT_SPECTRAL_BOLD);
    if(!fontObject)
    {
        Output::send<LogLevel::Error>(STR("Could not find font object\n"));
        return false;
    }

    outWrapBox = wrapBox.value();
    outWidgetTree = widgetTree.value();
    outFontObject = fontObject;
    return true;
}


/// Step 2: Create WrapBoxSlot objects and add them to the parent WrapBox
bool MainMenuManager::CreateWrapBoxSlots(UObject* wrapBox, TArray<UObject*>& outSlots)
{
    // Construct the WrapBoxSlot objects that will be the parents of our new text boxes
    UObject* wrapBoxSlotClass = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, UnrealClasses::CLASS_WRAP_BOX_SLOT);
    if (!wrapBoxSlotClass)
    {
        Output::send<LogLevel::Error>(STR("Could not find WrapBoxSlot class\n"));
        return false;
    }

    UObject* IPwrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox, STR("WrapBoxSlotArchipelagoIP"));
    UObject* UsernamewrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox, STR("WrapBoxSlotArchipelagoUsername"));
    UObject* PasswordwrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox, STR("WrapBoxSlotArchipelagoPassword"));

    if (!IPwrapBoxSlot || !UsernamewrapBoxSlot || !PasswordwrapBoxSlot)
    {
        Output::send<LogLevel::Error>(STR("Could not create WrapBoxSlot objects\n"));
        return false;
    }

    // Get the "Slots" property, which is an array of UPanelSlot
    TArray<UObject*>* slotsArray = wrapBox->GetValuePtrByPropertyNameInChain<TArray<UObject*>>(PropertyNames::PROP_SLOTS);
    if (!slotsArray)
    {
        Output::send<LogLevel::Error>(STR("Could not find Slots property in WrapBox\n"));
        return false;
    }

    // Insert our WrapBoxSlots
    slotsArray->Insert(IPwrapBoxSlot, 1);
    slotsArray->Insert(UsernamewrapBoxSlot, 2);
    slotsArray->Insert(PasswordwrapBoxSlot, 3);

    // Populate output array
    outSlots.Add(IPwrapBoxSlot);
    outSlots.Add(UsernamewrapBoxSlot);
    outSlots.Add(PasswordwrapBoxSlot);

    return true;
}


/// Step 3: Create EditableTextBox objects and set hint text on them
bool MainMenuManager::CreateEditableTextBoxes(UObject* widgetTree, TArray<UObject*>& outTextboxes)
{
    UObject* editableTextBoxClass = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, UnrealClasses::CLASS_EDITABLE_TEXT_BOX);
    if (!editableTextBoxClass)
    {
        Output::send<LogLevel::Error>(STR("Could not find EditableTextBox class\n"));
        return false;
    }

    UObject* IPeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree, GameObjects::TEXTBOX_IP);
    UObject* UsernameeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree, GameObjects::TEXTBOX_USERNAME);
    UObject* PasswordeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree, GameObjects::TEXTBOX_PASSWORD);

    if (!IPeditableTextBox || !UsernameeditableTextBox || !PasswordeditableTextBox)
    {
        Output::send<LogLevel::Error>(STR("Could not create EditableTextBox objects\n"));
        return false;
    }

    // Populate output array
    outTextboxes.Add(IPeditableTextBox);
    outTextboxes.Add(UsernameeditableTextBox);
    outTextboxes.Add(PasswordeditableTextBox);

    // Set hint text on each textbox
    TArray<FText*> hints;

    // Note : Yes this is clearly a memeory leak
    // This is fine to do for now, as it's not like we want to ever clear them from memory
    // They'll be deleted when the game closes, and there's only 3 of them, so who cares
    hints.Add(new FText(Strings::HINT_ARCHIPELAGO_SERVER_IP));
    hints.Add(new FText(Strings::HINT_ARCHIPELAGO_USERNAME));
    hints.Add(new FText(Strings::HINT_ARCHIPELAGO_PASSWORD));

    // Set hint text on each textbox
    for(int32_t i = 0; i < outTextboxes.Num() && i < hints.Num(); i++)
    {
        FText* hintTextProperty = outTextboxes[i]->GetValuePtrByPropertyNameInChain<FText>(PropertyNames::PROP_HINT_TEXT);
        if (hintTextProperty != nullptr)
        {
            *hintTextProperty = *(hints[i]);
        }
        else
        {
            Output::send<LogLevel::Warning>(STR("Could not find HintText property on textbox %d\n"), i);
        }
    }

    return true;
}


/// Step 4: Link parent/child relationships between slots and textboxes
bool MainMenuManager::SetupWidgetHierarchy(const TArray<UObject*>& slots, const TArray<UObject*>& textboxes, UObject* wrapBox)
{
    if (slots.Num() != textboxes.Num())
    {
        Output::send<LogLevel::Error>(STR("Slot and textbox arrays have different sizes\n"));
        return false;
    }

    for(int32_t i = 0; i < slots.Num(); i++)
    {
        UObject* slot = slots[i];
        UObject* textBox = textboxes[i];

        // Set the "Parent" property of the WrapBoxSlot to be the parent WrapBox
        UObject** wrapBoxSlotParentProperty = slot->GetValuePtrByPropertyNameInChain<UObject*>(PropertyNames::PROP_PARENT);
        if (wrapBoxSlotParentProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Parent property in WrapBoxSlot\n"));
            return false;
        }
        *wrapBoxSlotParentProperty = wrapBox;

        // Set the "Content" property of the WrapBoxSlot to be our new EditableTextBox
        UObject** wrapBoxSlotContentProperty = slot->GetValuePtrByPropertyNameInChain<UObject*>(PropertyNames::PROP_CONTENT);
        if (wrapBoxSlotContentProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Content property in WrapBoxSlot\n"));
            return false;
        }
        *wrapBoxSlotContentProperty = textBox;

        // Set the "Slot" property of the EditableTextBox
        UObject** editableTextBoxSlotProperty = textBox->GetValuePtrByPropertyNameInChain<UObject*>(PropertyNames::PROP_SLOT);
        if (editableTextBoxSlotProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Slot property in EditableTextBox\n"));
            return false;
        }
        *editableTextBoxSlotProperty = slot;
    }

    return true;
}


/// Step 5: Configure all styling properties (fonts, colors, justification, etc.)
bool MainMenuManager::ConfigureTextboxStyles(const TArray<UObject*>& textboxes, UObject* fontObject)
{
    if (textboxes.Num() == 0)
    {
        Output::send<LogLevel::Error>(STR("No textboxes to style\n"));
        return false;
    }

    // Configure font and style properties (nested struct access) for each textbox
    for(int32_t i = 0; i < textboxes.Num(); i++)
    {
        UObject* textBox = textboxes[i];

        // Get the "WidgetStyle" property
        FStructProperty* StyleWidgetProperty = static_cast<FStructProperty*>(textBox->GetPropertyByNameInChain(PropertyNames::PROP_WIDGET_STYLE));
        if (!StyleWidgetProperty)
        {
            Output::send<LogLevel::Error>(STR("Could not find WidgetStyle property\n"));
            return false;
        }

        // Get the "WidgetStyle.Font" property
        auto StyleWidgetStruct = StyleWidgetProperty->GetStruct();
        if (!StyleWidgetStruct)
        {
            Output::send<LogLevel::Error>(STR("Could not get struct from WidgetStyle property\n"));
            return false;
        }

        auto StyleWidget = StyleWidgetProperty->ContainerPtrToValuePtr<void>(textBox);
        if (!StyleWidget)
        {
            Output::send<LogLevel::Error>(STR("Could not get StyleWidget value pointer\n"));
            return false;
        }

        FStructProperty* FontProperty = static_cast<FStructProperty*>(StyleWidgetStruct->GetPropertyByNameInChain(PropertyNames::PROP_FONT));
        if (!FontProperty)
        {
            Output::send<LogLevel::Error>(STR("Could not find Font property in WidgetStyle\n"));
            return false;
        }

        auto FontStruct = FontProperty->GetStruct();
        if (!FontStruct)
        {
            Output::send<LogLevel::Error>(STR("Could not get struct from Font property\n"));
            return false;
        }

        auto Font = FontProperty->ContainerPtrToValuePtr<void>(StyleWidget);
        if (!Font)
        {
            Output::send<LogLevel::Error>(STR("Could not get Font value pointer\n"));
            return false;
        }

        // Get the "WidgetStyle.Font.FontObject" and "WidgetStyle.Font.Size" properties
        FProperty* FontSizeProperty = static_cast<FProperty*>(FontStruct->GetPropertyByNameInChain(PropertyNames::PROP_FONT_SIZE));
        FStructProperty* FontObjectProperty = static_cast<FStructProperty*>(FontStruct->GetPropertyByNameInChain(PropertyNames::PROP_FONT_OBJECT));
        if (!FontSizeProperty || !FontObjectProperty)
        {
            Output::send<LogLevel::Error>(STR("Could not find FontSize or FontObject property\n"));
            return false;
        }

        uint32_t* widgetFontSize = FontSizeProperty->ContainerPtrToValuePtr<uint32_t>(Font);
        UObject** widgetFontObject = FontObjectProperty->ContainerPtrToValuePtr<UObject*>(Font);

        if (!widgetFontSize)
        {
            Output::send<LogLevel::Error>(STR("Could not get font size value pointer for textbox %d\n"), i);
            return false;
        }

        if (!widgetFontObject)
        {
            Output::send<LogLevel::Error>(STR("Could not get font object value pointer for textbox %d\n"), i);
            return false;
        }

        // Set the "WidgetStyle.Font.FontObject" and "WidgetStyle.Font.Size" properties
        *widgetFontObject = fontObject;
        *widgetFontSize = UI::TEXTBOX_FONT_SIZE;
    }

    return true;
}


// ============================================================
// Public API: Refactored CreateTextboxes() using helper methods
// ============================================================

void MainMenuManager::CreateMenuTextboxes()
{
	if(bMenuTextboxesCreated)
	{
		return;
	}

	// Set this flag so we don't run this function twice
	bMenuTextboxesCreated = true;

	Output::send<LogLevel::Verbose>(STR("Creating textboxes for Archipelago connection menu\n"));

	// Orchestrate the creation process through helper methods
	// Each helper is responsible for one step, making the flow clear and modular

	// Step 1: Find all required UI containers (WrapBox, WidgetTree, Font)
	UObject* wrapBox = nullptr;
	UObject* widgetTree = nullptr;
	UObject* fontObject = nullptr;
	if (!FindUIContainers(wrapBox, widgetTree, fontObject))
	{
		return;
	}

	// Step 2: Create WrapBoxSlot objects and add to parent
	TArray<UObject*> slots;
	if (!CreateWrapBoxSlots(wrapBox, slots))
	{
		return;
	}

	// Step 3: Create EditableTextBox objects and set hints
	TArray<UObject*> textboxes;
	if (!CreateEditableTextBoxes(widgetTree, textboxes))
	{
		return;
	}

	// Step 4: Set up parent/child widget hierarchy
	if (!SetupWidgetHierarchy(slots, textboxes, wrapBox))
	{
		return;
	}

	// Step 5: Configure all styling properties
	if (!ConfigureTextboxStyles(textboxes, fontObject))
	{
		return;
	}
}

void MainMenuManager::DeleteOriginalTextbox()
{
	std::optional<UObject*> editableTextBoxOriginal = UnrealObjectQueries::FindArchipelagoTextbox(GameObjects::ORIGINAL_TEXTBOX_NAME);
	if (!editableTextBoxOriginal.has_value())
	{
	    Output::send<LogLevel::Error>(STR("Could not find original EditableTextBox to delete them\n"));
		return;
	}

	// Call the "RemoveFromParent" method on the original EditableTextBox to remove it from the menu
	UFunction* RemoveFromParentFunc = static_cast<UFunction*>(UObjectGlobals::StaticFindObject(nullptr, nullptr, UnrealClasses::FUNC_WIDGET_REMOVE_FROM_PARENT));
	if (!RemoveFromParentFunc)
	{
		Output::send<LogLevel::Error>(STR("Could not find RemoveFromParent function\n"));
		return;
	}
	editableTextBoxOriginal.value()->ProcessEvent(RemoveFromParentFunc, nullptr);
}

bool MainMenuManager::IsUserInArchipelagoMenu()
{
    // Find the game menu widget switcher
    std::optional<UObject*> widgetSwitcher = UnrealObjectQueries::FindGameMenuWidgetSwitcher();
    if(!widgetSwitcher.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find WidgetSwitcher object\n"));
        return false;
    }

    // Get the "ActiveWidgetIndex" property
    uint32_t* activeWidgetIndex = widgetSwitcher.value()->GetValuePtrByPropertyNameInChain<uint32_t>(PropertyNames::PROP_ACTIVE_WIDGET_INDEX);
    if (activeWidgetIndex == nullptr)
    {
        Output::send<LogLevel::Error>(STR("Could not find ActiveWidgetIndex property in WidgetSwitcher\n"));
        return false;
    }

    // Check if we are in the Archipelago menu (file naming section)
    if (*activeWidgetIndex != GameObjects::MENU_INDEX_ARCHIPELAGO)
    {
        return false;
    }

    return true;
}

// ============================================================
// Menu Navigation Methods - Public API
// ============================================================

void MainMenuManager::OnReturnPressed()
{
    if(!bMainMenuLoaded) return;

    Output::send<LogLevel::Verbose>(STR("RETURN was pressed\n"));

    if (!IsUserInArchipelagoMenu())
    {
        SetMenuFocusIndex(0);
        return;
    }

    int currentIndex = GetMenuFocusIndex();
    if (currentIndex == 3)
    {
        SubmitMenuConnection();
        return;
    }

    // Move focus down (increase index, staying within 1-3 range)
    SetMenuFocusIndex(std::clamp(currentIndex + 1, 1, 3));
    UpdateMenuFocus();
}

bool MainMenuManager::HandleUpKeyPress()
{
    Output::send<LogLevel::Verbose>(STR("UP key pressed in menu\n"));

    if (!IsUserInArchipelagoMenu())
    {
        return false;
    }

    // Move focus up (decrease index, staying within 1-3 range)
    int currentIndex = GetMenuFocusIndex();
    SetMenuFocusIndex(std::clamp(currentIndex - 1, 1, 3));

    return UpdateMenuFocus();
}

bool MainMenuManager::HandleDownKeyPress()
{
    Output::send<LogLevel::Verbose>(STR("DOWN key pressed in menu\n"));

    if (!IsUserInArchipelagoMenu())
    {
        return false;
    }

    int currentIndex = GetMenuFocusIndex();
    if (currentIndex == 3)
    {
        return true;
    }

    // Move focus down (increase index, staying within 1-3 range)
    SetMenuFocusIndex(std::clamp(currentIndex + 1, 1, 3));

    return UpdateMenuFocus();
}

void MainMenuManager::HandleEditableTextBoxCreated(const UObjectBase* object, int32 index)
{
    UObject* uobject = (UObject*)object;

    if(uobject->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
	   uobject->GetNamePrivate().ToString().ends_with(GameObjects::ORIGINAL_TEXTBOX_NAME))
    {
        CreateMenuTextboxes();
    }
}

/// Static hook callbacks - delegate to instance through BlueFireArchipelagoMod
bool MainMenuManager::UpKeyHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("UP key pressed in Archipelago menu\n"));

    if (BlueFireArchipelagoMod::mainMenuManager)
    {
        return BlueFireArchipelagoMod::mainMenuManager->HandleUpKeyPress();
    }
    return false;
}

bool MainMenuManager::DownKeyHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("DOWN key pressed in Archipelago menu\n"));

    if (BlueFireArchipelagoMod::mainMenuManager)
    {
        return BlueFireArchipelagoMod::mainMenuManager->HandleDownKeyPress();
    }
    return false;
}

void MainMenuManager::OnEditableTextBoxCreated(const UObjectBase* object, int32 index)
{
    if (BlueFireArchipelagoMod::mainMenuManager)
    {
        BlueFireArchipelagoMod::mainMenuManager->HandleEditableTextBoxCreated(object, index);
    }
}

void MainMenuManager::SubmitMenuConnection()
{
    std::optional<UObject*> IPEditableTextBox = UnrealObjectQueries::FindArchipelagoTextbox(GameObjects::TEXTBOX_IP);
    if (!IPEditableTextBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the IP textbox\n"));
        return;
    }

    std::optional<UObject*> UsernameEditableTextBox = UnrealObjectQueries::FindArchipelagoTextbox(GameObjects::TEXTBOX_USERNAME);
    if (!UsernameEditableTextBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the username textbox\n"));
        return;
    }

    std::optional<UObject*> PasswordEditableTextBox = UnrealObjectQueries::FindArchipelagoTextbox(GameObjects::TEXTBOX_PASSWORD);
    if (!PasswordEditableTextBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the password textbox\n"));
        return;
    }

    FText* IP = IPEditableTextBox.value()->GetValuePtrByPropertyNameInChain<FText>(STR("Text"));
    FText* Username = UsernameEditableTextBox.value()->GetValuePtrByPropertyNameInChain<FText>(STR("Text"));
    FText* Password = PasswordEditableTextBox.value()->GetValuePtrByPropertyNameInChain<FText>(STR("Text"));

    if (!IP)
    {
        Output::send<LogLevel::Error>(STR("Could not get IP text property\n"));
        return;
    }

    if (!Username)
    {
        Output::send<LogLevel::Error>(STR("Could not get Username text property\n"));
        return;
    }

    if (!Password)
    {
        Output::send<LogLevel::Error>(STR("Could not get Password text property\n"));
        return;
    }

    if (BlueFireArchipelagoMod::arcManager)
    {
        BlueFireArchipelagoMod::arcManager->setSuccessfulConnectionCallback(&(MainMenuManager::ArchipelagoConnected));
        BlueFireArchipelagoMod::arcManager->connectToArchipelagoServer(IP, Username, Password);
    }
}

void MainMenuManager::ArchipelagoConnected()
{
    // TODO : change the name of the save file to "Archipelago" here
    // TODO : Move this to a callback of the ArchipelagoManager that checks the connection status

    std::optional<UObject*> gameMenuController = UnrealObjectQueries::FindGameMenuController();
    if(!gameMenuController.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find Game Menu Controller object\n"));
        return;
    }

    // Check if save file already exists or not
    std::optional<UObject*> gameMenu = UnrealObjectQueries::FindGameMenu();
    if(!gameMenu.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find Game Menu object\n"));
        return;
    }

    bool* bIsAlreadyUsedSaveSlot = gameMenu.value()->GetValuePtrByPropertyNameInChain<bool>(STR("SaveGameExists"));
    if (!bIsAlreadyUsedSaveSlot)
    {
        Output::send<LogLevel::Error>(STR("Could not get SaveGameExists property\n"));
        return;
    }

    if(*bIsAlreadyUsedSaveSlot)
    {
        UFunction* StartGameFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, L"/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:StartGame");
        if(!StartGameFunc)
        {
            Output::send<LogLevel::Error>(STR("Could not find StartGame function\n"));
            return;
        }

        UFunction* CancelWriteFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, L"/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:CancelWrite");
        if(!CancelWriteFunc)
        {
            Output::send<LogLevel::Error>(STR("Could not find CancelWrite function\n"));
            return;
        }

        BlueFireArchipelagoMod::mainMenuManager->bNoHookGameStartOnce = true;

        struct dummyStruct{} params{};
        gameMenuController.value()->ProcessEvent(CancelWriteFunc, &params);
        gameMenuController.value()->ProcessEvent(StartGameFunc, &params);
    }
    else
    {
        UFunction* StartNewGameFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, L"/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:StartNewGame");
        if(!StartNewGameFunc)
        {
            Output::send<LogLevel::Error>(STR("Could not find StartNewGame function\n"));
            return;
        }

        struct startNewGameStruct{
            uint32_t demo;
        } params{0};
        gameMenuController.value()->ProcessEvent(StartNewGameFunc, &params);
    }
}


bool MainMenuManager::StartGameHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    if(!BlueFireArchipelagoMod::mainMenuManager)
    {
        Output::send<LogLevel::Error>(STR("mainMenuManager is null in StartGameHook\n"));
        return false;
    }

    if(BlueFireArchipelagoMod::mainMenuManager->bNoHookGameStartOnce)
    {
        BlueFireArchipelagoMod::mainMenuManager->bNoHookGameStartOnce = false;
        return false;
    }

    // User is trying to start a game file, first send them to the Archipelago menu
    std::optional<UObject*> gameMenu = UnrealObjectQueries::FindGameMenu();
    if(!gameMenu.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find Game Menu object\n"));
        return false;
    }

    UFunction* ChangeTabFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, L"/Game/BlueFire/HUD/Menu/GameMenu.GameMenu_C:ChangeTab");
    if(!ChangeTabFunc)
    {
        Output::send<LogLevel::Error>(STR("Could not find ChangeTab function\n"));
        return false;
    }

    struct changeTabStruct{
        uint32_t index;
    };
    changeTabStruct params{8};
    gameMenu.value()->ProcessEvent(ChangeTabFunc, &params);

    return true;
}


bool MainMenuManager::CancelWriteHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("CancelWriteHook called!\n"));

    if(!BlueFireArchipelagoMod::mainMenuManager)
    {
        Output::send<LogLevel::Error>(STR("mainMenuManager is null in CancelWriteHook\n"));
        return false;
    }
    BlueFireArchipelagoMod::mainMenuManager->SetMenuFocusIndex(0);

    if(!BlueFireArchipelagoMod::arcManager)
    {
        Output::send<LogLevel::Error>(STR("arcManager is null in CancelWriteHook\n"));
        return false;
    }
    BlueFireArchipelagoMod::arcManager->cancelConnection();

    return false;
}


bool MainMenuManager::MainMenuDoneLoading(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    // Is only done loading when called a 2nd time
    BlueFireArchipelagoMod::mainMenuManager->mainMenuLoadingState += 1;
    if(BlueFireArchipelagoMod::mainMenuManager->mainMenuLoadingState < 2)
    {
        return false;
    }

    Output::send<LogLevel::Verbose>(STR("MainMenuDoneLoading called!\n"));
    if(!BlueFireArchipelagoMod::mainMenuManager)
    {
        Output::send<LogLevel::Error>(STR("mainMenuManager is null in CancelWriteHook\n"));
        return false;
    }

    BlueFireArchipelagoMod::mainMenuManager->DeleteOriginalTextbox();
    BlueFireArchipelagoMod::mainMenuManager->bMainMenuLoaded = true;

    return false;
}
