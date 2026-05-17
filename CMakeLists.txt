cmake_minimum_required(VERSION 3.22)
project(BlueFireArchipelago)

set(GAME_MOD_DLL_DIR "D:/SteamLibrary/steamapps/common/Blue Fire/Blue Fire/Binaries/Win64/ue4ss/Mods/BlueFireArchipelagoMod/dlls/")

add_subdirectory(RE-UE4SS)
add_subdirectory(APCpp)
add_subdirectory(BlueFireArchipelagoMod)