#include <Unreal/UObject.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/FProperty.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UFunction.hpp>

#include <ObjectFinder.hpp>

std::optional<UObject*> ObjectFinder::FindObject(const std::wstring& objectFullName, std::function<bool(UObject*)> filter)
{
	std::vector<UObject*> foundObjects;

	UObjectGlobals::FindAllOf(objectFullName, foundObjects);

	for(UObject* foundObj : foundObjects)
	{
		// Check against filter function
		if (filter(foundObj))
		{
			return foundObj;
		}
	}

	Output::send<LogLevel::Error>(STR("Could not find object with full name: {}\n"), objectFullName);
	return std::nullopt;
}