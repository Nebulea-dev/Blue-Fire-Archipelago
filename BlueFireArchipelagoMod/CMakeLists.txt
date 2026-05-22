set(TARGET BlueFireArchipelagoMod)

add_library(${TARGET} SHARED
    dllmain.cpp
    BlueFireArchipelagoMod.cpp
    Helper/HookHelper.cpp
    Helper/ObjectCreateListener.cpp
    Helper/ObjectFinder.cpp
    Helper/UnrealObjectQueries.cpp
    MainMenuManager.cpp
    ItemManager.cpp
    ArchipelagoManager.cpp
)

target_include_directories(${TARGET} PRIVATE . Include)
target_link_libraries(${TARGET} PUBLIC UE4SS)
target_link_libraries(${TARGET} PUBLIC APCpp)

# Copy the DLL to the game's mod directory after building
add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${GAME_MOD_DLL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${TARGET}>
        "${GAME_MOD_DLL_DIR}main.dll"
)
