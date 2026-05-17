#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp> 
#include <Unreal/UObject.hpp>
#include <UE4SSProgram.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/UFunction.hpp>

#include <Archipelago.h>

#include <BlueFireArchipelagoMod.hpp>
#include <ObjectCreateListener.hpp>
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

    // It's critical that you enable ImGui before you create your tab.
    // If you don't do this, a crash will occur as soon as ImGui tries to render anything in your tab.
    UE4SS_ENABLE_IMGUI()

    // The 'register_tab' function will tell UE4SS to render a tab.
    // Tabs registered this way will be automatically cleaned up when this C++ mod is destructed.
    register_tab(STR("My Test Tab"), [](CppUserModBase* instance) {
        // In this callback, you can start rendering the contents of your tab with ImGui.
        ImGui::Text("This is the contents of the tab");

        // Add a button with a callback that calls BlueFireArchipelagoMod::CopyStyle
        if (ImGui::Button("Copy Style"))
        {
            auto mod = dynamic_cast<BlueFireArchipelagoMod*>(instance);
            if (!mod)
            {
                // Something went wrong that caused the 'instance' to not be correctly set.
                return;
            }

            mod->CopyStyle();
        }
    });

    fileNamingActiveWidgetIndex = 0;

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

    std::vector<UObject*> foundObjects;

    if(!bFileTextBoxRemoved)
    {
        UObject* editableTextBoxOriginal = nullptr;

        // Find the one EditableTextBox object that has a full path that
        // - starts with /Engine/
        // - ends with "VK_EditableTextBox_89"
        UObjectGlobals::FindAllOf(STR("EditableTextBox"), foundObjects);

        for(UObject* foundObj : foundObjects)
        {
            // Check if full path starts with /Engine/ and ends with "VK_EditableTextBox_89"
            if (foundObj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
                foundObj->GetNamePrivate().ToString().ends_with(STR("VK_EditableTextBox_89")))
            {
                Output::send<LogLevel::Verbose>(STR("Found EditableTextBox object: {}\n"), foundObj->GetNamePrivate().ToString());
                editableTextBoxOriginal = foundObj;
                break;
            }
        }

        if(!editableTextBoxOriginal)
        {
            Output::send<LogLevel::Error>(STR("Could not find EditableTextBox object\n"));
            return;
        }

        // Call the "RemoveFromParent" method on the original EditableTextBox to remove it from the menu
        UFunction* RemoveFromParentFunc = static_cast<UFunction*>(UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Script/UMG.Widget:RemoveFromParent")));
        editableTextBoxOriginal->ProcessEvent(RemoveFromParentFunc, nullptr);

        bFileTextBoxRemoved = true;
    }

    if(!isUserInArchipelagoMenu())
    {
        fileNamingActiveWidgetIndex = 0;
        return;
    }


    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelago"
    UObject* IPEditableTextBox = nullptr;
    UObjectGlobals::FindAllOf(STR("EditableTextBox"), foundObjects);

    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "EditableTextBoxArchipelagoIP"
        if (foundObj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoIP")))
        {
            Output::send<LogLevel::Verbose>(STR("Found EditableTextBox object: {}\n"), foundObj->GetNamePrivate().ToString());
            IPEditableTextBox = foundObj;
            break;
        }
    }

    if(!IPEditableTextBox)
    {
        Output::send<LogLevel::Error>(STR("Could not find EditableTextBox object\n"));
        return;
    }

    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelago"
    UObject* UsernameEditableTextBox = nullptr;
    UObjectGlobals::FindAllOf(STR("EditableTextBox"), foundObjects);

    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "EditableTextBoxArchipelagoUsername"
        if (foundObj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoUsername")))
        {
            Output::send<LogLevel::Verbose>(STR("Found EditableTextBox object: {}\n"), foundObj->GetNamePrivate().ToString());
            UsernameEditableTextBox = foundObj;
            break;
        }
    }

    if(!UsernameEditableTextBox)
    {
        Output::send<LogLevel::Error>(STR("Could not find EditableTextBox object\n"));
        return;
    }

    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelago"
    UObject* PasswordEditableTextBox = nullptr;
    UObjectGlobals::FindAllOf(STR("EditableTextBox"), foundObjects);

    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "EditableTextBoxArchipelagoPassword"
        if (foundObj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoPassword")))
        {
            Output::send<LogLevel::Verbose>(STR("Found EditableTextBox object: {}\n"), foundObj->GetNamePrivate().ToString());
            PasswordEditableTextBox = foundObj;
            break;
        }
    }

    if(!PasswordEditableTextBox)
    {
        Output::send<LogLevel::Error>(STR("Could not find EditableTextBox object\n"));
        return;
    }

    // Find the one GameMenuController_C object that has a full path that
    // - ends with "GameMenuController_2"
    UObject* gameMenuController = nullptr;
    UObjectGlobals::FindAllOf(STR("GameMenuController_C"), foundObjects);
    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "GameMenuController_2"
        if (foundObj->GetFullName().starts_with(STR("GameMenuController_C")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("GameMenuController_2")))
        {
            Output::send<LogLevel::Verbose>(STR("Found GameMenuController_C object: {}\n"), foundObj->GetFullName());
            gameMenuController = foundObj;
            break;
        }
    }

    if(!gameMenuController)
    {
        Output::send<LogLevel::Error>(STR("Could not find GameMenuController_C object\n"));
        return;
    }

    // Find the one GameMenuTitle_C object that has a full path that
    // - starts with /Engine/
    // - ends with "File_1"
    UObject* fileTitle = nullptr;
    UObjectGlobals::FindAllOf(STR("GameMenuTitle_C"), foundObjects);
    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "File_1"
        if (foundObj->GetFullName().starts_with(STR("GameMenuTitle_C /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("File_1")))
        {
            Output::send<LogLevel::Verbose>(STR("Found GameMenuTitle_C object: {}\n"), foundObj->GetNamePrivate().ToString());
            fileTitle = foundObj;
            break;
        }
    }

    if(!fileTitle)
    {
        Output::send<LogLevel::Error>(STR("Could not find GameMenuTitle_C object\n"));
        return;
    }

    // Find the "SetFocus" and "DownKey" functions
    UFunction* SetFocusFunc = static_cast<UFunction*>(UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Script/UMG.Widget:SetFocus")));
    UFunction* DownKeyFunc = static_cast<UFunction*>(UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey")));
    if(!DownKeyFunc || !SetFocusFunc)
    {
        Output::send<LogLevel::Error>(STR("Could not find one or more required functions\n"));
        return;
    }

    // Call the "SetFocus" method on the right element
    if(fileNamingActiveWidgetIndex == 0)
    {
        IPEditableTextBox->ProcessEvent(SetFocusFunc, nullptr);
        Output::send<LogLevel::Verbose>(STR("Set focus to IPEditableTextBox\n"));
    }
    if(fileNamingActiveWidgetIndex == 1)
    {
        UsernameEditableTextBox->ProcessEvent(SetFocusFunc, nullptr);
        Output::send<LogLevel::Verbose>(STR("Set focus to UsernameEditableTextBox\n"));
    }
    if(fileNamingActiveWidgetIndex == 2)
    {
        PasswordEditableTextBox->ProcessEvent(SetFocusFunc, nullptr);
        Output::send<LogLevel::Verbose>(STR("Set focus to PasswordEditableTextBox\n"));
    }
    if(fileNamingActiveWidgetIndex == 3)
    {
        struct dummyStruct{} params{};
        gameMenuController->ProcessEvent(DownKeyFunc, &params);
        Output::send<LogLevel::Verbose>(STR("Set focus to fileTitle\n"));
    }

    fileNamingActiveWidgetIndex = fileNamingActiveWidgetIndex == 4 ? 4 : fileNamingActiveWidgetIndex + 1;

    return;


}


void BlueFireArchipelagoMod::ObjectCreatedNewItem(const UObjectBase* object, int32 index)
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from ObjectCreatedNewItem ♥\n"));

    // We only want to listen to the first object created, since we only want to hook once
    ObjectCreateListener.deleteObjectCallback(std::wstring(L"NewItem_C"));

    // Find the function to hook
    UObject* PlayNewItemFunc = UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Game/BlueFire/HUD/NewItem.NewItem_C:PlayNewItem"));
    static std::wstring PlayNewItemFuncName = PlayNewItemFunc->GetNamePrivate().ToString();

    // Hook the PlayNewItem function
    HookManager.registerHook(PlayNewItemFuncName, PlayNewItemPreHook);
    Output::send(STR("Registered hook for function: {}\n"), PlayNewItemFuncName);

    // Find the function to hook
    UObject* UpKeyFunc = UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:UpKey"));
    static std::wstring UpKeyFuncName = UpKeyFunc->GetNamePrivate().ToString();

    // Hook the UpKey function
    HookManager.registerHook(UpKeyFuncName, UpKeyPreHook);

    // Find the function to hook
    UObject* DownKeyFunc = UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Game/BlueFire/HUD/Menu/GameMenuController.GameMenuController_C:DownKey"));
    static std::wstring DownKeyFuncName = DownKeyFunc->GetNamePrivate().ToString();

    // Hook the DownKey function
    HookManager.registerHook(DownKeyFuncName, DownKeyPreHook);
}

bool BlueFireArchipelagoMod::PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from PlayNewItem ♥\n"));

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
    if(!isUserInArchipelagoMenu())
    {
        return false;
    }

    // Update fileNamingActiveWidgetIndex
    fileNamingActiveWidgetIndex = fileNamingActiveWidgetIndex == 1 ? 1 : fileNamingActiveWidgetIndex - 1;

    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from UpKey ♥\n"));

    return UpdateFileNamingFocus();
}

bool BlueFireArchipelagoMod::DownKeyPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL)
{
    if(!isUserInArchipelagoMenu())
    {
        return false;
    }

    // Update fileNamingActiveWidgetIndex
    fileNamingActiveWidgetIndex = fileNamingActiveWidgetIndex == 4 ? 4 : fileNamingActiveWidgetIndex + 1;

    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from DownKey ♥\n"));

    return UpdateFileNamingFocus();
}

bool BlueFireArchipelagoMod::UpdateFileNamingFocus()
{
    std::vector<UObject*> foundObjects;

    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelago"
    UObject* IPEditableTextBox = nullptr;
    UObjectGlobals::FindAllOf(STR("EditableTextBox"), foundObjects);

    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "EditableTextBoxArchipelagoIP"
        if (foundObj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoIP")))
        {
            Output::send<LogLevel::Verbose>(STR("Found EditableTextBox object: {}\n"), foundObj->GetNamePrivate().ToString());
            IPEditableTextBox = foundObj;
            break;
        }
    }

    if(!IPEditableTextBox)
    {
        Output::send<LogLevel::Error>(STR("Could not find EditableTextBox object\n"));
        return false;
    }

    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelago"
    UObject* UsernameEditableTextBox = nullptr;
    UObjectGlobals::FindAllOf(STR("EditableTextBox"), foundObjects);

    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "EditableTextBoxArchipelagoUsername"
        if (foundObj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoUsername")))
        {
            Output::send<LogLevel::Verbose>(STR("Found EditableTextBox object: {}\n"), foundObj->GetNamePrivate().ToString());
            UsernameEditableTextBox = foundObj;
            break;
        }
    }

    if(!UsernameEditableTextBox)
    {
        Output::send<LogLevel::Error>(STR("Could not find EditableTextBox object\n"));
        return false;
    }

    // Find the one EditableTextBox object that has a full path that
    // - starts with /Engine/
    // - ends with "EditableTextBoxArchipelago"
    UObject* PasswordEditableTextBox = nullptr;
    UObjectGlobals::FindAllOf(STR("EditableTextBox"), foundObjects);

    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "EditableTextBoxArchipelagoPassword"
        if (foundObj->GetFullName().starts_with(STR("EditableTextBox /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("EditableTextBoxArchipelagoPassword")))
        {
            Output::send<LogLevel::Verbose>(STR("Found EditableTextBox object: {}\n"), foundObj->GetNamePrivate().ToString());
            PasswordEditableTextBox = foundObj;
            break;
        }
    }

    if(!PasswordEditableTextBox)
    {
        Output::send<LogLevel::Error>(STR("Could not find EditableTextBox object\n"));
        return false;
    }

    // Find the one GameMenuTitle_C object that has a full path that
    // - starts with /Engine/
    // - ends with "File_1"
    UObject* fileTitle = nullptr;
    UObjectGlobals::FindAllOf(STR("GameMenuTitle_C"), foundObjects);
    for(UObject* foundObj : foundObjects)
    {
        // Check if full path starts with /Engine/ and ends with "File_1"
        if (foundObj->GetFullName().starts_with(STR("GameMenuTitle_C /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("File_1")))
        {
            Output::send<LogLevel::Verbose>(STR("Found GameMenuTitle_C object: {}\n"), foundObj->GetNamePrivate().ToString());
            fileTitle = foundObj;
            break;
        }
    }

    // Find the "SetFocus" function in the EditableTextBox's class
    UFunction* SetFocusFunc = static_cast<UFunction*>(UObjectGlobals::StaticFindObject(nullptr, nullptr, STR("/Script/UMG.Widget:SetFocus")));

    // Call the "SetFocus" method on the right element
    if(fileNamingActiveWidgetIndex == 1)
    {
        IPEditableTextBox->ProcessEvent(SetFocusFunc, nullptr);
        Output::send<LogLevel::Verbose>(STR("Set focus to IPEditableTextBox\n"));
    }
    if(fileNamingActiveWidgetIndex == 2)
    {
        UsernameEditableTextBox->ProcessEvent(SetFocusFunc, nullptr);
        Output::send<LogLevel::Verbose>(STR("Set focus to UsernameEditableTextBox\n"));
    }
    if(fileNamingActiveWidgetIndex == 3)
    {
        PasswordEditableTextBox->ProcessEvent(SetFocusFunc, nullptr);
        Output::send<LogLevel::Verbose>(STR("Set focus to PasswordEditableTextBox\n"));
    }
    if(fileNamingActiveWidgetIndex == 4)
    {
        fileTitle->ProcessEvent(SetFocusFunc, nullptr);
        Output::send<LogLevel::Verbose>(STR("Set focus to fileTitle\n"));
        return false;
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
    if (const auto uobject = static_cast<UObject*>(const_cast<UObjectBase*>(object)))
	{
        if(!(uobject->GetNamePrivate().ToString() == STR("VK_EditableTextBox_89")))
        {
            return;
        }

        Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from ObjectCreatedEditableTextBox ♥\n"));


        std::vector<UObject*> foundObjects;
        UObject* wrapBox = nullptr;
        UObject* widgetTree = nullptr;

        // Find the one WrapBox object that has a full path that starts with /Engine/ and a name that ends with SaveFileWrapBox_1, since that's the one we want to modify
        UObjectGlobals::FindAllOf(STR("WrapBox"), foundObjects);

        for(UObject* foundObj : foundObjects)
        {

            // Check if full path starts with /Engine/ and ends with SaveFileWrapBox_1
            if (foundObj->GetFullName().starts_with(STR("WrapBox /Engine/")) && foundObj->GetNamePrivate().ToString().ends_with(STR("SaveFileWrapBox_1")))
            {
                Output::send<LogLevel::Verbose>(STR("Found WrapBox object: {}\n"), foundObj->GetNamePrivate().ToString());
                wrapBox = foundObj;
                break;
            }
        }

        if(!wrapBox)
        {
            Output::send<LogLevel::Error>(STR("Could not find WrapBox object\n"));
            return;
        }

        // Find the one WidgetTree object that has a full path that
        // - starts with /Engine/
        // - ends with WidgetTree_0
        // - contains only the substring "WidgetTree_0" once in its full path
        // - contains "GameMenu_C" in its full path
        UObjectGlobals::FindAllOf(STR("WidgetTree"), foundObjects);

        for(UObject* foundObj : foundObjects)
        {
            if (foundObj->GetFullName().starts_with(STR("WidgetTree /Engine/")) &&
                foundObj->GetNamePrivate().ToString().ends_with(STR("WidgetTree_0")) &&
                foundObj->GetFullName().find(STR("WidgetTree_0")) == foundObj->GetFullName().rfind(STR("WidgetTree_0")) &&
                foundObj->GetFullName().find(STR("GameMenu_C")) != std::string::npos)
            {
                Output::send<LogLevel::Verbose>(STR("Found WidgetTree object: {}\n"), foundObj->GetNamePrivate().ToString());
                widgetTree = foundObj;
                break;
            }
        }

        if(!widgetTree)
        {
            Output::send<LogLevel::Error>(STR("Could not find WidgetTree object\n"));
            return;
        }

        UObject* wrapBoxSlotClass  = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, STR("/Script/UMG.WrapBoxSlot"));
        UObject* IPwrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox, STR("WrapBoxSlotArchipelagoIP"));
        UObject* UsernamewrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox, STR("WrapBoxSlotArchipelagoUsername"));
        UObject* PasswordwrapBoxSlot = ConstructWidget(wrapBoxSlotClass, wrapBox, STR("WrapBoxSlotArchipelagoPassword"));

        // Get the "Slots" property, which is an array of UPanelSlot
        TArray<UObject*>* slotsArray = wrapBox->GetValuePtrByPropertyNameInChain<TArray<UObject*>>(STR("Slots"));

        // Insert our WrapBoxSlots
        slotsArray->Insert(IPwrapBoxSlot, 1);
        slotsArray->Insert(UsernamewrapBoxSlot, 2);
        slotsArray->Insert(PasswordwrapBoxSlot, 3);




        // Set the "bFillEmptySpace" property of the WrapBoxSlots to true
        uint8_t* bFillEmptySpaceProperty = IPwrapBoxSlot->GetValuePtrByPropertyNameInChain<uint8_t>(STR("bFillEmptySpace"));
        if (bFillEmptySpaceProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find bFillEmptySpace property in WrapBoxSlot\n"));
            return;
        }
        *bFillEmptySpaceProperty = 0x1;

        bFillEmptySpaceProperty = UsernamewrapBoxSlot->GetValuePtrByPropertyNameInChain<uint8_t>(STR("bFillEmptySpace"));
        if (bFillEmptySpaceProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find bFillEmptySpace property in WrapBoxSlot\n"));
            return;
        }
        *bFillEmptySpaceProperty = 0x1;

        bFillEmptySpaceProperty = PasswordwrapBoxSlot->GetValuePtrByPropertyNameInChain<uint8_t>(STR("bFillEmptySpace"));
        if (bFillEmptySpaceProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find bFillEmptySpace property in WrapBoxSlot\n"));
            return;
        }
        *bFillEmptySpaceProperty = 0x1;




        UObject* editableTextBoxClass  = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, STR("/Script/UMG.EditableTextBox"));
        UObject* IPeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree, STR("EditableTextBoxArchipelagoIP"));
        UObject* UsernameeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree, STR("EditableTextBoxArchipelagoUsername"));
        UObject* PasswordeditableTextBox = ConstructWidget(editableTextBoxClass, widgetTree, STR("EditableTextBoxArchipelagoPassword"));




        // Set the "Content" property of the WrapBoxSlot to be our new EditableTextBox
        UObject** wrapBoxSlotContentProperty = IPwrapBoxSlot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Content"));
        if (wrapBoxSlotContentProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Content property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotContentProperty = IPeditableTextBox;

        wrapBoxSlotContentProperty = UsernamewrapBoxSlot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Content"));
        if (wrapBoxSlotContentProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Content property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotContentProperty = UsernameeditableTextBox;

        wrapBoxSlotContentProperty = PasswordwrapBoxSlot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Content"));
        if (wrapBoxSlotContentProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Content property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotContentProperty = PasswordeditableTextBox;



        // Set the "Parent" property of the WrapBoxSlot to be the WrapBox
        UObject** wrapBoxSlotParentProperty = IPwrapBoxSlot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Parent"));
        if (wrapBoxSlotParentProperty == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find Parent property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotParentProperty = wrapBox;

        wrapBoxSlotParentProperty = UsernamewrapBoxSlot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Parent"));
        if (wrapBoxSlotParentProperty == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find Parent property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotParentProperty = wrapBox;

        wrapBoxSlotParentProperty = PasswordwrapBoxSlot->GetValuePtrByPropertyNameInChain<UObject*>(STR("Parent"));
        if (wrapBoxSlotParentProperty == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find Parent property in WrapBoxSlot\n"));
            return;
        }
        *wrapBoxSlotParentProperty = wrapBox;




        // Set the "Slot" property of the EditableTextBox
        UObject** editableTextBoxSlotProperty = IPeditableTextBox->GetValuePtrByPropertyNameInChain<UObject*>(STR("Slot"));
        if (editableTextBoxSlotProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Slot property in EditableTextBox\n"));
            return;
        }
        *editableTextBoxSlotProperty = IPwrapBoxSlot;

        editableTextBoxSlotProperty = UsernameeditableTextBox->GetValuePtrByPropertyNameInChain<UObject*>(STR("Slot"));
        if (editableTextBoxSlotProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Slot property in EditableTextBox\n"));
            return;
        }
        *editableTextBoxSlotProperty = UsernamewrapBoxSlot;

        editableTextBoxSlotProperty = PasswordeditableTextBox->GetValuePtrByPropertyNameInChain<UObject*>(STR("Slot"));
        if (editableTextBoxSlotProperty == nullptr)
        {
            Output::send<LogLevel::Error>(STR("Could not find Slot property in EditableTextBox\n"));
            return;
        }
        *editableTextBoxSlotProperty = PasswordwrapBoxSlot;





        // Set the "HintText" property of our new EditableTextBoxes
        FText* newHintText = IPeditableTextBox->GetValuePtrByPropertyNameInChain<FText>(STR("HintText"));
        if (newHintText == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find HintText property in new EditableTextBox\n"));
            return;
        }
        *newHintText = FText(STR("Archipelago Server IP"));

        newHintText = UsernameeditableTextBox->GetValuePtrByPropertyNameInChain<FText>(STR("HintText"));
        if (newHintText == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find HintText property in new EditableTextBox\n"));
            return;
        }
        *newHintText = FText(STR("Archipelago Username"));

        newHintText = PasswordeditableTextBox->GetValuePtrByPropertyNameInChain<FText>(STR("HintText"));
        if (newHintText == nullptr)    {
            Output::send<LogLevel::Error>(STR("Could not find HintText property in new EditableTextBox\n"));
            return;
        }
        *newHintText = FText(STR("Archipelago Password"));





        // Set the "Justification" property of our new EditableTextBox to center
        uint8_t* justificationProperty = IPeditableTextBox->GetValuePtrByPropertyNameInChain<uint8_t>(STR("Justification"));
        if (justificationProperty == nullptr)        {
            Output::send<LogLevel::Error>(STR("Could not find Justification property in new EditableTextBox\n"));
            return;
        }
        *justificationProperty = 0x1; // ETextJustify::Center

        justificationProperty = UsernameeditableTextBox->GetValuePtrByPropertyNameInChain<uint8_t>(STR("Justification"));
        if (justificationProperty == nullptr)        {
            Output::send<LogLevel::Error>(STR("Could not find Justification property in new EditableTextBox\n"));
            return;
        }
        *justificationProperty = 0x1; // ETextJustify::Center

        justificationProperty = PasswordeditableTextBox->GetValuePtrByPropertyNameInChain<uint8_t>(STR("Justification"));
        if (justificationProperty == nullptr)        {
            Output::send<LogLevel::Error>(STR("Could not find Justification property in new EditableTextBox\n"));
            return;
        }
        *justificationProperty = 0x1; // ETextJustify::Center




        UObject* fontObject = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, STR("/Game/BlueFire/HUD/Font/BlueFire-Spectral/BlueFire-SpectralSC-Bold_Font.BlueFire-SpectralSC-Bold_Font"));


        // Get the "WidgetStyle.Font.Size" and "WidgetStyle.Font.FontObject" property
        FStructProperty* StyleWidgetProperty = static_cast<FStructProperty*>(IPeditableTextBox->GetPropertyByNameInChain(STR("WidgetStyle")));
        auto StyleWidgetStruct = StyleWidgetProperty->GetStruct();
        auto StyleWidget = StyleWidgetProperty->ContainerPtrToValuePtr<void>(IPeditableTextBox);
        FStructProperty* FontProperty = static_cast<FStructProperty*>(StyleWidgetStruct->GetPropertyByNameInChain(STR("Font")));
        auto FontStruct = FontProperty->GetStruct();
        auto Font = FontProperty->ContainerPtrToValuePtr<void>(StyleWidget);
        FProperty* FontSizeProperty = static_cast<FProperty*>(FontStruct->GetPropertyByNameInChain(STR("Size")));
        FProperty* FontObjectProperty = static_cast<FStructProperty*>(FontStruct->GetPropertyByNameInChain(STR("FontObject")));

        uint32_t* newWidgetStyleFontSize = FontSizeProperty->ContainerPtrToValuePtr<uint32_t>(Font);
        UObject** newWidgetStyleFontObject = FontObjectProperty->ContainerPtrToValuePtr<UObject*>(Font);


        // Set the font object
        *newWidgetStyleFontObject = fontObject;

        // Set the font size
        *newWidgetStyleFontSize = 40;


        StyleWidgetProperty = static_cast<FStructProperty*>(UsernameeditableTextBox->GetPropertyByNameInChain(STR("WidgetStyle")));
        StyleWidgetStruct = StyleWidgetProperty->GetStruct();
        StyleWidget = StyleWidgetProperty->ContainerPtrToValuePtr<void>(UsernameeditableTextBox);
        FontProperty = static_cast<FStructProperty*>(StyleWidgetStruct->GetPropertyByNameInChain(STR("Font")));
        FontStruct = FontProperty->GetStruct();
        Font = FontProperty->ContainerPtrToValuePtr<void>(StyleWidget);
        FontSizeProperty = static_cast<FProperty*>(FontStruct->GetPropertyByNameInChain(STR("Size")));
        FontObjectProperty = static_cast<FStructProperty*>(FontStruct->GetPropertyByNameInChain(STR("FontObject")));

        newWidgetStyleFontSize = FontSizeProperty->ContainerPtrToValuePtr<uint32_t>(Font);
        newWidgetStyleFontObject = FontObjectProperty->ContainerPtrToValuePtr<UObject*>(Font);


        // Set the font object
        *newWidgetStyleFontObject = fontObject;

        // Set the font size
        *newWidgetStyleFontSize = 40;


        StyleWidgetProperty = static_cast<FStructProperty*>(PasswordeditableTextBox->GetPropertyByNameInChain(STR("WidgetStyle")));
        StyleWidgetStruct = StyleWidgetProperty->GetStruct();
        StyleWidget = StyleWidgetProperty->ContainerPtrToValuePtr<void>(PasswordeditableTextBox);
        FontProperty = static_cast<FStructProperty*>(StyleWidgetStruct->GetPropertyByNameInChain(STR("Font")));
        FontStruct = FontProperty->GetStruct();
        Font = FontProperty->ContainerPtrToValuePtr<void>(StyleWidget);
        FontSizeProperty = static_cast<FProperty*>(FontStruct->GetPropertyByNameInChain(STR("Size")));
        FontObjectProperty = static_cast<FStructProperty*>(FontStruct->GetPropertyByNameInChain(STR("FontObject")));

        newWidgetStyleFontSize = FontSizeProperty->ContainerPtrToValuePtr<uint32_t>(Font);
        newWidgetStyleFontObject = FontObjectProperty->ContainerPtrToValuePtr<UObject*>(Font);


        // Set the font object
        *newWidgetStyleFontObject = fontObject;

        // Set the font size
        *newWidgetStyleFontSize = 40;
	}
}


void BlueFireArchipelagoMod::CopyStyle()
{
    Output::send<LogLevel::Verbose>(STR("BlueFireArchipelagoMod says hello from SetTextPreHook ♥\n"));
}

bool BlueFireArchipelagoMod::isUserInArchipelagoMenu()
{
    std::vector<UObject*> foundObjects;
    UObject* widgetSwitcher = nullptr;

    UObjectGlobals::FindAllOf(STR("WidgetSwitcher"), foundObjects);

    for(UObject* foundObj : foundObjects)
    {
        if (foundObj->GetFullName().starts_with(STR("WidgetSwitcher /Engine/")) &&
            foundObj->GetNamePrivate().ToString().ends_with(STR("WidgetSwitcher_0")) &&
            foundObj->GetFullName().find(STR("WidgetSwitcher_0")) == foundObj->GetFullName().rfind(STR("WidgetSwitcher_0")) &&
            foundObj->GetFullName().find(STR("GameMenu_C")) != std::string::npos)
        {
            Output::send<LogLevel::Verbose>(STR("Found WidgetSwitcher object: {}\n"), foundObj->GetNamePrivate().ToString());
            widgetSwitcher = foundObj;
            break;
        }
    }

    if(!widgetSwitcher)
    {
        Output::send<LogLevel::Error>(STR("Could not find WidgetSwitcher object\n"));
        return false;
    }

    uint32_t* activeWidgetIndex = widgetSwitcher->GetValuePtrByPropertyNameInChain<uint32_t>(STR("ActiveWidgetIndex"));
    if (activeWidgetIndex == nullptr)
    {
        Output::send<LogLevel::Error>(STR("Could not find ActiveWidgetIndex property in WidgetSwitcher\n"));
        return false;
    }

    // If we are not in the file naming section of the menu
    if (*activeWidgetIndex != 8)
    {
        return false;
    }

    return true;
}