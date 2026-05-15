#include <Unreal/UObject.hpp>
#include <Unreal/UFunction.hpp>

#include <HookManager.hpp>

using namespace RC;
using namespace Unreal;

void HookManager::Init()
{
    Hook::RegisterProcessLocalScriptFunctionPreCallback([&]([[maybe_unused]] Hook::TCallbackIterationData<void>& CallbackIterationData, [[maybe_unused]] UObject* Context, FFrame& Stack, [[maybe_unused]] void* RESULT_DECL)
    {
        // Get name of the function that was called
        const std::wstring objectName = Stack.Node()->GetNamePrivate().ToString();

        // Find in the Map the object with the right name
        std::map<std::wstring, HookFunctionSignature>::iterator objectCallbackIt = hooksRegistered.find(objectName);

        // If object is not in the objectsListened Map
        if (objectCallbackIt == hooksRegistered.end()) return;

        // Call the callback associated with the object
        HookFunctionSignature objectCallback = objectCallbackIt->second;
        objectCallback(Context, Stack, RESULT_DECL);
    }, { false, false, STR("NebuleasMod"), STR("NebuleasPreHookWrapper") });
}

bool HookManager::registerHook(std::wstring objectName, HookFunctionSignature callback)
{

	// If object is already in the objectsListened Map
	auto objectCallbackIt = this->hooksRegistered.find(objectName);
	if (objectCallbackIt != this->hooksRegistered.end() && objectCallbackIt->second == callback) return false;

	this->hooksRegistered.insert({ objectName, callback });

	return true;
}

void HookManager::deleteHook(std::wstring objectName)
{
	this->hooksRegistered.erase(objectName);
}