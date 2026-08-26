#pragma once

#include <cstdint>

struct KeyTracker
{
	// Old Keys
	bool oldKeyFireKeep = false;
	bool oldKeyForestTempleAmbush = false;
	bool oldKeyForestTempleCenterRoom = false;
	bool oldKeyForestTempleAmbush2 = false;
	bool oldKeyUthasTempleMainRoom = false;
	bool oldKeyUthasTempleAmbush = false;
	bool oldKeyUthasTemple2ndSide = false;
	bool oldKeyUthasTempleFinalFloor = false;

	// Holy Keys
	bool holyKeyForestTempleBoss = false;
	bool holyKeyForestTempleNuosClaw = false;
	bool holyKeyUthasTempleHolyTower = false;

	// Other Keys
	bool uthasTempleKey = false;
	bool templeOfGodsKey = false;
	bool steamKey = false;
	bool keyOfEmber = false;
	bool graveyardKey = false;

	void updateKey(int keyID)
	{
		switch(keyID)
		{
			case 0: oldKeyFireKeep = true; break;
			case 1: oldKeyForestTempleAmbush = true; break;
			case 2: oldKeyForestTempleCenterRoom = true; break;
			case 3: oldKeyForestTempleAmbush2 = true; break;
			case 4: oldKeyUthasTempleMainRoom = true; break;
			case 5: oldKeyUthasTempleAmbush = true; break;
			case 6: oldKeyUthasTemple2ndSide = true; break;
			case 7: oldKeyUthasTempleFinalFloor = true; break;
			case 8: holyKeyForestTempleBoss = true; break;
			case 9: holyKeyForestTempleNuosClaw = true; break;
			case 10: holyKeyUthasTempleHolyTower = true; break;
			case 11: uthasTempleKey = true; break;
			case 12: templeOfGodsKey = true; break;
			case 13: steamKey = true; break;
			case 14: keyOfEmber = true; break;
			case 15: graveyardKey = true; break;
		}
	}

	void reset()
	{
		oldKeyFireKeep = false;
		oldKeyForestTempleAmbush = false;
		oldKeyForestTempleCenterRoom = false;
		oldKeyForestTempleAmbush2 = false;
		oldKeyUthasTempleMainRoom = false;
		oldKeyUthasTempleAmbush = false;
		oldKeyUthasTemple2ndSide = false;
		oldKeyUthasTempleFinalFloor = false;
		holyKeyForestTempleBoss = false;
		holyKeyForestTempleNuosClaw = false;
		holyKeyUthasTempleHolyTower = false;
		uthasTempleKey = false;
		templeOfGodsKey = false;
		steamKey = false;
		keyOfEmber = false;
		graveyardKey = false;
	}
};
