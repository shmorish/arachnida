#include <iostream>

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " [OPTIONS] URL\n"
              << "Options:\n"
              << "  -r          Recursively download images by following links\n"
              << "  -l LEVEL    Maximum depth level for recursive download (default: 5)\n"
              << "  -p PATH     Path to save downloaded files (default: ./data/)\n"
              << "  -h          Display this help message\n"
              << "\nExample:\n"
              << "  " << programName << " -r -l 3 -p ./images https://example.com\n";
}

void printConfiguration(const std::string& url, bool recursive, int maxDepth, const std::string& savePath) {
    std::cout << "Spider Configuration:\n"
              << "  URL: " << url << "\n"
              << "  Recursive: " << (recursive ? "Yes" : "No") << "\n"
              << "  Max Depth: " << maxDepth << "\n"
              << "  Save Path: " << savePath << "\n";
}