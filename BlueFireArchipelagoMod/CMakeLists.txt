set(TARGET BlueFireArchipelagoMod)

# Ensure LocationsData.hpp is generated before building
# This script runs the Rust generator to create the header from Locations.yaml
set(LOCATIONS_DATA_HPP "${CMAKE_CURRENT_SOURCE_DIR}/Include/LocationsData.hpp")
set(LOCATIONS_YAML "${CMAKE_CURRENT_SOURCE_DIR}/Locations.yaml")
set(GENERATOR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/generate_locations/src/main.rs")
set(GENERATOR_CARGO "${CMAKE_CURRENT_SOURCE_DIR}/generate_locations/Cargo.toml")

# Find cargo (Rust build tool)
find_program(CARGO_EXECUTABLE cargo)
if(NOT CARGO_EXECUTABLE)
    message(WARNING "cargo not found. Please install Rust/Cargo. LocationsData.hpp must be pre-generated.")
else()
    # Custom command to generate LocationsData.hpp from Locations.yaml
    add_custom_command(
        OUTPUT "${LOCATIONS_DATA_HPP}"
        COMMAND "${CARGO_EXECUTABLE}" build --manifest-path "${GENERATOR_CARGO}" --release
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/generate_locations/target/release/generate_locations$<$<PLATFORM_ID:Windows>:.exe>"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        DEPENDS "${LOCATIONS_JSON}" "${GENERATOR_SRC}" "${GENERATOR_CARGO}"
        COMMENT "Generating LocationsData.hpp from Locations.yaml using Rust generator"
        VERBATIM
    )
endif()

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
    LocationManager.cpp
    "${LOCATIONS_DATA_HPP}"
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
