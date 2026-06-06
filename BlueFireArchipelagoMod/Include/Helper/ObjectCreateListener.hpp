#pragma once

#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

using ProcessLocalScriptFunctionSignature = void(*)(const UObjectBase* object, int32 index);

/*******************************************************************************
 * ObjectCreateListener
 *
 * @brief   Listener for Unreal object creation events.
 *
 *          Inherits from FUObjectCreateListener to intercept object creation
 *          in the Unreal engine. Allows registration of callbacks that fire
 *          when specific object types are created.
 */
struct ObjectCreateListener : public FUObjectCreateListener {
public:
    // TODO : add documentation
    ObjectCreateListener();

    /*******************************************************************************
     * @fn      registerObjectCallback
     *
     * @brief   Register a callback to fire when an object of a given type is created.
     *
     *          The callback will be invoked whenever an object matching the
     *          specified name is instantiated in the engine.
     *
     * @param   objectName - Name or pattern of object type to listen for
     * @param   callback   - Function to call when object is created
     *
     * @return  true if callback registered successfully, false otherwise
     */
    bool registerObjectCallback(std::wstring objectName, ProcessLocalScriptFunctionSignature callback);

    /*******************************************************************************
     * @fn      deleteObjectCallback
     *
     * @brief   Unregister a previously registered object creation callback.
     *
     * @param   objectName - Name of the object type to stop listening for
     *
     * @return  true if callback was removed, false if not found
     */
    bool deleteObjectCallback(std::wstring objectName);

private:
    /*******************************************************************************
     * @brief   Map of object names to their registered callbacks.
     */
    std::map<std::wstring, ProcessLocalScriptFunctionSignature> objectsListened;

    /*******************************************************************************
     * @fn      NotifyUObjectCreated
     *
     * @brief   Callback invoked when any object is created in the engine.
     *
     *          Overridden from FUObjectCreateListener to implement custom
     *          object creation event handling. Checks if the created object
     *          matches any registered callbacks and executes them.
     *
     * @param   object - Pointer to the newly created object
     * @param   index  - Index of the object in the object array
     *
     * @return  none
     */
    void NotifyUObjectCreated(const UObjectBase* object, int32 index) override;

    /*******************************************************************************
     * @fn      OnUObjectArrayShutdown
     *
     * @brief   Callback invoked when the object array is being shut down.
     *
     *          Overridden from FUObjectCreateListener to clean up when the
     *          engine is shutting down.
     *
     * @return  none
     */
    void OnUObjectArrayShutdown() override;
};