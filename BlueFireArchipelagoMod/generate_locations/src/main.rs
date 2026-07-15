use serde::{Deserialize, Serialize};
use std::fs;
use std::path::PathBuf;

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Location {
    name: String,
    #[serde(default)]
    objectName: String,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Chest {
    name: String,
    #[serde(default)]
    objectName: String,
    #[serde(default)]
    fullObjectName: String,
    #[serde(default)]
    dance: String,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Shop {
    name: String,
    items: u32,
    shopId: u32,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Event {
    name: String,
    #[serde(default)]
    requiredItems: Vec<String>,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct ManaUpgrade {
    number: u32,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Subregion {
    name: String,
    #[serde(default)]
    chests: Vec<Chest>,
    #[serde(default)]
    statues: Vec<Location>,
    #[serde(default)]
    pickups: Vec<Location>,
    #[serde(default)]
    void_gates: Vec<Location>,
    #[serde(default)]
    shops: Vec<Shop>,
    #[serde(default)]
    events: Vec<Event>,
    #[serde(default)]
    mana_upgrades: Vec<ManaUpgrade>,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Region {
    name: String,
    #[serde(default)]
    subregions: Vec<Subregion>,
}

#[derive(Debug, Serialize, Deserialize)]
struct LocationsJson {
    regions: Vec<Region>,
}

fn escape_string(s: &str) -> String {
    s.replace('\\', "\\\\")
        .replace('"', "\\\"")
        .replace('\n', "\\n")
        .replace('\r', "\\r")
        .replace('\t', "\\t")
}

fn generate_header(yaml_path: &PathBuf, output_path: &PathBuf) -> Result<(), Box<dyn std::error::Error>> {
    // Read the YAML file
    let yaml_content = fs::read_to_string(yaml_path)?;
    let data: LocationsJson = serde_yaml::from_str(&yaml_content)?;

    let mut header = String::new();
    header.push_str("#pragma once\n");
    header.push_str("\n");
    header.push_str("#include <map>\n");
    header.push_str("#include <string>\n");
    header.push_str("#include <cstdint>\n");
    header.push_str("\n");
    header.push_str("/**\n");
    header.push_str(" * LocationsData\n");
    header.push_str(" *\n");
    header.push_str(" * @brief Compiled-in location data for the Archipelago mod.\n");
    header.push_str(" *        Generated at build time from Locations.yaml.\n");
    header.push_str(" */\n");
    header.push_str("class LocationsData\n");
    header.push_str("{\n");
    header.push_str("public:\n");
    header.push_str("    /**\n");
    header.push_str("     * Get the chest name to location ID map\n");
    header.push_str("     * @return A map of chest names to location IDs\n");
    header.push_str("     */\n");
    header.push_str("    static const std::map<std::wstring, uint32_t>& GetChestNameToLocationIDMap()\n");
    header.push_str("    {\n");
    header.push_str("        return s_chestNameToLocationID;\n");
    header.push_str("    }\n");
    header.push_str("\n");
    header.push_str("    /**\n");
    header.push_str("     * Get the statue name to location ID map\n");
    header.push_str("     * @return A map of statue names to location IDs\n");
    header.push_str("     */\n");
    header.push_str("    static const std::map<std::wstring, uint32_t>& GetStatueNameToLocationIDMap()\n");
    header.push_str("    {\n");
    header.push_str("        return s_statueNameToLocationID;\n");
    header.push_str("    }\n");
    header.push_str("\n");
    header.push_str("    /**\n");
    header.push_str("     * Get the pickup name to location ID map\n");
    header.push_str("     * @return A map of pickup names to location IDs\n");
    header.push_str("     */\n");
    header.push_str("    static const std::map<std::wstring, uint32_t>& GetPickupNameToLocationIDMap()\n");
    header.push_str("    {\n");
    header.push_str("        return s_pickupNameToLocationID;\n");
    header.push_str("    }\n");
    header.push_str("\n");
    header.push_str("    /**\n");
    header.push_str("     * Get the void gate name to location ID map\n");
    header.push_str("     * @return A map of void gate names to location IDs\n");
    header.push_str("     */\n");
    header.push_str("    static const std::map<std::wstring, uint32_t>& GetVoidGateNameToLocationIDMap()\n");
    header.push_str("    {\n");
    header.push_str("        return s_voidGateNameToLocationID;\n");
    header.push_str("    }\n");
    header.push_str("\n");
    header.push_str("    /**\n");
    header.push_str("     * Get the shop IDs to location ID map\n");
    header.push_str("     * @return A map of shop IDs to location IDs\n");
    header.push_str("     */\n");
    header.push_str("    static const std::map<uint32_t, uint32_t>& GetShopIDToLocationIDMap()\n");
    header.push_str("    {\n");
    header.push_str("        return s_shopIDToLocationID;\n");
    header.push_str("    }\n");
    header.push_str("\n");
    header.push_str("    /**\n");
    header.push_str("     * Get the mana upgrades IDs to location ID map\n");
    header.push_str("     * @return A map of mana upgrade IDs to location IDs\n");
    header.push_str("     */\n");
    header.push_str("    static const std::map<uint32_t, uint32_t>& GetManaUpgradeIDToLocationIDMap()\n");
    header.push_str("    {\n");
    header.push_str("        return s_manaUpgradeIDToLocationID;\n");
    header.push_str("    }\n");
    header.push_str("\n");
    header.push_str("     * Get the chest full name to location ID map (for disambiguating duplicate objectNames)\n");
    header.push_str("     * @return A map of full chest names to location IDs\n");
    header.push_str("     */\n");
    header.push_str("    static const std::map<std::wstring, uint32_t>& GetChestFullNameToLocationIDMap()\n");
    header.push_str("    {\n");
    header.push_str("        return s_chestFullNameToLocationID;\n");
    header.push_str("    }\n");
    header.push_str("\n");


    header.push_str("public:\n");
    header.push_str("    // Shop location IDs (not in the map, but reserved in ID space)\n");
    // We'll add shop defines here after processing

    header.push_str("\nprivate:\n");

    let mut location_id: u32 = 0;
    let mut chest_entries = String::new();
    let mut chest_full_entries = String::new();
    let mut statue_entries = String::new();
    let mut pickup_entries = String::new();
    let mut void_gate_entries = String::new();
    let mut shop_entries = String::new();
    let mut mana_entries = String::new();
    let mut mana_upgrade_entries: u32 = 0;

    // Iterate: regions -> subregions -> location_types -> locations
    // Location types order: chests, statues, pickups, void_gates, shops
    // All locations get sequential IDs regardless of type
    for region in &data.regions {
        for subregion in &region.subregions {
            // Process chests
            for location in &subregion.chests {
                // Use fullObjectName if present, otherwise use objectName
                if !location.fullObjectName.is_empty() {
                    let escaped_name = escape_string(&location.fullObjectName);
                    chest_full_entries.push_str(&format!("        {{L\"{}\", {}}},\n", escaped_name, location_id));
                } else if !location.objectName.is_empty() {
                    let escaped_name = escape_string(&location.objectName);
                    chest_entries.push_str(&format!("        {{L\"{}\", {}}},\n", escaped_name, location_id));
                }
                location_id += 1;
            }

            // Process statues
            for location in &subregion.statues {
                if !location.objectName.is_empty() {
                    let escaped_name = escape_string(&location.objectName);
                    statue_entries.push_str(&format!("        {{L\"{}\", {}}},\n", escaped_name, location_id));
                }
                location_id += 1;
            }

            // Process pickups
            for location in &subregion.pickups {
                if !location.objectName.is_empty() {
                    let escaped_name = escape_string(&location.objectName);
                    pickup_entries.push_str(&format!("        {{L\"{}\", {}}},\n", escaped_name, location_id));
                }
                location_id += 1;
            }

            // Process void_gates
            for location in &subregion.void_gates {
                if !location.objectName.is_empty() {
                    let escaped_name = escape_string(&location.objectName);
                    void_gate_entries.push_str(&format!("        {{L\"{}\", {}}},\n", escaped_name, location_id));
                }
                location_id += 1;
            }

            // Process shops
            for location in &subregion.shops {
                shop_entries.push_str(&format!("        {{{}, {}}},\n", location.shopId, location_id));

                // Reserve IDs for all shop items
                for _ in 0..location.items {
                    location_id += 1;
                }
            }

            // Process mana upgrades
            for location in &subregion.mana_upgrades {
                for _ in 0..location.number {
                    mana_upgrade_entries += 1;
                    mana_entries.push_str(&format!("        {{{}, {}}},\n", mana_upgrade_entries, location_id));
                    location_id += 1;
                }
            }

            // Events are parsed but not exposed in the C++ header
            // They are used by the Python Archipelago mod parser only
        }
    }


    header.push_str("public:\n");
    header.push_str("    // Shop location IDs (not in the map, but reserved in ID space)\n");
    // We'll add shop defines here after processing

    header.push_str("\nprivate:\n");

    // Build all the maps
    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_chestNameToLocationID = {\n");
    header.push_str(&chest_entries);
    header.push_str("    };\n");
    header.push_str("\n");

    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_chestFullNameToLocationID = {\n");
    header.push_str(&chest_full_entries);
    header.push_str("    };\n");
    header.push_str("\n");

    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_statueNameToLocationID = {\n");
    header.push_str(&statue_entries);
    header.push_str("    };\n");
    header.push_str("\n");

    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_pickupNameToLocationID = {\n");
    header.push_str(&pickup_entries);
    header.push_str("    };\n");
    header.push_str("\n");

    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_voidGateNameToLocationID = {\n");
    header.push_str(&void_gate_entries);
    header.push_str("    };\n");

    header.push_str("    static inline const std::map<uint32_t, uint32_t> s_shopIDToLocationID = {\n");
    header.push_str(&shop_entries);
    header.push_str("    };\n");

    header.push_str("    static inline const std::map<uint32_t, uint32_t> s_manaUpgradeIDToLocationID = {\n");
    header.push_str(&mana_entries);
    header.push_str("    };\n");

    header.push_str("};\n");

    // Create output directory if it doesn't exist
    if let Some(parent) = output_path.parent() {
        fs::create_dir_all(parent)?;
    }

    // Write the header file
    fs::write(output_path, header)?;
    println!("Generated {}", output_path.display());

    Ok(())
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Navigate to the parent project directory (up from target/debug or target/release)
    // When running from CMake, the current directory should be the project root
    let project_dir = std::env::current_dir()?;

    let yaml_path = project_dir.join("Locations.yaml");
    let output_path = project_dir.join("Include").join("LocationsData.hpp");

    if !yaml_path.exists() {
        eprintln!("Error: {} not found", yaml_path.display());
        std::process::exit(1);
    }

    if let Err(e) = generate_header(&yaml_path, &output_path) {
        eprintln!("Error: {}", e);
        std::process::exit(1);
    }

    Ok(())
}
