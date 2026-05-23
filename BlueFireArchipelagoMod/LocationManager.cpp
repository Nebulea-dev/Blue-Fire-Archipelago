#include <LocationManager.hpp>
#include <ArchipelagoModConfig.hpp>
#include <BlueFireArchipelagoMod.hpp>

#include <Archipelago.h>

using namespace RC;
using namespace Unreal;
using namespace ArchipelagoModConfig;

LocationManager::LocationManager() {}

void LocationManager::Init(HookHelper* hookManager) {
	hookManager->registerPreHook(STR("Function /Game/BlueFire/InteractiveObjects/Collectibles/Chest/Chest_Master.Chest_Master_C:Set Used Chest"), OnChestOpened);
}

bool LocationManager::OnChestOpened(UObject* Context, FFrame& Stack, void* RESULT_DECL) {
	// Get the Used_Chest parameter
	std::optional<bool> Used_Chest = HookHelper::readParamValue<bool>(STR("Used_Chest"), Stack);
    if (!Used_Chest.has_value())
    {
        Output::send<LogLevel::Error>(STR("Could not find the Used_Chest parameter in Set Used Chest function call\n"));
    }

	// Get the path of the chest being opened
	const std::wstring chestName = Context->GetNamePrivate().ToString();

	// Match the chest name to a location ID and mark it as checked
	std::optional<uint32_t> locationID = BlueFireArchipelagoMod::locationManager->GetLocationIDFromChestName(chestName);
	if (!locationID.has_value())
	{
		Output::send<LogLevel::Error>(STR("Chest name {} not found in ChestNameToLocationID mapping\n"), chestName);
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