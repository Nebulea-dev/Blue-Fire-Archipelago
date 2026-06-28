#include <Unreal/UObject.hpp>
#include <Unreal/CoreUObject/UObject/Class.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include <Helper/HookHelper.hpp>

using namespace RC;
using namespace Unreal;

HookHelper::HookHelper()
    : prehooksRegistered(), posthooksRegistered(), functionsToExecuteInGameThread()
{
    Hook::RegisterProcessLocalScriptFunctionPreCallback([&]([[maybe_unused]] Hook::TCallbackIterationData<void>& CallbackIterationData, [[maybe_unused]] UObject* Context, FFrame& Stack, [[maybe_unused]] void* RESULT_DECL)
    {
        // Get name of the function that was called
        const std::wstring objectName = Stack.Node()->GetFullName();

        /*
        if(objectName.find(STR("Ubergraph")) == std::string::npos &&
           objectName.find(STR("Tick")) == std::string::npos &&
           objectName.find(STR("ReceiveTraceHandler")) == std::string::npos &&
           objectName.find(STR("ReceivePlayerHandler")) == std::string::npos &&
           objectName.find(STR("TraceReplace")) == std::string::npos &&
           objectName.find(STR("K2Node")) == std::string::npos &&
           objectName.find(STR("Camera")) == std::string::npos &&
           objectName.find(STR("UpdatePlayerLoc")) == std::string::npos &&
           objectName.find(STR("UpdateFootOffset")) == std::string::npos &&
           objectName.find(STR("UpdateIK")) == std::string::npos &&
           objectName.find(STR("UpdateSurface")) == std::string::npos &&
           objectName.find(STR(":UpdateSize")) == std::string::npos &&
           objectName.find(STR("ReceiveDrawHUD")) == std::string::npos &&
           objectName.find(STR("Global_Controller_C:Timer")) == std::string::npos &&
           objectName.find(STR("CheckDistance")) == std::string::npos &&
           objectName.find(STR(":GetHeadRotActors")) == std::string::npos &&
           objectName.find(STR(":MusicCallback2")) == std::string::npos &&
           objectName.find(STR("Inventory_C:Move")) == std::string::npos &&
           objectName.find(STR("changertpcvalue")) == std::string::npos &&
           objectName.find(STR("BreathSound")) == std::string::npos &&
           objectName.find(STR("OnopFlute")) == std::string::npos &&
           objectName.find(STR("AlreadyOutOfVolume")) == std::string::npos &&
           //objectName.find(STR("BlueFireHUD")) == std::string::npos &&
           //objectName.find(STR("BlueFireHud")) == std::string::npos &&
           //objectName.find(STR("HUD/Inventory")) == std::string::npos &&
           objectName.find(STR("CheckDash")) == std::string::npos &&
           objectName.find(STR("Life")) == std::string::npos &&
           objectName.find(STR("ChangeIdle1")) == std::string::npos &&
           objectName.find(STR("Fog")) == std::string::npos &&
           objectName.find(STR("Random_MoveTo")) == std::string::npos &&
           objectName.find(STR("PerformConditionCheck")) == std::string::npos &&
           objectName.find(STR("Cutscenes")) == std::string::npos &&
           objectName.find(STR("Animation")) == std::string::npos)
        {
            Output::send<LogLevel::Verbose>(STR("Function call : {}\n"), objectName);
        }

        if(objectName.find(STR("TunicMaker")) != std::string::npos && objectName.find(STR("Ubergraph")) == std::string::npos)
        {
            Output::send<LogLevel::Verbose>(STR("Function call : {}\n"), objectName);
        }

        if(objectName.find(STR("Shop")) != std::string::npos && objectName.find(STR("Ubergraph")) == std::string::npos)
        {
            Output::send<LogLevel::Verbose>(STR("Function call : {}\n"), objectName);
        }

        if(objectName.find(STR("MainDialogWB")) != std::string::npos && objectName.find(STR("Ubergraph")) == std::string::npos && objectName.find(STR("Tick")) == std::string::npos)
        {
            Output::send<LogLevel::Verbose>(STR("Function call : {}\n"), objectName);
        }
        */

        // Find in the Map the object with the right name
        std::map<std::wstring, HookFunctionSignature>::iterator objectCallbackIt = prehooksRegistered.find(objectName);

        // If object is not in the objectsListened Map
        if (objectCallbackIt == prehooksRegistered.end()) return;

        // Call the callback associated with the object
        HookFunctionSignature objectCallback = objectCallbackIt->second;
        bool shouldPreventCall = objectCallback(Context, Stack, RESULT_DECL);

        // If the callback returns true, prevent the original function from being called
        if (shouldPreventCall)
        {
            CallbackIterationData.PreventOriginalFunctionCall();
        }
    }, { false, false, STR("NebuleasMod"), STR("NebuleasPreHookWrapper") });

    Hook::RegisterProcessLocalScriptFunctionPostCallback([&]([[maybe_unused]] Hook::TCallbackIterationData<void>& CallbackIterationData, [[maybe_unused]] UObject* Context, FFrame& Stack, [[maybe_unused]] void* RESULT_DECL)
    {
        // Get name of the function that was called
        const std::wstring objectName = Stack.Node()->GetFullName();

        // Find in the Map the object with the right name
        std::map<std::wstring, HookFunctionSignature>::iterator objectCallbackIt = posthooksRegistered.find(objectName);

        // If object is not in the objectsListened Map
        if (objectCallbackIt == posthooksRegistered.end()) return;

        // Call the callback associated with the object
        HookFunctionSignature objectCallback = objectCallbackIt->second;
        bool shouldPreventCall = objectCallback(Context, Stack, RESULT_DECL);

        // If the callback returns true, prevent the original function from being called
        if (shouldPreventCall)
        {
            CallbackIterationData.PreventOriginalFunctionCall();
        }
    }, { false, false, STR("NebuleasMod"), STR("NebuleasPostHookWrapper") });

    Output::send<LogLevel::Verbose>(STR("HookHelper instance created\n"));
}

bool HookHelper::registerPreHook(std::wstring objectName, HookFunctionSignature callback)
{
	// If object is already in the objectsListened Map
	auto objectCallbackIt = this->prehooksRegistered.find(objectName);
	if (objectCallbackIt != this->prehooksRegistered.end() && objectCallbackIt->second == callback) return false;

	this->prehooksRegistered.insert({ objectName, callback });

	return true;
}

bool HookHelper::registerPostHook(std::wstring objectName, HookFunctionSignature callback)
{
	// If object is already in the objectsListened Map
	auto objectCallbackIt = this->posthooksRegistered.find(objectName);
	if (objectCallbackIt != this->posthooksRegistered.end() && objectCallbackIt->second == callback) return false;

	this->posthooksRegistered.insert({ objectName, callback });

	return true;
}

void HookHelper::deleteHook(std::wstring objectName)
{
    this->prehooksRegistered.erase(objectName);
    this->posthooksRegistered.erase(objectName);
}

auto static engine_tick_hook([[maybe_unused]] Unreal::Hook::TCallbackIterationData<void>& CallbackIterationData,
                                 [[maybe_unused]] Unreal::UEngine* Context,
                                 [[maybe_unused]] float DeltaSeconds,
                                 [[maybe_unused]] bool bIdle) -> void
{


}

void HookHelper::executeInGameThread(std::function<void()> func)
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