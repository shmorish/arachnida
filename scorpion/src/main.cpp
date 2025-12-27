#include "scorpion.hpp"
#include <iostream>
#include <vector>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " FILE1 [FILE2 ...]\n";
    std::cout << "\nDescription:\n";
    std::cout << "  Extracts and displays EXIF and other metadata from image files.\n";
    std::cout << "\nSupported formats:\n";
    auto extensions = getSupportedExtensions();
    for (const auto& ext : extensions) {
        std::cout << "  " << ext << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No image files provided\n\n";
        printUsage(argv[0]);
        return 1;
    }

    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i) {
        files.push_back(argv[i]);
    }

    bool hasErrors = false;

    for (const auto& file : files) {
        if (!isValidImageFile(file)) {
            std::cerr << "Error: Invalid or unsupported image file: " << file << "\n";
            hasErrors = true;
            continue;
        }

        try {
            displayImageMetadata(file);
        } catch (const std::exception& e) {
            std::cerr << "Error processing " << file << ": " << e.what() << "\n";
            hasErrors = true;
        }

        if (&file != &files.back()) {
            std::cout << "\n" << std::string(80, '-') << "\n\n";
        }
    }

    return hasErrors ? 1 : 0;
}
