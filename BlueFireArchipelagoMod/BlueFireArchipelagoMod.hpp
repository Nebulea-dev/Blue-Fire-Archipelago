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


    BlueFireArchipelagoMod();

    void on_unreal_init();

    static void ObjectCreatedNewItem(const UObjectBase* object, int32 index);

    static void PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);
};