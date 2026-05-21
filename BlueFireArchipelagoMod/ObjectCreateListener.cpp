#include <Unreal/UObject.hpp>
#include <Unreal/CoreUObject/UObject/Class.hpp>

#include <ObjectCreateListener.hpp>

using namespace RC;
using namespace Unreal;

void ObjectCreateListener::NotifyUObjectCreated(const UObjectBase* object, int32 index)
{
	// Get name of the object that was created
	const StringType objectName = ((UObject*)object)->GetClassPrivate()->GetName();

	// Find in the Map the object with the right name
	std::map<std::wstring, ProcessLocalScriptFunctionSignature>::iterator objectCallbackIt = this->objectsListened.find(objectName);

	// If object is not in the objectsListened Map
	if (objectCallbackIt == this->objectsListened.end()) return;

	// Call the callback associated with the object
	ProcessLocalScriptFunctionSignature objectCallback = objectCallbackIt->second;
	objectCallback(object, index);
}

bool ObjectCreateListener::registerObjectCallback(std::wstring objectName, ProcessLocalScriptFunctionSignature callback)
{

	// If object is already in the objectsListened Map
	auto objectCallbackIt = this->objectsListened.find(objectName);
	if (objectCallbackIt != this->objectsListened.end()) return false;

	this->objectsListened.insert({ objectName, callback });

	return true;
}

bool ObjectCreateListener::deleteObjectCallback(std::wstring objectName)
{
	return this->objectsListened.erase(objectName);
}

void ObjectCreateListener::OnUObjectArrayShutdown() {}