#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <FL/Fl_Hold_Browser.H>
#include <string>
#include <yaml-cpp/yaml.h>

class ConfigManager {
public:
  // Constructor that takes reference to docker-compose.yml file
  ConfigManager(const std::string &filename);

  // Accessor for the loaded config. Returns a const reference to the loaded
  // configuration.
  const YAML::Node &getConfig() const;

  // Prints YAML::Node object
  void printConfig() const;

  // Loads the YAML configuration file
  bool loadConfig();

  // Import docker-compose.yml as YAML::Node
  void loadVolumesFromYAML(Fl_Hold_Browser *volumesListBox);

  // Populate volumesListBox from imported YAML::Node
  void mountVolumes(Fl_Hold_Browser *volumesListBox);

  // Save and close docker-compose.yml file
  void saveConfig();

private:
  std::string filename_;
  YAML::Node config_;
};

#endif // CONFIG_MANAGER_H
