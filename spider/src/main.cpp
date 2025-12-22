#include "spider.hpp"

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
                char *endptr;
                maxDepth = std::strtol(optarg, &endptr, 10);
                if (maxDepth < 0) {
                    std::cerr << "Error: Level must be a non-negative integer\n";
                    return 1;
                } else if (*endptr != '\0') {
                    std::cerr << "Error: Invalid level value\n";
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

    SpiderConfig config{recursive, maxDepth, savePath, url};
    printConfiguration(config.url, config.recursive, config.maxDepth, config.savePath);

    return 0;
}
