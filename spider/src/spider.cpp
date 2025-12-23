#include "spider.hpp"
#include <curl/curl.h>
#include <set>

static void crawlPage(const std::string& url, const SpiderConfig& config,
                      std::set<std::string>& visitedUrls, int currentDepth) {
    // Check if we've already visited this URL
    if (visitedUrls.find(url) != visitedUrls.end()) {
        return;
    }

    // Check if we've exceeded max depth
    if (currentDepth > config.maxDepth) {
        return;
    }

    visitedUrls.insert(url);
    std::cout << "\nCrawling [depth " << currentDepth << "]: " << url << "\n";

    // Download the page
    std::string html = downloadPage(url);
    if (html.empty()) {
        std::cerr << "Failed to download page: " << url << "\n";
        return;
    }

    // Extract and download images
    std::vector<std::string> imageUrls = extractImageUrls(html, url);
    std::cout << "Found " << imageUrls.size() << " image(s) on this page\n";

    for (const auto& imageUrl : imageUrls) {
        downloadImage(imageUrl, config.savePath);
    }

    // If recursive mode is enabled, follow links
    if (config.recursive && currentDepth < config.maxDepth) {
        std::vector<std::string> pageLinks = extractPageLinks(html, url);
        std::cout << "Found " << pageLinks.size() << " link(s) on this page\n";

        for (const auto& link : pageLinks) {
            crawlPage(link, config, visitedUrls, currentDepth + 1);
        }
    }
}

void runSpider(const SpiderConfig& config) {
    std::set<std::string> visitedUrls;

    // Initialize curl globally
    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::cout << "\nStarting spider...\n";
    std::cout << "================================================\n";

    // Create save directory
    if (!createDirectory(config.savePath)) {
        std::cerr << "Error: Failed to create directory: " << config.savePath << "\n";
        curl_global_cleanup();
        return;
    }

    // Start crawling from the initial URL
    crawlPage(config.url, config, visitedUrls, 0);

    std::cout << "\n================================================\n";
    std::cout << "Spider finished!\n";
    std::cout << "Total pages visited: " << visitedUrls.size() << "\n";
    std::cout << "Images saved to: " << config.savePath << "\n";

    // Cleanup curl
    curl_global_cleanup();
}
