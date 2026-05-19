#pragma once

#include <optional>
#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

class ObjectFinder
{
public:
	// Helper function to find an object by its full name and a lambda filter function.
	static std::optional<UObject*> FindObject(const std::wstring& objectFullName, std::function<bool(UObject*)> filter);
};