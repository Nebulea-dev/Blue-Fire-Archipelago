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
        bool shouldPreventCall = objectCallback(Context, Stack, RESULT_DECL);

        // If the callback returns true, prevent the original function from being called
        if (shouldPreventCall)
        {
            CallbackIterationData.PreventOriginalFunctionCall();
        }
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

auto static engine_tick_hook([[maybe_unused]] Unreal::Hook::TCallbackIterationData<void>& CallbackIterationData,
                                 [[maybe_unused]] Unreal::UEngine* Context,
                                 [[maybe_unused]] float DeltaSeconds,
                                 [[maybe_unused]] bool bIdle) -> void
{


}

// Receives a lambda function and adds it to a queue of functions to be executed in the game thread, then registers a hook on the engine tick to execute the functions in the queue
void HookManager::executeInGameThread(std::function<void()> func)
{
    // Add function to the queue of functions to be executed in the game thread
    functionsToExecuteInGameThread.push(func);

    Unreal::Hook::RegisterEngineTickPreCallback([&]([[maybe_unused]] Unreal::Hook::TCallbackIterationData<void>& CallbackIterationData,
                                 [[maybe_unused]] Unreal::UEngine* Context,
                                 [[maybe_unused]] float DeltaSeconds,
                                 [[maybe_unused]] bool bIdle)
    {
        // Execute all functions in the queue
        while (!functionsToExecuteInGameThread.empty())
        {
            std::function<void()> func = functionsToExecuteInGameThread.front();
            func();
            functionsToExecuteInGameThread.pop();
        }
    }, {true, false, STR("NebuleasMod"), STR("NebuleasEngineTickPreHook")});
}