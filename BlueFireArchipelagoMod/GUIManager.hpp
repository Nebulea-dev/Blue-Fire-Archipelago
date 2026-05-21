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

};