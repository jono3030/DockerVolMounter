#include "ConfigManager.h"
#include "PathValidator.h"
#include <FL/Fl.H> // FLTK base lib
#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Window.H>
#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <yaml-cpp/yaml.h>

// Global configuration manager
std::unique_ptr<ConfigManager> configManager =
    std::make_unique<ConfigManager>("docker-compose.yml");

/*
 * FL_Window.H
 * Creates a window with a pointer '*' to window var loc in mem
 * Five args (first four mandatory): x,y,width,height,title
 */

// Main window
std::unique_ptr<Fl_Window> mainWindow =
    std::make_unique<Fl_Window>(10, 10, 700, 500, "Mount drives");

// Add volume section
std::unique_ptr<Fl_Button> selectHostLoc =
    std::make_unique<Fl_Button>(10, 10, 170, 30, "On host: ");
std::unique_ptr<Fl_Input> hostLocInput =
    std::make_unique<Fl_Input>(185, 10, 480, 30);
std::unique_ptr<Fl_Button> wipeHostLocInputButton =
    std::make_unique<Fl_Button>(670, 15, 20, 20, "X");

std::unique_ptr<Fl_Input> containerLocInput =
    std::make_unique<Fl_Input>(185, 45, 480, 30, "In container:            ");
std::unique_ptr<Fl_Button> wipeContainerLocInputButton =
    std::make_unique<Fl_Button>(670, 50, 20, 20, "X");

std::unique_ptr<Fl_Button> addVolumeButton =
    std::make_unique<Fl_Button>(10, 80, 680, 30, "Add volumes to list");

// Volume list
std::unique_ptr<Fl_Hold_Browser> volumesListBox =
    std::make_unique<Fl_Hold_Browser>(10, 115, 680, 300);

// Utility fns
std::unique_ptr<Fl_Button> editVolumeButton =
    std::make_unique<Fl_Button>(10, 420, 335, 30, "Edit volume");
std::unique_ptr<Fl_Button> delVolumeButton =
    std::make_unique<Fl_Button>(355, 420, 335, 30, "Delete volume");

// Mount and quit program
std::unique_ptr<Fl_Button> mountVolumeButton =
    std::make_unique<Fl_Button>(10, 455, 600, 30, "Mount volumes and exit");
// Mount and quit program
std::unique_ptr<Fl_Button> cancelButton =
    std::make_unique<Fl_Button>(620, 455, 70, 30, "Cancel");

// Instantiate class object
void loadVolumesFromYAML() {
  if (configManager->loadConfig()) {
    configManager->loadVolumesFromYAML(volumesListBox.get());
  }
}

// Instantiate class object
void mountVolumes(Fl_Widget *, void *) {
  configManager->mountVolumes(volumesListBox.get());
}

void chooseHostLocation(Fl_Widget *, void *) {
  int choice = fl_choice("What do type of volume do you want to mount?",
                         "Cancel", "Directory", "File");

  Fl_Native_File_Chooser chooserDialog; // Create dialog object
  if (choice == 1) {                    // Choose dir
    chooserDialog.title("Choose a Directory");
    chooserDialog.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
  } else if (choice == 2) { // Choose file
    chooserDialog.title("Choose a File");
    chooserDialog.type(Fl_Native_File_Chooser::BROWSE_FILE);
  } else {
    return; // Canceled
  }

  chooserDialog.options(Fl_Native_File_Chooser::NEW_FOLDER);
  chooserDialog.directory("."); // Start in cwd

  switch (chooserDialog.show()) { // Show chooser dialog and handle result
  case -1:                        // Error
    fl_alert("Error choosing location: %s", chooserDialog.errmsg());
    break;
  case 1: // Cancel
    break;
  default:                                       // Path selected
    std::string path = chooserDialog.filename(); // Get selected path
    hostLocInput->value(
        path.c_str()); // Set value of host loc field to selected path
    break;
  }
}

// Logic for adding file paths to list box
void addMounts(Fl_Widget *, void *) {
  // Get input values as std::string
  std::string hostStr = hostLocInput->value();
  std::string containerStr = containerLocInput->value();

  if (hostStr.empty() || containerStr.empty()) {
    fl_alert("Both host and container paths must be provided.");
    return;
  }

  // The path on the host system can be validated
  if (!PathValidator::checkPathExists(hostStr)) {
    return; // Exit if either path does not exist, keeping inputs intact
  }

  // Remove all trailing slashes using the helper function
  hostStr = PathValidator::cleanUpPath(hostStr);
  containerStr = PathValidator::cleanUpPath(containerStr);

  std::string combinedStr = hostStr + ":" + containerStr;
  volumesListBox->add(combinedStr.c_str());

  // Clear input fields after adding
  hostLocInput->value("");
  containerLocInput->value("");
}

// In case users want to edit volume paths that they have already added
void editMounts(Fl_Widget *, void *) {
  int selectedIdx = volumesListBox->value();
  // Inform user in case no volume is selected
  if (selectedIdx == 0) {
    fl_alert("Please select a volume to edit first.");
    return;
  }

  std::string selectedItem = volumesListBox->text(
      selectedIdx); // Get the text from selected volumes list entry
  auto delimiterCount = std::count(selectedItem.begin(), selectedItem.end(),
                                   ':'); // Make sure only one ':' exists
  if (delimiterCount != 1) {
    fl_alert("Error: Invalid volume format. There should be exactly one ':' "
             "delimiter. Please delete the entry and add the correct path.");
    return;
  }
  size_t delimiterPos =
      selectedItem.find(':'); // Find the ':' delimiter position
  // Check if delimiter was actually found
  // Should be guaranteed as this is checked in the validator when volumes
  // get added
  if (delimiterPos != std::string::npos) {
    std::string hostPath = selectedItem.substr(0, delimiterPos);
    std::string containerPath = selectedItem.substr(delimiterPos + 1);
    hostLocInput->value(hostPath.c_str());           // Put in input field
    containerLocInput->value(containerPath.c_str()); // Put in input field
    volumesListBox->remove(selectedIdx); // Remove to avoid duplicates
  }
}

// Delete mount
void delMount(Fl_Widget *, void *) {
  int selectedIdx = volumesListBox->value();
  if (selectedIdx == 0) {
    fl_alert("Please select a volume to delete first.");
    return;
  }
  volumesListBox->remove(selectedIdx);
  /*volumesListBox->remove(volumesListBox->value());*/
}

// Utility function to wipe input field content
void wipeInputField(Fl_Widget *, void *data) {
  Fl_Input *inputField =
      static_cast<Fl_Input *>(data); // Cast data back to Fl_Input*
  if (std::string_view(inputField->value()).empty()) {
    return;
  }
  inputField->value("");
}

void cancelGUI(Fl_Widget *, void *window) {
  static_cast<Fl_Window *>(window)->hide();
  exit(0);
}

int main() {
  /*Fl::scheme("gtk+");*/
  /*mainWindow->resizable(mainWindow);*/
  loadVolumesFromYAML(); // Load volumes from docker-compose.yml

  // Set tooltips -> this should be done more elegantly in a YAML file or
  // something
  wipeHostLocInputButton->copy_tooltip("Wipe content of host input field");
  wipeContainerLocInputButton->copy_tooltip(
      "Wipe content of container input field");
  selectHostLoc->copy_tooltip(
      "Select location of folder or file with a chooser dialog");
  delVolumeButton->copy_tooltip("Delete selected volume mounts");
  editVolumeButton->copy_tooltip("Edit selected volume mounts");
  addVolumeButton->copy_tooltip("Add volumes to list of mounted volumes");
  mountVolumeButton->copy_tooltip("Mount all volumes and exit GUI");
  cancelButton->copy_tooltip("Exit GUI without saving");

  selectHostLoc->callback(chooseHostLocation);
  wipeHostLocInputButton->callback(
      wipeInputField,
      hostLocInput.get()); // get() used here because std::unique_ptr manages
                           // its own memory and cannot implicitly converted to
                           // a raw pointer
  wipeContainerLocInputButton->callback(
      wipeInputField,
      containerLocInput.get()); // get() used here because std::unique_ptr
                                // manages its own memory and cannot implicitly
                                // converted to a raw pointer
  delVolumeButton->callback(delMount);
  editVolumeButton->callback(editMounts);
  addVolumeButton->callback(addMounts);
  mountVolumeButton->callback(mountVolumes); // Attach the mount callback
  cancelButton->callback(cancelGUI, mainWindow.get());

  mainWindow->end();  // Indicate that we are done adding widgets
  mainWindow->show(); // Pointer equivalent of "window.show();"
  return Fl::run();   // This is the FLTK equivalent of "return 0;"
}
