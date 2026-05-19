#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp> 
#include <Unreal/UObject.hpp>
#include <UE4SSProgram.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/UFunction.hpp>

#include <Archipelago.h>

#include <BlueFireArchipelagoMod.hpp>
#include <ObjectCreateListener.hpp>
#include <ObjectFinder.hpp>
#include <HookManager.hpp>

using namespace RC;
using namespace Unreal;

bool bNewItemAlreadyHooked{ false };
FText* itemName;
FText* itemDescription;

BlueFireArchipelagoMod::BlueFireArchipelagoMod() : CppUserModBase()
{
    ModName = STR("BlueFireArchipelagoMod");
    ModVersion = STR("0.1");
    ModDescription = STR("Blue Fire mod to add Archipelago support");
    ModAuthors = STR("Nebulea");
    // Do not change this unless you want to target a UE4SS version
    // other than the one you're currently building with somehow.
    //ModIntendedSDKVersion = STR("2.6");

    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod initialized\n"));
}


// You are allowed to use the 'Unreal' namespace in this function and anywhere else after this function has fired
void BlueFireArchipelagoMod::on_unreal_init()
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from on_unreal_init ♥\n"));

    // Listen to object creation
    UObjectArray::AddUObjectCreateListener(&ObjectCreateListener);
    ObjectCreateListener.registerObjectCallback(std::wstring(L"NewItem_C"), ObjectCreatedNewItem);
    ObjectCreateListener.registerObjectCallback(std::wstring(L"EditableTextBox"), ObjectCreatedEditableTextBox);

    // Hook the RETURN key for the menus
    register_keydown_event(Input::Key::RETURN, {}, [&]() {
        onReturnPressed();
    });


    // Initialize the global hooks once we can now access UE objects
    HookManager.Init();

    /// Initialize Archipelago client
    AP_Init("127.0.0.1", "BlueFire", "Player1", "Password1");

    itemName = new FText(STR("Archipelago Item"));
    itemDescription = new FText(STR("An item from the Archipelago Mod. Is used to unlock items in the game of the other players in your session."));
}

void BlueFireArchipelagoMod::onReturnPressed()
{
    Output::send<LogLevel::Verbose>(STR("RETURN was pressed\n"));

    if(!bFileTextBoxRemoved)
    {
        std::optional<UObject*> editableTextBoxOriginal = ObjectFinder::FindObject(L"EditableTextBox", [](UObject* obj) {
            return obj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
                   obj->GetNamePrivate().ToString().ends_with(STR("VK_EditableTextBox_89"));
        });

        if (!editableTextBoxOriginal.has_value())
        {
            return;
        }

        // Call the "RemoveFromParent" method on the original EditableTextBox to remove it from the menu
        UFunction* RemoveFromParentFunc = static_cast<UFunction*>(UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Script/UMG.Widget:RemoveFromParent")));
        editableTextBoxOriginal.value()->ProcessEvent(RemoveFromParentFunc, nullptr);

        bFileTextBoxRemoved = true;
    }

    if(!isUserInArchipelagoMenu())
    {
        fileNamingActiveWidgetIndex = 0;
        return;
    }

    fileNamingActiveWidgetIndex = std::clamp(fileNamingActiveWidgetIndex + 1, 0, 4);

    UpdateFileNamingFocus();
}


void BlueFireArchipelagoMod::ObjectCreatedNewItem(const UObjectBase* object, int32 index)
{
    // We only want to listen to the first object created, since we only want to hook once
    ObjectCreateListener.deleteObjectCallback(std::wstring(L"NewItem_C"));

    // Hook the functions
    HookManager.registerHook(STR("Function /Game/BlueFire/HUD/NewItem.NewItem_C:PlayNewItem"), PlayNewItemPreHook);
    HookManager.registerHook(STR("Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:UpKey"), UpKeyPreHook);
    HookManager.registerHook(STR("Function /Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey"), DownKeyPreHook);

    Output::send<LogLevel::Verbose>(STR("Created hooks for key functions ♥\n"));
}

bool BlueFireArchipelagoMod::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from PlayNewItem ♥\n"));

    // Example of how to read and write function parameters from a hook
    // Useless here, but kept as a reference
    std::optional<FText> InText = HookManager::readParamValue<FText>(STR("InText"), Stack);
    if (InText.has_value())
    {
        Output::send<LogLevel::Verbose>(STR("InText has value {} ♥\n"), InText.value().ToString());
    }

    HookManager::setParamValue<FText>(STR("InText"), Stack, itemName);
    HookManager::setParamValue<FText>(STR("Description"), Stack, itemDescription);
    HookManager::setParamValue<uint8_t>(STR("KeyItem"), Stack, 0x1);
    HookManager::setParamValue<uint32_t>(STR("Amount"), Stack, 0xF);

    // Do not prevent the original function from being called
    return false;
}

bool BlueFireArchipelagoMod::UpKeyPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("Upkey pressed!\n"));

    if(!isUserInArchipelagoMenu())
    {
        return false;
    }

    // Update fileNamingActiveWidgetIndex
    fileNamingActiveWidgetIndex = std::clamp(fileNamingActiveWidgetIndex - 1, 1, 4);

    return UpdateFileNamingFocus();
}

bool BlueFireArchipelagoMod::DownKeyPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("Downkey pressed!\n"));

    if(!isUserInArchipelagoMenu() || fileNamingActiveWidgetIndex == 4)
    {
        return false;
    }

    // Update fileNamingActiveWidgetIndex
    fileNamingActiveWidgetIndex = std::clamp(fileNamingActiveWidgetIndex + 1, 1, 4);

    return UpdateFileNamingFocus();
}

bool BlueFireArchipelagoMod::UpdateFileNamingFocus()
{
    // Step 1 - Get all the references to the widgets that we will need.
    // This includes the 3 text boxes, the game controller and the start button


    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelagoIP"
    std::optional<UObject*> IPEditableTextBox = ObjectFinder::FindObject(L"EditableTextBox", [](UObject* obj) {
        return obj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
                obj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoIP"));
    });

    if (!IPEditableTextBox.has_value())
    {
        return false;
    }

    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelagoUsername"
    std::optional<UObject*> UsernameEditableTextBox = ObjectFinder::FindObject(L"EditableTextBox", [](UObject* obj) {
        return obj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
                obj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoUsername"));
    });

    if (!UsernameEditableTextBox.has_value())
    {
        return false;
    }

    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelagoPassword"
    std::optional<UObject*> PasswordEditableTextBox = ObjectFinder::FindObject(L"EditableTextBox", [](UObject* obj) {
        return obj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
                obj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoPassword"));
    });

    if (!PasswordEditableTextBox.has_value())
    {
        return false;
    }

    // Find the one GameMenuController_C object that has a full path that
    // - ends with "GameMenuController_2"
    std::optional<UObject*> GameMenuController = ObjectFinder::FindObject(L"GameMenuController_C", [](UObject* obj) {
        return obj->GetFullName().starts_with(STR("GameMenuController_C")) &&
                obj->GetNamePrivate().ToString().ends_with(STR("GameMenuController_2"));
    });

    if (!GameMenuController.has_value())
    {
        return false;
    }

    // Find the one GameMenuTitle_C object that has a full path that
    // - starts with /Engine/
    // - ends with "File_1"
    std::optional<UObject*> fileTitle = ObjectFinder::FindObject(L"GameMenuTitle_C", [](UObject* obj) {
        return obj->GetFullName().starts_with(STR("GameMenuTitle_C /Engine/")) &&
                obj->GetNamePrivate().ToString().ends_with(STR("File_1"));
    });

    if (!fileTitle.has_value())
    {
        return false;
    }


    // Step 2 - Find the functions that we will need to call


    // Find the "SetFocus" and "DownKey" functions
    UFunction* SetFocusFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Script/UMG.Widget:SetFocus"));
    UFunction* DownKeyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey"));
    if(!DownKeyFunc || !SetFocusFunc)
    {
        Output::send<LogLevel::Error>(STR("Could not find one or more required functions\n"));
        return false;
    }

    // Step 3 - Call the method on the right element depending on what is currently focused


    Output::send<LogLevel::Verbose>(STR("Setting focus on widget with index {} ♥\n"), fileNamingActiveWidgetIndex);
    switch(fileNamingActiveWidgetIndex)
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
        case 4:
            // For the button, we use the initial DownKey button, as this function
            // is the only way I currently have to allow the button to be pressed

            // Also this function takes an empty param struct because fuck you idk
            // My suspicion is that it's because it's not a /Script/ function
            struct dummyStruct{} params{};
            GameMenuController.value()->ProcessEvent(DownKeyFunc, &params);
            break;
    }

    return true;
}

// ============================================================
// Helper: construct a UObject of a given UClass with a given outer
// ============================================================
static UObject* ConstructWidget(UObject* classObj, UObject* outer, const wchar_t* name)
{
    auto* cls = static_cast<UClass*>(classObj);
    FStaticConstructObjectParameters params(cls);
    params.Outer = outer;
    params.Name  = FName(name, FNAME_Add);
    params.SetFlags = (EObjectFlags)(RF_Transactional);
    return UObjectGlobals::StaticConstructObject(params);
}


void BlueFireArchipelagoMod::ObjectCreatedEditableTextBox(const UObjectBase* object, int32 index)
{
    // Only listen to the correct editable box
    if(!(object->GetNamePrivate().ToString() == STR("VK_EditableTextBox_89")))
    {
        return;
    }


    // Step 1 - Get the necessary objects for creating everything


    // Find the one WrapBox object that has a full path that :
    // - starts with /Engine/
    // - ends with SaveFileWrapBox_1
    std::optional<UObject*> wrapBox = ObjectFinder::FindObject(L"WrapBox", [](UObject* obj) {
            return obj->GetFullName().starts_with(STR("WrapBox /Engine/")) &&
                   obj->GetNamePrivate().ToString().ends_with(STR("SaveFileWrapBox_1"));
        });

    if(!wrapBox.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find WrapBox object\n"));
        return;
    }


    // Find the one WidgetTree object that has a full path that
    // - starts with /Engine/
    // - ends with WidgetTree_0
    // - contains only the substring "WidgetTree_0" once in its full path
    // - contains "GameMenu_C" in its full path
    std::optional<UObject*> widgetTree = ObjectFinder::FindObject(L"WidgetTree", [](UObject* obj) {
            return obj->GetFullName().starts_with(STR("WidgetTree /Engine/")) &&
                   obj->GetNamePrivate().ToString().ends_with(STR("WidgetTree_0")) &&
                   obj->GetFullName().find(STR("WidgetTree_0")) == obj->GetFullName().rfind(STR("WidgetTree_0")) &&
                   obj->GetFullName().find(STR("GameMenu_C")) != std::string::npos;
        });

    if(!widgetTree.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find WidgetTree object\n"));
        return;
    }

    // Find font object for the text boxes
    UObject* fontObject = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, STR("/Game/BlueFire/HUD/Font/BlueFire-Spectral/BlueFire-SpectralSC-Bold_Font.BlueFire-SpectralSC-Bold_Font"));
    if(!fontObject)
    {
        Output::send<LogLevel::Error>(STR("Could not find font object\n"));
        return;
    }


    // Step 2 - Create the WrapBoxSlots


    // Construct the WrapBoxSlot objects that will be the parents of our new text boxes
    UObject* wrapBoxSlotClass  = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, STR("/Script/UMG.WrapBoxSlot"));
    UObject* IPwrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox.value(), STR("WrapBoxSlotArchipelagoIP"));
    UObject* UsernamewrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox.value(), STR("WrapBoxSlotArchipelagoUsername"));
    UObject* PasswordwrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox.value(), STR("WrapBoxSlotArchipelagoPassword"));

    // Get the "Slots" property, which is an array of UPanelSlot
    TArray<UObject*>* slotsArray = wrapBox.value()->GetValuePtrByPropertyNameInChain<TArray<UObject*>>(STR("Slots"));

    // Insert our WrapBoxSlots
    slotsArray->Insert(IPwrapBoxSlot, 1);
    slotsArray->Insert(UsernamewrapBoxSlot, 2);
    slotsArray->Insert(PasswordwrapBoxSlot, 3);

    // Create an array for the slots
    TArray<UObject*> wrapBoxSlotsArray;
    wrapBoxSlotsArray.Add(IPwrapBoxSlot);
    wrapBoxSlotsArray.Add(UsernamewrapBoxSlot);
    wrapBoxSlotsArray.Add(PasswordwrapBoxSlot);


    // Step 3 - Create the EditableTextBoxes


    UObject* editableTextBoxClass  = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, STR("/Script/UMG.EditableTextBox"));
    UObject* IPeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree.value(), STR("EditableTextBoxArchipelagoIP"));
    UObject* UsernameeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree.value(), STR("EditableTextBoxArchipelagoUsername"));
    UObject* PasswordeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree.value(), STR("EditableTextBoxArchipelagoPassword"));

    // Create an array for the text boxes
    TArray<UObject*> editableTextBoxesArray;
    editableTextBoxesArray.Add(IPeditableTextBox);
    editableTextBoxesArray.Add(UsernameeditableTextBox);
    editableTextBoxesArray.Add(PasswordeditableTextBox);

    TArray<std::wstring> editableTextBoxHint = {
        STR("Archipelago Server IP"),
        STR("Archipelago Username"),
        STR("Archipelago Password")
    };


    // Step 4 - Set the widget hierarchy

    // The hierarchy should be :
    // WrapBox
    //  ├─ WrapBoxSlotArchipelagoIP
    //  │   └─ EditableTextBoxArchipelagoIP
    //  ├─ WrapBoxSlotArchipelagoUsername
    //  │   └─ EditableTextBoxArchipelagoUsername
    //  └─ WrapBoxSlotArchipelagoPassword
    //      └─ EditableTextBoxArchipelagoPassword


     for(uint8_t i = 0; i < wrapBoxSlotsArray.Num(); i++)
    {
        UObject* slot = wrapBoxSlotsArray[i];
        UObject* textBox = editableTextBoxesArray[i];
        std::wstring textBoxHint = editableTextBoxHint[i];


        // Set the "Parent" property of the WrapBoxSlot to be the parent WrapBox
        UObject** wrapBoxSlotParentProperty = slot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Parent"));
        if (wrapBoxSlotParentProperty == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find Parent property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotParentProperty = wrapBox.value();


        // Set the "Content" property of the WrapBoxSlot to be our new EditableTextBox
        UObject** wrapBoxSlotContentProperty = slot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Content"));
        if (wrapBoxSlotContentProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Content property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotContentProperty = textBox;


        // Set the "Slot" property of the EditableTextBox
        UObject** editableTextBoxSlotProperty = textBox->GetValuePtrByPropertyNameInChain<UObject*>(STR("Slot"));
        if (editableTextBoxSlotProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Slot property in EditableTextBox\n"));
            return;
        }
        *editableTextBoxSlotProperty = slot;
    }


    // Step 5 - Set the easy properties of the widgets


    for(uint8_t i = 0; i < wrapBoxSlotsArray.Num(); i++)
    {
        UObject* slot = wrapBoxSlotsArray[i];
        UObject* textBox = editableTextBoxesArray[i];
        std::wstring textBoxHint = editableTextBoxHint[i];


        // Set the "bFillEmptySpace" property of the WrapBoxSlot to true
        uint8_t* bFillEmptySpaceProperty = slot->GetValuePtrByPropertyNameInChain<uint8_t>(STR("bFillEmptySpace"));
        if (bFillEmptySpaceProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find bFillEmptySpace property in WrapBoxSlot\n"));
            return;
        }
        *bFillEmptySpaceProperty = 0x1;


        // Set the "Justification" property of our new EditableTextBox to center
        uint8_t* justificationProperty = textBox->GetValuePtrByPropertyNameInChain<uint8_t>(STR("Justification"));
        if (justificationProperty == nullptr)        {
            Output::send<LogLevel::Error>(STR("Could not find Justification property in new EditableTextBox\n"));
            return;
        }
        *justificationProperty = 0x1; // ETextJustify::Center


        // Set the "HintText" property of our new EditableTextBoxes
        FText* newHintText = textBox->GetValuePtrByPropertyNameInChain<FText>(STR("HintText"));
        if (newHintText == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find HintText property in new EditableTextBox\n"));
            return;
        }
        *newHintText = FText(textBoxHint);
    }


    // Step 5 - Set the style struct property of the EditableTextBoxes


    for(uint8_t i = 0; i < wrapBoxSlotsArray.Num(); i++)
    {
        UObject* slot = wrapBoxSlotsArray[i];
        UObject* textBox = editableTextBoxesArray[i];
        std::wstring textBoxHint = editableTextBoxHint[i];


        // Get the "WidgetStyle" property
        FStructProperty* StyleWidgetProperty = static_cast<FStructProperty*>(textBox->GetPropertyByNameInChain(STR("WidgetStyle")));
        if (!StyleWidgetProperty)
        {
            return;
        }


        // Get the "WidgetStyle.Font" property
        auto StyleWidgetStruct = StyleWidgetProperty->GetStruct();
        auto StyleWidget = StyleWidgetProperty->ContainerPtrToValuePtr<void>(textBox);
        FStructProperty* FontProperty = static_cast<FStructProperty*>(StyleWidgetStruct->GetPropertyByNameInChain(STR("Font")));
        if (!FontProperty)
        {
            return;
        }

        auto FontStruct = FontProperty->GetStruct();
        auto Font = FontProperty->ContainerPtrToValuePtr<void>(StyleWidget);


        // Get the "WidgetStyle.Font.FontObject" and "WidgetStyle.Font.Size" properties
        FProperty* FontSizeProperty = static_cast<FProperty*>(FontStruct->GetPropertyByNameInChain(STR("Size")));
        FStructProperty* FontObjectProperty = static_cast<FStructProperty*>(FontStruct->GetPropertyByNameInChain(STR("FontObject")));
        if (!FontSizeProperty || !FontObjectProperty)
        {
            return;
        }


        uint32_t* widgetFontSize = FontSizeProperty->ContainerPtrToValuePtr<uint32_t>(Font);
        UObject** widgetFontObject = FontObjectProperty->ContainerPtrToValuePtr<UObject*>(Font);


        // Set the "WidgetStyle.Font.FontObject" and "WidgetStyle.Font.Size" properties
        *widgetFontObject = fontObject;
        *widgetFontSize = 40;
    }
}


bool BlueFireArchipelagoMod::isUserInArchipelagoMenu()
{
    // Find the one WidgetTree object that has a full path that
    // - starts with /Engine/
    // - ends with WidgetTree_0
    // - contains only the substring "WidgetTree_0" once in its full path
    // - contains "GameMenu_C" in its full path
    std::optional<UObject*> widgetSwitcher = ObjectFinder::FindObject(L"WidgetTree", [](UObject* obj) {
            return obj->GetFullName().starts_with(STR("WidgetSwitcher /Engine/")) &&
                   obj->GetNamePrivate().ToString().ends_with(STR("WidgetSwitcher_0")) &&
                   obj->GetFullName().find(STR("WidgetSwitcher_0")) == obj->GetFullName().rfind(STR("WidgetSwitcher_0")) &&
                   obj->GetFullName().find(STR("GameMenu_C")) != std::string::npos;
        });

    if(!widgetSwitcher.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find WidgetSwitcher object\n"));
        return false;
    }

    // Get the "ActiveWidgetIndex" property
    uint32_t* activeWidgetIndex = widgetSwitcher->GetValuePtrByPropertyNameInChain<uint32_t>(STR("ActiveWidgetIndex"));
    if (activeWidgetIndex == nullptr)
    {
        Output::send<LogLevel::Error>(STR("Could not find ActiveWidgetIndex property in WidgetSwitcher\n"));
        return false;
    }


    // If we are not in the file naming section of the menu
    // TODO : not hardcode this value
    if (*activeWidgetIndex != 8)
    {
        return false;
    }

    return true;
}