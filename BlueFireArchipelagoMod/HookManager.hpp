#pragma once

#include <queue>

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/FProperty.hpp>
#include <DynamicOutput/DynamicOutput.hpp> 

using namespace RC;
using namespace Unreal;

using HookFunctionSignature = bool(*)(UObject* Context, FFrame& Stack, void* RESULT_DECL);

class HookManager {
public:
    // Inits the UE hooks. Need to be called from on_unreal_init
    void Init();

    // Registers a callback for a specific object name
    // Returns true if hook was registered with the same callback
    // Returns false if objectName was already hooked with the same function
    bool registerHook(std::wstring funcName, HookFunctionSignature callback);

    // Delete a callback for a specific object name that was previously registered through registerObjectCallback
    void deleteHook(std::wstring objectName);

    // Read the value of a parameter of the function
    // Needs to be called from a hooked function
    template <class T>
    static std::optional<T> readParamValue(std::wstring paramName, FFrame& Stack);

    // Changes the value of a parameter of the function
    // Needs to be called from a hooked function
    // Returns true if value was written
    // Returns false if param was not found
    template <class T>
    static bool setParamValue(std::wstring paramName, FFrame& Stack, T object);
    template <class T>
    static bool setParamValue(std::wstring paramName, FFrame& Stack, T* object);

    void executeInGameThread(std::function<void()> func);

    std::queue<std::function<void()>> functionsToExecuteInGameThread;

private:
    // We can't use FName as Maps use comparator operators for insertion 
    // and FName does not implement them
    std::map<std::wstring, HookFunctionSignature> hooksRegistered;
};


template <class T>
std::optional<T> HookManager::readParamValue(std::wstring paramName, FFrame& Stack)
{
    UFunction* function = Stack.Node();
    uint8_t* paramsStack = Stack.Locals();

    T paramValue;

    for (const FProperty* param : TFieldRange<FProperty>(function))
    {
        if (param->GetName() == paramName)
        {
            int32_t offset = param->GetOffset_ForInternal();
            memcpy(&paramValue, paramsStack + offset, sizeof(T));

            return paramValue;
        }
    }

    return {};
}

// Changes the value of a parameter of the function
template <class T>
bool HookManager::setParamValue(std::wstring paramName, FFrame& Stack, T object)
{
    UFunction* function = Stack.Node();
    uint8_t* paramsStack = Stack.Locals();

    for (const FProperty* param : TFieldRange<FProperty>(function))
    {
        if (param->GetName() == paramName)
        {
            int32_t offset = param->GetOffset_ForInternal();
            memcpy(paramsStack + offset, &object, sizeof(T));

            return true;
        }
    }

    return false;
}

// Changes the value of a parameter of the function
template <class T>
bool HookManager::setParamValue(std::wstring paramName, FFrame& Stack, T* object)
{
    UFunction* function = Stack.Node();
    uint8_t* paramsStack = Stack.Locals();

    for (const FProperty* param : TFieldRange<FProperty>(function))
    {
        if (param->GetName() == paramName)
        {
            int32_t offset = param->GetOffset_ForInternal();
            memcpy(paramsStack + offset, object, sizeof(T));

            return true;
        }
    }

    return false;
}