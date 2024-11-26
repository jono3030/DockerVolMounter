#ifndef PATH_VALIDATOR_H
#define PATH_VALIDATOR_H

#include <string>

class PathValidator {
public:
  // Method to normalize the path using filesystem
  static std::string normalizePath(const std::string &input);

  // Method to clean up path by removing trailing slashes
  static std::string removeTrailingSlashes(const std::string &input);

  // Method to ensure only one leading slash remains
  static std::string ensureSingleLeadingSlash(const std::string &input);

  // Method to check if the given path exists and handle errors
  static bool checkPathExists(const std::string &input);

  // Combined method to clean up paths
  static std::string cleanUpPath(const std::string &input);
};

#endif // PATH_VALIDATOR_H
