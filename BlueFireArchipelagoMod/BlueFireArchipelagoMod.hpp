#pragma once

#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp> 
#include <Unreal/UObject.hpp>

#include <Unreal/Hooks.hpp>
#include <Unreal/UFunction.hpp>

#include <ObjectCreateListener.hpp>
#include <HookManager.hpp>

using namespace RC;
using namespace Unreal;

class BlueFireArchipelagoMod : public RC::CppUserModBase
{
public:
    static inline ObjectCreateListener ObjectCreateListener{};
    static inline HookManager HookManager{};

    static inline uint8_t fileNamingActiveWidgetIndex;


    BlueFireArchipelagoMod();

    void on_unreal_init();

    static void ObjectCreatedNewItem(const UObjectBase* object, int32 index);
    static void ObjectCreatedEditableTextBox(const UObjectBase* object, int32 index);

    static bool PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool UpKeyPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);
    static bool DownKeyPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);


    static void CopyStyle();

private:
    static bool UpdateFileNamingFocus();
    static bool isUserInArchipelagoMenu();
    static void onReturnPressed();

    static inline bool bFileTextBoxRemoved{false};
};