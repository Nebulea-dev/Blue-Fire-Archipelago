#pragma once

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>

using namespace RC;
using namespace Unreal;

class ItemManager {
	public:
	static void itemReceiveCb(int itemID, bool notify);

	static bool PlayNewItemPreHook(UObject* Context, FFrame& Stack, void* RESULT_DECL);
};