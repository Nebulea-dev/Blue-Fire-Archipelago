#pragma once

#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

using ProcessLocalScriptFunctionSignature = void(*)(const UObjectBase* object, int32 index);

struct ObjectCreateListener : public FUObjectCreateListener {
public:
    // Registers a callback for a specific object name
    bool registerObjectCallback(std::wstring objectName, ProcessLocalScriptFunctionSignature callback);


    // Delete a callback for a specific object name that was previously registered through registerObjectCallback
    bool deleteObjectCallback(std::wstring objectName);

private:
    std::map<std::wstring, ProcessLocalScriptFunctionSignature> objectsListened;

    // Will be called when any object is created in Unity
    // Overriden to create custom implementation
    void NotifyUObjectCreated(const UObjectBase* object, int32 index) override;
    void OnUObjectArrayShutdown() override;
};