#include "spider.hpp"

int main(int argc, char* argv[]) {
    SpiderConfig config = parseArguments(argc, argv);
    printConfiguration(&config);

    runSpider(config);

    return 0;
}
