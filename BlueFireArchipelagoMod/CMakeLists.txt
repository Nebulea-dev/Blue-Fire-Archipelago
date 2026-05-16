set(TARGET BlueFireArchipelagoMod)

add_library(${TARGET} SHARED
    dllmain.cpp
    BlueFireArchipelagoMod.cpp
    HookManager.cpp
    ObjectCreateListener.cpp
)

target_include_directories(${TARGET} PRIVATE .)
target_link_libraries(${TARGET} PUBLIC UE4SS)

# Copy the DLL to the game's mod directory after building (optional, adjust path as needed)

add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "D:/SteamLibrary/steamapps/common/Blue Fire/Blue Fire/Binaries/Win64/ue4ss/Mods/${TARGET}/dlls/"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET}> "D:/SteamLibrary/steamapps/common/Blue Fire/Blue Fire/Binaries/Win64/ue4ss/Mods/${TARGET}/dlls/main.dll"
)
