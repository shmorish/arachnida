#include "spider.hpp"

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

void printConfiguration(const SpiderConfig* config) {
    bool recursive = config->recursive;
    int maxDepth = config->maxDepth;
    std::string savePath = config->savePath;
    std::string url = config->url;
    std::cout << "Spider Configuration:\n"
              << "  URL: " << url << "\n"
              << "  Recursive: " << (recursive ? "Yes" : "No") << "\n"
              << "  Max Depth: " << maxDepth << "\n"
              << "  Save Path: " << savePath << "\n";
}
