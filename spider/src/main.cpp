#include <iostream>
#include <unistd.h>
#include <string>
#include <cstdlib>

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

int main(int argc, char* argv[]) {
    bool recursive = false;
    int maxDepth = 5;
    std::string savePath = "./data/";
    int opt;

    while ((opt = getopt(argc, argv, "rl:p:h")) != -1) {
        switch (opt) {
            case 'r':
                recursive = true;
                break;
            case 'l':
                maxDepth = std::atoi(optarg);
                if (maxDepth < 0) {
                    std::cerr << "Error: Level must be a non-negative integer\n";
                    return 1;
                }
                break;
            case 'p':
                savePath = optarg;
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            case '?':
                printUsage(argv[0]);
                return 1;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }

    if (optind >= argc) {
        std::cerr << "Error: URL is required\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string url = argv[optind];

    std::cout << "Spider Configuration:\n"
              << "  URL: " << url << "\n"
              << "  Recursive: " << (recursive ? "Yes" : "No") << "\n"
              << "  Max Depth: " << maxDepth << "\n"
              << "  Save Path: " << savePath << "\n";

    return 0;
}
