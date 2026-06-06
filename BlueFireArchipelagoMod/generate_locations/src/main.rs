use serde::{Deserialize, Serialize};
use std::fs;
use std::path::PathBuf;

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Location {
    name: String,
    objectName: String,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Subregion {
    name: String,
    locations: Vec<Location>,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct Item {
    region: String,
    #[serde(default)]
    subregions: Vec<Subregion>,
    #[serde(default)]
    locations: Vec<Location>,
}

#[allow(non_snake_case)]
#[derive(Debug, Serialize, Deserialize)]
struct LocationsJson {
    chests: Vec<Item>,
    statues: Vec<Item>,
    pickups: Vec<Item>,
}

fn escape_string(s: &str) -> String {
    s.replace('\\', "\\\\")
        .replace('"', "\\\"")
        .replace('\n', "\\n")
        .replace('\r', "\\r")
        .replace('\t', "\\t")
}

fn add_locations(locations: &[Item], location_id: &mut u32, header: &mut String) {
    for region in locations {
        // Skip the "Menu" and "Victory" regions since they don't have any locations
        if region.region == "Menu" || region.region == "Victory" {
            continue;
        }

        // Add subregions
        for subregion in &region.subregions {
            for location in &subregion.locations {
                if !location.objectName.is_empty() {
                    let escaped_name = escape_string(&location.objectName);
                    header.push_str(&format!("        {{L\"{}\", {}}},\n", escaped_name, location_id));
                }
                *location_id += 1;
            }
        }

        // Add direct locations
        for location in &region.locations {
            if !location.objectName.is_empty() {
                let escaped_name = escape_string(&location.objectName);
                header.push_str(&format!("        {{L\"{}\", {}}},\n", escaped_name, location_id));
            }
            *location_id += 1;
        }
    }
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
    header.push_str("private:\n");

    let mut location_id: u32 = 0;


    // Add chest locations
    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_chestNameToLocationID = {\n");
    add_locations(&data.chests, &mut location_id, &mut header);
    header.push_str("    };\n\n");

    // Add statue locations
    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_statueNameToLocationID = {\n");
    add_locations(&data.statues, &mut location_id, &mut header);
    header.push_str("    };\n\n");

    // Add pickup locations
    header.push_str("    static inline const std::map<std::wstring, uint32_t> s_pickupNameToLocationID = {\n");
    add_locations(&data.pickups, &mut location_id, &mut header);
    header.push_str("    };\n");


    // Close the class declaration
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
