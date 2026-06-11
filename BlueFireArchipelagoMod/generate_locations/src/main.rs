use serde::{Deserialize, Serialize};
use std::fs;
use std::path::PathBuf;

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Location {
    name: String,
    #[serde(default)]
    chestName: String,
    #[serde(default)]
    objectName: String,
    #[serde(default)]
    dance: String,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Shop {
    name: String,
    items: u32,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Subregion {
    name: String,
    #[serde(default)]
    chests: Vec<Location>,
    #[serde(default)]
    statues: Vec<Location>,
    #[serde(default)]
    pickups: Vec<Location>,
    #[serde(default)]
    void_gates: Vec<Location>,
    #[serde(default)]
    shops: Vec<Shop>,
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

fn generate_header(json_path: &PathBuf, output_path: &PathBuf) -> Result<(), Box<dyn std::error::Error>> {
    // Read the JSON file
    let json_content = fs::read_to_string(json_path)?;
    let data: LocationsJson = serde_json::from_str(&json_content)?;

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
    header.push_str(" *        Generated at build time from locations.json.\n");
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

    header.push_str("public:\n");
    header.push_str("    // Shop location IDs (not in the map, but reserved in ID space)\n");
    // We'll add shop defines here after processing

    header.push_str("\nprivate:\n");

    let mut location_id: u32 = 0;
    let mut shop_defines = String::new();
    let mut chest_entries = String::new();
    let mut statue_entries = String::new();
    let mut pickup_entries = String::new();
    let mut void_gate_entries = String::new();

    // NEW STRUCTURE: Region-first hierarchy
    // Iterate: regions -> subregions -> location_types -> locations
    // Location types order: chests, statues, pickups, void_gates, shops
    // All locations get sequential IDs regardless of type
    for region in &data.regions {
        for subregion in &region.subregions {
            // Process chests
            for location in &subregion.chests {
                if !location.chestName.is_empty() {
                    let escaped_name = escape_string(&location.chestName);
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

            // Process shops - reserve IDs but don't add to map
            // Instead, create defines for other code to reference
            for shop in &subregion.shops {
                let shop_name_normalized = shop.name.to_uppercase().replace(" ", "_");
                let define_name = format!("{}_START", shop_name_normalized);
                shop_defines.push_str(&format!("    static constexpr uint32_t {} = {};\n", define_name, location_id));

                // Reserve IDs for all shop items
                for _ in 0..shop.items {
                    location_id += 1;
                }
            }
        }
    }

    // Build all the maps
    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_chestNameToLocationID = {\n");
    header.push_str(&chest_entries);
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

    header.push_str("};\n");

    // Insert shop defines before the closing brace
    if !shop_defines.is_empty() {
        // Find the last public: section and insert there
        if let Some(pos) = header.rfind("public:") {
            // Find the closing brace of this section and insert before the private section
            if let Some(private_pos) = header.rfind("\nprivate:") {
                header.insert_str(private_pos, &format!("\n{}", shop_defines));
            }
        }
    }

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

    let json_path = project_dir.join("locations.json");
    let output_path = project_dir.join("Include").join("LocationsData.hpp");

    if !json_path.exists() {
        eprintln!("Error: {} not found", json_path.display());
        std::process::exit(1);
    }

    if let Err(e) = generate_header(&json_path, &output_path) {
        eprintln!("Error: {}", e);
        std::process::exit(1);
    }

    Ok(())
}
