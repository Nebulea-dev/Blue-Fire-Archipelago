#pragma once

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

/**
 * LocationsJsonParser
 *
 * @brief Parses the unified locations.json file for the Archipelago mod.
 *        Location IDs are auto-generated based on the order they appear in the JSON.
 */
class LocationsJsonParser
{
public:
    struct Location
    {
        std::wstring name;
        std::wstring chestName;
        uint32_t locationId;  // Auto-generated based on parse order
    };

    struct Subregion
    {
        std::wstring name;
        std::vector<Location> locations;
    };

    struct Region
    {
        std::wstring name;
        std::vector<Subregion> subregions;
        std::vector<Location> locations;
    };

    struct Statue
    {
        std::wstring name;
        std::wstring objectName;
        uint32_t locationId;  // Auto-generated based on order after all regular locations
    };

    /**
     * Parse the locations.json file
     * @param jsonPath Path to the locations.json file
     * @return true if parsing succeeded, false otherwise
     */
    static bool ParseLocationsJson(const std::string& jsonPath)
    {
        std::ifstream file(jsonPath);
        if (!file.is_open())
        {
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        return ParseJsonContent(content);
    }

    /**
     * Build the ChestNameToLocationID map from parsed data
     * @return A map of chest names to location IDs
     */
    static std::map<std::wstring, uint32_t> GetChestNameToLocationIDMap()
    {
        std::map<std::wstring, uint32_t> result;

        for (const auto& region : s_regions)
        {
            // Handle subregions
            for (const auto& subregion : region.subregions)
            {
                for (const auto& location : subregion.locations)
                {
                    if (!location.chestName.empty())
                    {
                        result[location.chestName] = location.locationId;
                    }
                }
            }

            // Handle locations without subregions
            for (const auto& location : region.locations)
            {
                if (!location.chestName.empty())
                {
                    result[location.chestName] = location.locationId;
                }
            }
        }

        return result;
    }

    /**
     * Build the StatueNameToLocationID map from parsed data
     * @return A map of statue names to location IDs
     */
    static std::map<std::wstring, uint32_t> GetStatueNameToLocationIDMap()
    {
        std::map<std::wstring, uint32_t> result;

        for (const auto& statue : s_statues)
        {
            result[statue.objectName] = statue.locationId;
        }

        return result;
    }

    /**
     * Get all parsed regions
     */
    static const std::vector<Region>& GetRegions()
    {
        return s_regions;
    }

    /**
     * Get all parsed statues
     */
    static const std::vector<Statue>& GetStatues()
    {
        return s_statues;
    }

private:
    static std::vector<Region> s_regions;
    static std::vector<Statue> s_statues;
    static uint32_t s_locationIdCounter;

    /**
     * Simple JSON parser for locations.json
     * Location IDs are assigned incrementally as locations are parsed.
     */
    static bool ParseJsonContent(const std::string& content)
    {
        s_regions.clear();
        s_statues.clear();
        s_locationIdCounter = 0;

        // Find the "regions" array
        size_t regionsStart = content.find("\"regions\"");
        if (regionsStart == std::string::npos)
        {
            return false;
        }

        regionsStart = content.find('[', regionsStart);
        if (regionsStart == std::string::npos)
        {
            return false;
        }

        // Find the "statues" array
        size_t statuesStart = content.find("\"statues\"");
        if (statuesStart == std::string::npos)
        {
            return false;
        }

        statuesStart = content.find('[', statuesStart);
        if (statuesStart == std::string::npos)
        {
            return false;
        }

        // Parse regions (from regionsStart to statuesStart)
        ParseRegionsArray(content.substr(regionsStart, statuesStart - regionsStart));

        // Parse statues (from statuesStart to end)
        ParseStatuesArray(content.substr(statuesStart));

        return true;
    }

    static void ParseRegionsArray(const std::string& regionsJson)
    {
        size_t braceCount = 0;
        size_t regionStart = 0;
        bool inRegion = false;

        for (size_t i = 0; i < regionsJson.length(); ++i)
        {
            if (regionsJson[i] == '{')
            {
                if (!inRegion)
                {
                    inRegion = true;
                    regionStart = i;
                }
                braceCount++;
            }
            else if (regionsJson[i] == '}')
            {
                braceCount--;
                if (braceCount == 0 && inRegion)
                {
                    inRegion = false;
                    std::string regionObj = regionsJson.substr(regionStart, i - regionStart + 1);
                    ParseRegionObject(regionObj);
                }
            }
        }
    }

    static void ParseRegionObject(const std::string& regionJson)
    {
        Region region;

        // Parse name
        ExtractStringField(regionJson, "\"name\"", region.name);

        // Parse subregions
        size_t subregionsPos = regionJson.find("\"subregions\"");
        if (subregionsPos != std::string::npos)
        {
            size_t arrayStart = regionJson.find('[', subregionsPos);
            if (arrayStart != std::string::npos)
            {
                size_t arrayEnd = FindMatchingBracket(regionJson, arrayStart);
                std::string subregionsArray = regionJson.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                ParseSubregionsArray(subregionsArray, region.subregions);
            }
        }

        // Parse direct locations
        size_t locationsPos = regionJson.find("\"locations\"");
        if (locationsPos != std::string::npos)
        {
            size_t arrayStart = regionJson.find('[', locationsPos);
            if (arrayStart != std::string::npos)
            {
                size_t arrayEnd = FindMatchingBracket(regionJson, arrayStart);
                std::string locationsArray = regionJson.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                ParseLocationsArray(locationsArray, region.locations);
            }
        }

        s_regions.push_back(region);
    }

    static void ParseSubregionsArray(const std::string& subregionsJson, std::vector<Subregion>& subregions)
    {
        size_t braceCount = 0;
        size_t subregionStart = 0;
        bool inSubregion = false;

        for (size_t i = 0; i < subregionsJson.length(); ++i)
        {
            if (subregionsJson[i] == '{')
            {
                if (!inSubregion)
                {
                    inSubregion = true;
                    subregionStart = i;
                }
                braceCount++;
            }
            else if (subregionsJson[i] == '}')
            {
                braceCount--;
                if (braceCount == 0 && inSubregion)
                {
                    inSubregion = false;
                    std::string subregionObj = subregionsJson.substr(subregionStart, i - subregionStart + 1);
                    Subregion subregion;
                    ExtractStringField(subregionObj, "\"name\"", subregion.name);

                    size_t locationsPos = subregionObj.find("\"locations\"");
                    if (locationsPos != std::string::npos)
                    {
                        size_t arrayStart = subregionObj.find('[', locationsPos);
                        if (arrayStart != std::string::npos)
                        {
                            size_t arrayEnd = FindMatchingBracket(subregionObj, arrayStart);
                            std::string locationsArray = subregionObj.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                            ParseLocationsArray(locationsArray, subregion.locations);
                        }
                    }

                    subregions.push_back(subregion);
                }
            }
        }
    }

    static void ParseLocationsArray(const std::string& locationsJson, std::vector<Location>& locations)
    {
        size_t braceCount = 0;
        size_t locationStart = 0;
        bool inLocation = false;

        for (size_t i = 0; i < locationsJson.length(); ++i)
        {
            if (locationsJson[i] == '{')
            {
                if (!inLocation)
                {
                    inLocation = true;
                    locationStart = i;
                }
                braceCount++;
            }
            else if (locationsJson[i] == '}')
            {
                braceCount--;
                if (braceCount == 0 && inLocation)
                {
                    inLocation = false;
                    std::string locationObj = locationsJson.substr(locationStart, i - locationStart + 1);
                    Location location;
                    location.chestName = L"";
                    location.locationId = s_locationIdCounter++;

                    ExtractStringField(locationObj, "\"name\"", location.name);
                    ExtractStringField(locationObj, "\"chestName\"", location.chestName);

                    locations.push_back(location);
                }
            }
        }
    }

    static void ParseStatuesArray(const std::string& statuesJson)
    {
        size_t braceCount = 0;
        size_t statueStart = 0;
        bool inStatue = false;

        for (size_t i = 0; i < statuesJson.length(); ++i)
        {
            if (statuesJson[i] == '{')
            {
                if (!inStatue)
                {
                    inStatue = true;
                    statueStart = i;
                }
                braceCount++;
            }
            else if (statuesJson[i] == '}')
            {
                braceCount--;
                if (braceCount == 0 && inStatue)
                {
                    inStatue = false;
                    std::string statueObj = statuesJson.substr(statueStart, i - statueStart + 1);
                    Statue statue;
                    statue.locationId = s_locationIdCounter++;

                    ExtractStringField(statueObj, "\"name\"", statue.name);
                    ExtractStringField(statueObj, "\"objectName\"", statue.objectName);

                    s_statues.push_back(statue);
                }
            }
        }
    }

    static void ExtractStringField(const std::string& json, const std::string& fieldName, std::wstring& result)
    {
        size_t pos = json.find(fieldName);
        if (pos == std::string::npos)
        {
            return;
        }

        pos = json.find('\"', pos + fieldName.length());
        if (pos == std::string::npos)
        {
            return;
        }

        size_t endPos = json.find('\"', pos + 1);
        if (endPos == std::string::npos)
        {
            return;
        }

        std::string value = json.substr(pos + 1, endPos - pos - 1);
        result = std::wstring(value.begin(), value.end());
    }

    static size_t FindMatchingBracket(const std::string& str, size_t startPos)
    {
        int count = 1;
        for (size_t i = startPos + 1; i < str.length(); ++i)
        {
            if (str[i] == '[')
                count++;
            else if (str[i] == ']')
            {
                count--;
                if (count == 0)
                    return i;
            }
        }
        return std::string::npos;
    }
};

// Static member initialization
std::vector<LocationsJsonParser::Region> LocationsJsonParser::s_regions;
std::vector<LocationsJsonParser::Statue> LocationsJsonParser::s_statues;
uint32_t LocationsJsonParser::s_locationIdCounter = 0;
