#include "../include/ConfigManager.h"
#include <FL/fl_ask.H>
#include <filesystem>
#include <fstream>
#include <iostream>

ConfigManager::ConfigManager(const std::string &filename)
    : filename_(filename), config_() {}

const YAML::Node &ConfigManager::getConfig() const { return config_; }

void ConfigManager::printConfig() const {
  YAML::Emitter out;
  out << config_;
  std::cout << out.c_str() << '\n';
}

bool ConfigManager::loadConfig() {
  // Check if the file exists and is a regular file
  if (!std::filesystem::exists(filename_) ||
      !std::filesystem::is_regular_file(filename_)) {
    fl_alert("Warning: '%s' not found or is not a regular file. Exiting.",
             filename_.c_str());
    return false;
  }

  std::cout << "Loading file: " << filename_ << '\n';
  try {
    config_ =
        YAML::LoadFile(filename_); // Load the YAML file into the config object
    std::cout << "File loaded successfully!" << '\n';
    return true;
  } catch (const YAML::Exception &e) {
    fl_alert("Error parsing YAML file '%s': %s", filename_.c_str(), e.what());
    return false;
  }
}

void ConfigManager::loadVolumesFromYAML(Fl_Hold_Browser *volumesListBox) {
  const YAML::Node &services = config_["services"];
  if (services) {
    for (YAML::const_iterator it = services.begin(); it != services.end();
         ++it) {
      std::cout << "Found service: " << it->first.as<std::string>() << '\n';

      const YAML::Node &service = it->second;
      if (service["volumes"]) {
        const YAML::Node &volumes = service["volumes"];
        std::cout << "Found volumes section for service "
                  << it->first.as<std::string>() << "!\n";

        for (std::size_t i = 0; i < volumes.size(); ++i) {
          std::string volume = volumes[i].as<std::string>();
          std::cout << "Volume entry: " << volume << '\n';
          volumesListBox->add(volume.c_str());
        }
      }
    }
  } else {
    std::cout << "No 'services' section found in the YAML file!\n";
  }
}

void ConfigManager::mountVolumes(Fl_Hold_Browser *volumesListBox) {
  // Retrieve current volumes from the list box
  std::vector<std::string> currentVolumes;
  for (int i = 1; i <= volumesListBox->size(); ++i) {
    currentVolumes.push_back(volumesListBox->text(i));
  }

  // Access the 'services' section from the loaded YAML configuration
  YAML::Node services = config_["services"];
  if (!services) {
    std::cout << "No 'services' section found in the YAML configuration!\n";
    return;
  }

  // Iterate over each service and update its volumes
  for (YAML::iterator serviceIt = services.begin(); serviceIt != services.end();
       ++serviceIt) {
    YAML::Node &service = serviceIt->second;
    if (service["volumes"]) {
      YAML::Node volumes = service["volumes"];
      // Clear existing volumes
      service["volumes"] = YAML::Node(YAML::NodeType::Sequence);

      // Add only those volumes that are present in currentVolumes
      for (const std::string &volume : currentVolumes) {
        service["volumes"].push_back(volume);
      }
    }
  }

  // Save updated configuration back to the file
  saveConfig();
}

// Method to save the updated configuration back to the file
void ConfigManager::saveConfig() {
  try {
    YAML::Emitter out;
    out << config_;
    std::ofstream fout(filename_);
    fout << out.c_str();
    std::cout << "Updated YAML configuration written to " << filename_ << '\n';
  } catch (const std::exception &e) {
    std::cerr << "Error saving updated YAML file: " << e.what() << '\n';
  }
}
