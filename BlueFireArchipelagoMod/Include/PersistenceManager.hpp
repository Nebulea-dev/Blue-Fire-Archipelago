#pragma once

#include <string>
#include <vector>
#include <json/json.h>

/*******************************************************************************
 * PersistenceManager
 *
 * @brief   Unified persistence layer for save-specific archipelago state.
 *
 *          Consolidates JSON file I/O for both received items and checked
 *          locations into a single unified file per save. Provides a flexible
 *          architecture for future extensions (doors opened, spirit slots, etc.).
 *
 *          JSON schema:
 *          {
 *            "receivedItems": { "items": [...], "lastProcessedIndex": 0 },
 *            "checkedLocations": { "locations": [...], "lastSentIndex": 0 },
 *            "doorsOpened": { "doors": [...] },
 *            "spiritSlotsPurchased": 0,
 *            ...
 *          }
 */
class PersistenceManager
{
	public:
	/*******************************************************************************
	 * @fn      getSaveDataFilePath
	 *
	 * @brief   Gets the unified save data file path for the current save index.
	 *
	 *          File is named: save_data_<SaveIndex>.json
	 *
	 * @return  String path to save_data_<SaveIndex>.json in the DLL directory
	 */
	static std::string getSaveDataFilePath();

	/*******************************************************************************
	 * @fn      saveDataFileExists
	 *
	 * @brief   Checks if the unified save data file exists.
	 *
	 * @return  true if save_data_<SaveIndex>.json exists, false otherwise
	 */
	static bool saveDataFileExists();


	/*******************************************************************************
	 * @fn      readSaveData
	 *
	 * @brief   Reads the entire unified save data JSON from disk.
	 *
	 *          Creates empty structure if file doesn't exist:
	 *          { "receivedItems": {...}, "checkedLocations": {...} }
	 *
	 * @return  Json::Value containing the full save data structure
	 */
	static Json::Value readSaveData();

	/*******************************************************************************
	 * @fn      writeSaveData
	 *
	 * @brief   Writes the entire unified save data JSON to disk.
	 *
	 * @param   data - Json::Value containing full save data structure
	 *
	 * @return  true on success, false on write failure
	 */
	static bool writeSaveData(const Json::Value& data);

	/*******************************************************************************
	 * @fn      deleteSaveData
	 *
	 * @brief   Deletes the unified save data file for current save.
	 *
	 * @return  none
	 */
	static void deleteSaveData();

	// ============================================================================
	// Received Items API
	// ============================================================================

	/*******************************************************************************
	 * @fn      appendReceivedItem
	 *
	 * @brief   Appends an item ID to the received items queue.
	 *
	 * @param   itemID - The Archipelago item ID to queue
	 *
	 * @return  none
	 */
	static void appendReceivedItem(int itemID);

	/*******************************************************************************
	 * @fn      flushReceivedItems
	 *
	 * @brief   Returns all queued received items and clears them from storage.
	 *
	 * @return  Vector of item IDs that were queued
	 */
	static std::vector<int> flushReceivedItems();

	// ============================================================================
	// Checked Locations API
	// ============================================================================

	/*******************************************************************************
	 * @fn      appendCheckedLocation
	 *
	 * @brief   Appends a checked location to the queue.
	 *
	 *          If authenticated and in-game, sends immediately. Otherwise queues
	 *          to persistent storage.
	 *
	 * @param   locationID - The location ID to queue/send
	 *
	 * @return  none
	 */
	static void appendCheckedLocation(int64_t locationID);

	/*******************************************************************************
	 * @fn      flushUnsentCheckedLocations
	 *
	 * @brief   Sends all unsent checked locations to the Archipelago server.
	 *
	 * @return  Number of locations that were sent
	 */
	static int flushUnsentCheckedLocations();

	private:
	/*******************************************************************************
	 * @fn      getModuleDirectory
	 *
	 * @brief   Gets the directory where the mod DLL is located.
	 *
	 * @return  String path to mod directory, or empty string on failure
	 */
	static std::string getModuleDirectory();
};
