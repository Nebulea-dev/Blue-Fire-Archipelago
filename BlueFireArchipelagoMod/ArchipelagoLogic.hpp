#pragma once

#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

class ArchipelagoLogic
{
	public:
		static void init();
		static void itemClearCallback();
		static void itemReceiveCallback(int64_t item, bool notifyPlayer);
		static void locationCheckCallback(int64_t location);
		static void connectToArchipelagoServer(const FText* IP, const FText* Username, const FText* Password);
};