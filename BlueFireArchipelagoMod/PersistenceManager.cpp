#include <PersistenceManager.hpp>
#include <Helper/UnrealObjectQueries.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <ArchipelagoManager.hpp>

#include <Archipelago.h>

#include <fstream>
#include <filesystem>
#include <windows.h>

using namespace RC;

std::string PersistenceManager::getModuleDirectory()
{
	wchar_t dllPath[MAX_PATH] = {0};
	HMODULE moduleHandle = GetModuleHandle(nullptr);
	if (!moduleHandle)
	{
		Output::send<LogLevel::Error>(STR("Could not get module handle for DLL path\n"));
		return "";
	}

	GetModuleFileNameW(moduleHandle, dllPath, MAX_PATH);
	std::filesystem::path modPath = std::filesystem::path(dllPath).parent_path();
	return modPath.string();
}

int32_t getSaveFileIndex()
{
	int32_t saveFileIndex = 0;
	std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
	if (gameInstance.has_value())
	{
		int32_t* saveIndex = UnrealObjectQueries::GetNestedPropertyValue<int32_t>(gameInstance.value(), L"System", L"SaveFileIndex_7_9ACF69B4474D76AACA0E349806254782");
		if (saveIndex)
		{
			saveFileIndex = *saveIndex;
		}
	}
	return saveFileIndex;
}

std::string PersistenceManager::getSaveDataFilePath()
{
	std::string modDir = getModuleDirectory();
	if (modDir.empty())
		return "";

	int32_t saveFileIndex = getSaveFileIndex();
	std::string filename = "save_data_" + std::to_string(saveFileIndex) + ".json";
	std::string filePath = (std::filesystem::path(modDir) / filename).string();

	Output::send<LogLevel::Verbose>(STR("Save data file path set for save index {}\n"), saveFileIndex);
	return filePath;
}

bool PersistenceManager::saveDataFileExists()
{
	std::string filePath = getSaveDataFilePath();
	if (filePath.empty())
		return false;

	return std::filesystem::exists(filePath);
}

Json::Value PersistenceManager::readSaveData()
{
	Json::Value root(Json::objectValue);
	std::string filePath = getSaveDataFilePath();

	if (filePath.empty())
		return root;

	try
	{
		if (!std::filesystem::exists(filePath))
		{
			// Initialize with empty sections
			root["receivedItems"] = Json::Value(Json::objectValue);
			root["receivedItems"]["items"] = Json::Value(Json::arrayValue);
			root["receivedItems"]["lastProcessedIndex"] = 0;

			root["checkedLocations"] = Json::Value(Json::objectValue);
			root["checkedLocations"]["locations"] = Json::Value(Json::arrayValue);
			root["checkedLocations"]["lastSentIndex"] = 0;

			root["doorsOpened"] = Json::Value(Json::arrayValue);
			root["spiritSlotsPurchased"] = 0;

			return root;
		}

		Json::Reader reader;
		std::ifstream inFile(filePath);
		if (!reader.parse(inFile, root))
		{
			Output::send<LogLevel::Error>(STR("Failed to parse save data JSON\n"));
			inFile.close();
			return Json::Value(Json::objectValue);
		}
		inFile.close();

		// Ensure all expected sections exist (for forward/backward compatibility)
		if (!root.isMember("receivedItems"))
		{
			root["receivedItems"] = Json::Value(Json::objectValue);
			root["receivedItems"]["items"] = Json::Value(Json::arrayValue);
			root["receivedItems"]["lastProcessedIndex"] = 0;
		}
		if (!root.isMember("checkedLocations"))
		{
			root["checkedLocations"] = Json::Value(Json::objectValue);
			root["checkedLocations"]["locations"] = Json::Value(Json::arrayValue);
			root["checkedLocations"]["lastSentIndex"] = 0;
		}
		if (!root.isMember("doorsOpened"))
		{
			root["doorsOpened"] = Json::Value(Json::arrayValue);
		}
		if (!root.isMember("spiritSlotsPurchased"))
		{
			root["spiritSlotsPurchased"] = 0;
		}
	}
	catch (const std::exception&)
	{
		Output::send<LogLevel::Error>(STR("Exception while reading save data\n"));
	}

	return root;
}

bool PersistenceManager::writeSaveData(const Json::Value& data)
{
	std::string filePath = getSaveDataFilePath();
	if (filePath.empty())
		return false;

	try
	{
		std::ofstream outFile(filePath);
		if (!outFile.is_open())
		{
			Output::send<LogLevel::Error>(STR("Could not open save data file for writing\n"));
			return false;
		}

		Json::FastWriter writer;
		outFile << writer.write(data);
		outFile.close();

		Output::send<LogLevel::Verbose>(STR("Saved persistence data\n"));
		return true;
	}
	catch (const std::exception&)
	{
		Output::send<LogLevel::Error>(STR("Exception while writing save data\n"));
		return false;
	}
}

void PersistenceManager::deleteSaveData()
{
	std::string filePath = getSaveDataFilePath();
	if (filePath.empty())
		return;

	try
	{
		if (std::filesystem::exists(filePath))
		{
			std::filesystem::remove(filePath);

			std::wstring wFilePath(filePath.begin(), filePath.end());
			Output::send<LogLevel::Verbose>(STR("Deleted {} file\n"), wFilePath);
		}
	}
	catch (const std::exception&)
	{
		Output::send<LogLevel::Error>(STR("Exception while deleting save data\n"));
	}
}

void PersistenceManager::appendReceivedItem(int itemID)
{
	try
	{
		Json::Value data = readSaveData();
		data["receivedItems"]["items"].append(itemID);

		if (writeSaveData(data))
		{
			Output::send<LogLevel::Verbose>(STR("Saved item {} to queue (total: {})\n"), itemID, data["receivedItems"]["items"].size());
		}
	}
	catch (const std::exception&)
	{
		Output::send<LogLevel::Verbose>(STR("Exception while saving item to queue\n"));
	}
}

std::vector<int> PersistenceManager::flushReceivedItems()
{
	std::vector<int> items;

	try
	{
		Json::Value data = readSaveData();

		const Json::Value& itemArray = data["receivedItems"]["items"];
		for (const auto& item : itemArray)
		{
			if (item.isInt())
			{
				items.push_back(item.asInt());
			}
		}

		// Clear the items array after flushing
		data["receivedItems"]["items"] = Json::Value(Json::arrayValue);
		writeSaveData(data);
		Output::send<LogLevel::Verbose>(STR("Loaded and cleared {} items from queue\n"), items.size());
	}
	catch (const std::exception&)
	{
		Output::send<LogLevel::Verbose>(STR("Exception while loading queue\n"));
	}

	return items;
}

void PersistenceManager::appendCheckedLocation(int64_t locationID)
{
	// Read current state from managers
	bool bIsAuthenticated = (AP_GetConnectionStatus() == AP_ConnectionStatus::Authenticated);
	bool bIsGameLoaded = BlueFireArchipelagoMod::arcManager ? BlueFireArchipelagoMod::arcManager->isGameLoaded() : false;

	// If already authenticated and in game, send immediately
	if (bIsAuthenticated && bIsGameLoaded)
	{
		AP_SendItem(locationID);
		Output::send<LogLevel::Verbose>(STR("Sent location {} immediately\n"), (int64_t)locationID);
		return;
	}
	else
	{
		Output::send<LogLevel::Error>(STR("Couldn't send item to server\n"));
		Output::send<LogLevel::Error>(STR("Value of bIsAuthenticated : {}\n"), bIsAuthenticated);
		Output::send<LogLevel::Error>(STR("Value of bIsGameLoaded : {}\n"), bIsGameLoaded);
		Output::send<LogLevel::Error>(STR("Value of isGameLoaded : {}\n"), BlueFireArchipelagoMod::arcManager->isGameLoaded());
		Output::send<LogLevel::Error>(STR("Value of BlueFireArchipelagoMod::arcManager : {}\n"), (void*)BlueFireArchipelagoMod::arcManager);
	}

	try
	{
		Json::Value data = readSaveData();
		data["checkedLocations"]["locations"].append((Json::Value::Int64)locationID);
		writeSaveData(data);

		Output::send<LogLevel::Verbose>(STR("Appended location {} to queue (total: {})\n"), (int64_t)locationID, data["checkedLocations"]["locations"].size());
	}
	catch (const std::exception&)
	{
		Output::send<LogLevel::Verbose>(STR("Exception while appending to checked location queue\n"));
	}
}

int PersistenceManager::flushUnsentCheckedLocations()
{
	int sentCount = 0;

	try
	{
		Json::Value data = readSaveData();

		const Json::Value& locationArray = data["checkedLocations"]["locations"];
		int lastSentIndex = data["checkedLocations"]["lastSentIndex"].asInt();

		for (int i = lastSentIndex; i < (int)locationArray.size(); ++i)
		{
			if (locationArray[i].isInt64())
			{
				int64_t locationID = locationArray[i].asInt64();
				AP_SendItem(locationID);
				sentCount++;
			}
		}

		if (sentCount > 0)
		{
			data["checkedLocations"]["lastSentIndex"] = lastSentIndex + sentCount;
			writeSaveData(data);
			Output::send<LogLevel::Verbose>(STR("Flushed {} queued locations\n"), sentCount);
		}
	}
	catch (const std::exception&)
	{
		Output::send<LogLevel::Verbose>(STR("Exception while flushing send queue\n"));
	}

	return sentCount;
}
