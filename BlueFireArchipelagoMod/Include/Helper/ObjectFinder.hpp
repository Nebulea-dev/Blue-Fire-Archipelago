#pragma once

#include <optional>
#include <Unreal/UObject.hpp>

using namespace RC;
using namespace Unreal;

/*******************************************************************************
 * ObjectFinder
 *
 * @brief   Low-level utility for searching the Unreal object hierarchy.
 *
 *          Provides static methods to locate objects by full name with
 *          optional filtering. Base functionality used by higher-level
 *          query helpers.
 */
class ObjectFinder
{
public:
	/*******************************************************************************
	 * @fn      FindObject
	 *
	 * @brief   Find an object in the Unreal object hierarchy by name and filter.
	 *
	 *          Searches the global object array for an object matching the
	 *          provided full name, applying an optional filter lambda to
	 *          further refine the search.
	 *
	 * @param   objectFullName - Full name path of the object to find
	 * @param   filter         - Lambda returning true if object matches criteria
	 *
	 * @return  optional containing pointer to found object, empty if not found
	 */
	static std::optional<UObject*> FindObject(const std::wstring& objectFullName, std::function<bool(UObject*)> filter);
};