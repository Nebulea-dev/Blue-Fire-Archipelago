#include <LocationManager.hpp>
#include <ArchipelagoModConfig.hpp>
#include <LocationsJsonParser.hpp>
#include <BlueFireArchipelagoMod.hpp>
#include <Helper/UnrealObjectQueries.hpp>

#include <Archipelago.h>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

// Static maps loaded from JSON
static std::map<std::wstring, uint32_t> ChestNameToLocationID;
static std::map<std::wstring, uint32_t> StatueNameToLocationID;
static bool g_locationsLoaded = false;

LocationManager::LocationManager() {}

void LocationManager::Init(HookHelper* hookManager, ObjectCreateListener* objectListener) {
	// Load locations from JSON
	if (!g_locationsLoaded) {
		std::string jsonPath = "locations.json";  // Local copy in mod directory
		if (LocationsJsonParser::ParseLocationsJson(jsonPath)) {
			ChestNameToLocationID = LocationsJsonParser::GetChestNameToLocationIDMap();
			StatueNameToLocationID = LocationsJsonParser::GetStatueNameToLocationIDMap();
			g_locationsLoaded = true;
			Output::send<LogLevel::Verbose>(STR("Successfully loaded locations from JSON\n"));
		} else {
			Output::send<LogLevel::Warning>(STR("Failed to load locations from JSON, location mapping may be unavailable\n"));
		}
	}

	hookManager->registerPreHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Chest/Chest_Master.Chest_Master_C:Set Used Chest"), OnChestOpened);
	hookManager->registerPreHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Chest/Chest_Master.Chest_Master_C:PressButton"), OnPressButton);
	hookManager->registerPostHook(STR("Function /Game/BlueFire/InteractiveObjects/EmoteStatue/EmoteStatue_BP.EmoteStatue_BP_C:CustomEvent_5"), OnDialogueWithStatueEnded);

	// Listen to object creation for EditableTextBox controls
	// objectListener->registerObjectCallback(std::wstring(L"NewItem_C"), OnNewItemCreated);
}

bool LocationManager::OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL) {
	// Get the path of the chest being opened
	const std::wstring chestName = Context->GetNamePrivate().ToString();

	// Match the chest name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromChestName(chestName);
	if (!locationID.has_value())
	{
		logIncorrectMapping(chestName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Chest {} opened, marking location ID {} as checked in Archipelago\n"), chestName, locationID.value());
	AP_SendItem(locationID.value());


    return false;
}

std::optional<uint32_t> LocationManager::GetLocationIDFromChestName(const std::wstring& chestName) {
	auto it = ChestNameToLocationID.find(chestName);
	if (it != ChestNameToLocationID.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}


bool LocationManager::OnPressButton(UObject* Context, FFrame& Stack, void* RESULT_DECL) {
	// Get the Type parameter
	uint8_t* chestType = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Type");
	if(!chestType)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Type parameter of the opened chest\n"));
		return false;
	}

	// Get the Weapon parameter
	uint8_t* chestWeapon = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Weapon");
	if(!chestWeapon)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Weapon parameter of the opened chest\n"));
		return false;
	}

	// Set the chest content to the default sword
	*chestType = 1;
	*chestWeapon = 0;

    return false;
}

bool LocationManager::OnDialogueWithStatueEnded(UObject* Context, FFrame& Stack, void* RESULT_DECL) {

	// Get the emote of the statue
	uint8_t* emoteEnum = Context->GetValuePtrByPropertyNameInChain<uint8_t>(L"Emote");

	// Check the game inventory
	std::optional<UObject*> gameInstance = UnrealObjectQueries::FindGameInstance();
	if(!gameInstance.has_value())
	{
		Output::send<LogLevel::Error>(STR("Could not find the game instance object\n"));
		return false;
	}

	// Get the emote inventory
	TArray<uint8_t>* emoteInventory = gameInstance.value()->GetValuePtrByPropertyNameInChain<TArray<uint8_t>>(L"Emotes");
	if(!emoteInventory)
	{
        Output::send<LogLevel::Error>(STR("Could not find the Emotes parameter of the game instance\n"));
		return false;
	}

	// The dialogue has been cancelled and the emote was not purchased
	if(emoteInventory->Top() != *emoteEnum)
	{
		return false;
	}


	const std::wstring statueName = Context->GetNamePrivate().ToString();

	// Match the statue name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromStatueName(statueName);
	if (!locationID.has_value())
	{
		logIncorrectMapping(statueName);
		return false;
	}

	Output::send<LogLevel::Verbose>(STR("Emote Statue {} purchased, marking location ID {} as checked in Archipelago\n"), statueName, locationID.value());
	AP_SendItem(locationID.value());

	// TODO : remove item from inventory
	emoteInventory->Pop(true);


	return false;
}

std::optional<uint32_t> LocationManager::GetLocationIDFromStatueName(const std::wstring& statueName) {
	auto it = StatueNameToLocationID.find(statueName);
	if (it != StatueNameToLocationID.end())
	{
		return it->second + Archipelago::BF_BASE_ID;
	}
	else
	{
		return std::nullopt;
	}
}


void LocationManager::OnNewItemCreated(const UObjectBase* object, int32 index)
{
	Output::send<LogLevel::Verbose>(STR("Hi from OnNewItemCreated!\n"));
}

void LocationManager::logIncorrectMapping(const std::wstring locationName)
{
	Output::send<LogLevel::Error>(STR("Hi ! It looks like the location you just checked was not correctly registered in the location mapping in	ArchipelagoModConfig.hpp. This mod is still very new, and some chests or other locations might be missing. Please open an issue on Github or contact nebulea__ on discord to get this added. Be sure to include a description of where you found this location, and include the following location name in your message : {}\n"), locationName);
}