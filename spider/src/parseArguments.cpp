#include "spider.hpp"
#include <cstdlib>

SpiderConfig parseArguments(int argc, char* argv[]) {
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
                char *endptr;
                maxDepth = std::strtol(optarg, &endptr, 10);
                if (maxDepth < 0) {
                    std::cerr << "Error: Level must be a non-negative integer\n";
                    printUsage(argv[0]);
                    std::exit(1);
                } else if (*endptr != '\0') {
                    std::cerr << "Error: Invalid level value\n";
                    printUsage(argv[0]);
                    std::exit(1);
                }
                break;
            case 'p':
                savePath = optarg;
                break;
            case 'h':
                printUsage(argv[0]);
                std::exit(0);
            case '?':
                printUsage(argv[0]);
                std::exit(1);
            default:
                printUsage(argv[0]);
                std::exit(1);
        }
    }

    if (optind >= argc) {
        std::cerr << "Error: URL is required\n";
        printUsage(argv[0]);
        std::exit(1);
    }

    std::string url = argv[optind];

    return SpiderConfig{recursive, maxDepth, savePath, url};
}
