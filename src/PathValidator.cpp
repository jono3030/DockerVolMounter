#include "PathValidator.h"
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <filesystem>

// Normalize path using filesystem
std::string PathValidator::normalizePath(const std::string &input) {
  std::filesystem::path path = input;
  return path.lexically_normal().string();
}

// Remove trailing slashes from the input path
std::string PathValidator::removeTrailingSlashes(const std::string &input) {
  std::string path = input;
  while (!path.empty() && path.back() == '/') {
    path.pop_back(); // Remove the last character if it is '/'
  }
  return path;
}

// Ensure only one leading slash remains
std::string PathValidator::ensureSingleLeadingSlash(const std::string &input) {
  std::string path = input;

  // Remove all but one leading '/'
  while (path.size() > 1 && path.front() == '/') {
    path.erase(0, 1);
  }

  // Ensure there is exactly one leading '/'
  if (path.empty() || path.front() != '/') {
    path.insert(path.begin(), '/'); // Add a leading '/' if it doesn't exist
  }

  return path;
}

// Check if the specified path exists
bool PathValidator::checkPathExists(const std::string &input) {
  std::filesystem::path path = input;
  if (!std::filesystem::exists(path)) {
    fl_alert("The specified path does not exist on the host:\n%s\nPlease "
             "enter the correct path and try again.",
             input.c_str());
    return false; // Return false if the path doesn't exist
  }
  return true; // Return true if the path exists
}

// Combined method to clean up paths (normalize, remove trailing slashes, ensure
// one leading slash)
std::string PathValidator::cleanUpPath(const std::string &input) {
  return ensureSingleLeadingSlash(removeTrailingSlashes(normalizePath(input)));
}
