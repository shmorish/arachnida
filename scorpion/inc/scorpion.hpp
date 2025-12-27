#pragma once

#include <string>
#include <vector>

// Metadata extraction functions
void displayImageMetadata(const std::string& filePath);
bool isValidImageFile(const std::string& filePath);
std::vector<std::string> getSupportedExtensions();
