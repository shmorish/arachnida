#pragma once

#include <string>
#include <unistd.h>
#include <iostream>

struct SpiderConfig {
    bool recursive;
    int maxDepth;
    std::string savePath;
    std::string url;
};

void printUsage(const char* programName);
void printConfiguration(const std::string& url, bool recursive, int maxDepth, const std::string& savePath);
