#include "spider.hpp"
#include <regex>
#include <algorithm>

std::vector<std::string> extractImageUrls(const std::string& html, const std::string& baseUrl) {
    std::vector<std::string> imageUrls;
    std::set<std::string> uniqueUrls;

    // Regex patterns for different image sources
    // Match <img src="...">
    std::regex imgRegex(R"(<img[^>]+src=['"]([^'"]+)['"][^>]*>)", std::regex::icase);
    // Match <a href="..."> for direct image links
    std::regex linkRegex(R"(<a[^>]+href=['"]([^'"]+)['"][^>]*>)", std::regex::icase);

    // Extract from img tags
    auto imgBegin = std::sregex_iterator(html.begin(), html.end(), imgRegex);
    auto imgEnd = std::sregex_iterator();

    for (std::sregex_iterator i = imgBegin; i != imgEnd; ++i) {
        std::smatch match = *i;
        std::string url = match[1].str();
        std::string normalizedUrl = normalizeUrl(url, baseUrl);

        if (isImageUrl(normalizedUrl) && uniqueUrls.find(normalizedUrl) == uniqueUrls.end()) {
            imageUrls.push_back(normalizedUrl);
            uniqueUrls.insert(normalizedUrl);
        }
    }

    // Extract from anchor tags (for direct image links)
    auto linkBegin = std::sregex_iterator(html.begin(), html.end(), linkRegex);
    auto linkEnd = std::sregex_iterator();

    for (std::sregex_iterator i = linkBegin; i != linkEnd; ++i) {
        std::smatch match = *i;
        std::string url = match[1].str();
        std::string normalizedUrl = normalizeUrl(url, baseUrl);

        if (isImageUrl(normalizedUrl) && uniqueUrls.find(normalizedUrl) == uniqueUrls.end()) {
            imageUrls.push_back(normalizedUrl);
            uniqueUrls.insert(normalizedUrl);
        }
    }

    return imageUrls;
}

std::vector<std::string> extractPageLinks(const std::string& html, const std::string& baseUrl) {
    std::vector<std::string> pageLinks;
    std::set<std::string> uniqueUrls;

    // Match <a href="...">
    std::regex linkRegex(R"(<a[^>]+href=['"]([^'"]+)['"][^>]*>)", std::regex::icase);

    auto linkBegin = std::sregex_iterator(html.begin(), html.end(), linkRegex);
    auto linkEnd = std::sregex_iterator();

    // Extract base domain from baseUrl
    std::regex domainRegex(R"(^(https?://[^/]+))");
    std::smatch domainMatch;
    std::string baseDomain;

    if (std::regex_search(baseUrl, domainMatch, domainRegex)) {
        baseDomain = domainMatch[1].str();
    }

    for (std::sregex_iterator i = linkBegin; i != linkEnd; ++i) {
        std::smatch match = *i;
        std::string url = match[1].str();

        // Skip anchors, javascript, and mailto links
        if (url.empty() || url[0] == '#' ||
            url.find("javascript:") == 0 ||
            url.find("mailto:") == 0) {
            continue;
        }

        std::string normalizedUrl = normalizeUrl(url, baseUrl);

        // Only follow links on the same domain
        if (!baseDomain.empty() && normalizedUrl.find(baseDomain) == 0 &&
            !isImageUrl(normalizedUrl) &&
            uniqueUrls.find(normalizedUrl) == uniqueUrls.end()) {
            pageLinks.push_back(normalizedUrl);
            uniqueUrls.insert(normalizedUrl);
        }
    }

    return pageLinks;
}
