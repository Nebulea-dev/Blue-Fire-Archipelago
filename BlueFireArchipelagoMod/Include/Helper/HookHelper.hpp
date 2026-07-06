#pragma once

#include <queue>

#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <DynamicOutput/DynamicOutput.hpp> 

using namespace RC;
using namespace Unreal;

using HookFunctionSignature = bool(*)(UObject* Context, FFrame& Stack, void* RESULT_DECL);

/*******************************************************************************
 * HookHelper
 *
 * @brief   Helper utility class for managing Unreal Engine function hooks.
 *
 *          This class manages registration and execution of pre and post-hooks
 *          on Unreal functions, allowing interception and modification of
 *          function calls at runtime.
 */
class HookHelper {
public:
    /*******************************************************************************
     * @fn      HookHelper
     *
     * @brief   Constructor - initializes the hook helper infrastructure.
     *
     *          Must be called after Unreal has initialized and before any
     *          hooks are registered.
     *
     * @return  none
     */
    HookHelper();

    /*******************************************************************************
     * @fn      ~HookHelper
     *
     * @brief   Destructor - cleans up registered hooks and resources.
     *
     *          Unregisters all hooks and clears internal state.
     *
     * @return  none
     */
    ~HookHelper();

    /*******************************************************************************
     * @fn      registerPreHook
     *
     * @brief   Register a callback to be executed before a function runs.
     *
     *          The callback can examine and modify function parameters.
     *          Returning true from the callback prevents the original
     *          function from executing.
     *
     * @param   funcName   - Full path to the function to hook (e.g., "/Game/Func.Class:Method")
     * @param   callback   - Function pointer matching HookFunctionSignature
     *
     * @return  true if hook registered successfully, false if already hooked
     */
    bool registerPreHook(std::wstring funcName, HookFunctionSignature callback);

    /*******************************************************************************
     * @fn      registerPostHook
     *
     * @brief   Register a callback to be executed after a function completes.
     *
     *          The callback is invoked after the original function has
     *          finished executing.
     *
     * @param   funcName   - Full path to the function to hook
     * @param   callback   - Function pointer matching HookFunctionSignature
     *
     * @return  true if hook registered successfully, false if already hooked
     */
    bool registerPostHook(std::wstring funcName, HookFunctionSignature callback);

    /*******************************************************************************
     * @fn      deleteHook
     *
     * @brief   Remove a previously registered hook callback.
     *
     *          Removes both pre and post-hook callbacks for the given function.
     *
     * @param   objectName - Full path to the function to unhook
     *
     * @return  none
     */
    void deleteHook(std::wstring objectName);

    /*******************************************************************************
     * @fn      readParamValue
     *
     * @brief   Read the value of a function parameter from within a hook.
     *
     *          Must be called only from within a registered hook callback.
     *          Uses reflection to locate and extract the parameter value.
     *
     * @param   paramName - Name of the parameter to read
     * @param   Stack     - Function stack frame from the hook callback
     *
     * @return  optional containing parameter value if found, empty otherwise
     */
    template <class T>
    static std::optional<T> readParamValue(std::wstring paramName, FFrame& Stack);

    /*******************************************************************************
     * @fn      setParamValue (pointer overload)
     *
     * @brief   Modify the value of a pointer parameter from within a hook.
     *
     *          Specialized overload for pointer types. Copies the pointer
     *          value to the parameter location.
     *
     * @param   paramName - Name of the parameter to modify
     * @param   Stack     - Function stack frame from the hook callback
     * @param   object    - Pointer to copy to the parameter
     *
     * @return  none
     */
    template <class T>
    static void setParamValue(std::wstring paramName, FFrame& Stack, T* object);

    /*******************************************************************************
     * @fn      executeInGameThread
     *
     * @brief   Queue a function to be executed on the game thread.
     *
     *          Registers a hook on engine tick to execute queued functions,
     *          ensuring they run on the same thread as game logic.
     *
     * @param   func - Lambda or function to execute on game thread
     *
     * @return  none
     */
    void executeInGameThread(std::function<void()> func);

    /*******************************************************************************
     * @brief   Queue of functions waiting to execute on game thread.
     *
     *          These functions are executed on the next engine tick.
     */
    std::queue<std::function<void()>> functionsToExecuteInGameThread;

private:
    /// Map of function names to pre-hook callbacks
    std::map<std::wstring, HookFunctionSignature> prehooksRegistered;

    /// Map of function names to post-hook callbacks
    std::map<std::wstring, HookFunctionSignature> posthooksRegistered;

    // Ids of Unreal Engine hook callbacks
    RC::Unreal::Hook::GlobalCallbackId globalCallbackIds[2];
};


template <class T>
std::optional<T> HookHelper::readParamValue(std::wstring paramName, FFrame& Stack)
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
void HookHelper::setParamValue(std::wstring paramName, FFrame& Stack, T* object)
{
    UFunction* function = Stack.Node();
    uint8_t* paramsStack = Stack.Locals();

    for (const FProperty* param : TFieldRange<FProperty>(function))
    {
        if (param->GetName() == paramName)
        {
            int32_t offset = param->GetOffset_ForInternal();
            memcpy(paramsStack + offset, object, sizeof(T));

            return;
        }
    }

    Output::send<LogLevel::Error>(STR("Could not find param {}\n"), paramName);
}