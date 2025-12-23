#pragma once

#include <string>
#include <unistd.h>
#include <iostream>
#include <set>
#include <vector>

struct SpiderConfig {
    bool recursive;
    int maxDepth;
    std::string savePath;
    std::string url;
};

void printUsage(const char* programName);
void printConfiguration(const SpiderConfig* config);
SpiderConfig parseArguments(int argc, char* argv[]);

// Spider core functions
void runSpider(const SpiderConfig& config);
std::string downloadPage(const std::string& url);
std::vector<std::string> extractImageUrls(const std::string& html, const std::string& baseUrl);
std::vector<std::string> extractPageLinks(const std::string& html, const std::string& baseUrl);
void downloadImage(const std::string& imageUrl, const std::string& savePath);
std::string normalizeUrl(const std::string& url, const std::string& baseUrl);
std::string getFileNameFromUrl(const std::string& url);
bool isImageUrl(const std::string& url);
bool createDirectory(const std::string& path);
