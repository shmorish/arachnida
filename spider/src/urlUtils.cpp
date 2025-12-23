#include "spider.hpp"
#include <algorithm>
#include <cctype>
#include <regex>
#include <sys/stat.h>
#include <cerrno>

bool isImageUrl(const std::string& url) {
    std::string lowerUrl = url;
    std::transform(lowerUrl.begin(), lowerUrl.end(), lowerUrl.begin(), ::tolower);

    const std::vector<std::string> imageExtensions = {".jpg", ".jpeg", ".png", ".gif", ".bmp"};

    // Remove query parameters and fragments for extension checking
    size_t queryPos = lowerUrl.find('?');
    size_t fragPos = lowerUrl.find('#');
    size_t endPos = std::min(queryPos, fragPos);
    std::string urlWithoutParams = (endPos != std::string::npos) ? lowerUrl.substr(0, endPos) : lowerUrl;

    for (const auto& ext : imageExtensions) {
        if (urlWithoutParams.length() >= ext.length() &&
            urlWithoutParams.compare(urlWithoutParams.length() - ext.length(), ext.length(), ext) == 0) {
            return true;
        }
    }
    return false;
}

std::string getFileNameFromUrl(const std::string& url) {
    // Remove query parameters and fragments
    size_t queryPos = url.find('?');
    size_t fragPos = url.find('#');
    size_t endPos = std::min(queryPos, fragPos);
    std::string cleanUrl = (endPos != std::string::npos) ? url.substr(0, endPos) : url;

    // Extract filename from path
    size_t lastSlash = cleanUrl.find_last_of('/');
    std::string filename = (lastSlash != std::string::npos) ? cleanUrl.substr(lastSlash + 1) : cleanUrl;

    // If filename is empty, generate a default name
    if (filename.empty() || filename.find('.') == std::string::npos) {
        filename = "image_" + std::to_string(std::hash<std::string>{}(url)) + ".jpg";
    }

    return filename;
}

std::string normalizeUrl(const std::string& url, const std::string& baseUrl) {
    // If URL is already absolute, return as is
    if (url.find("http://") == 0 || url.find("https://") == 0) {
        return url;
    }

    // Parse base URL
    std::regex urlRegex(R"(^(https?://[^/]+)(/.*)?)");
    std::smatch match;

    if (!std::regex_search(baseUrl, match, urlRegex)) {
        return url; // Invalid base URL
    }

    std::string protocol = match[1].str();
    std::string basePath = match[2].str();

    // Handle absolute path (starts with /)
    if (url[0] == '/') {
        return protocol + url;
    }

    // Handle relative path
    // Remove filename from base path
    size_t lastSlash = basePath.find_last_of('/');
    std::string baseDir = (lastSlash != std::string::npos) ? basePath.substr(0, lastSlash + 1) : "/";

    return protocol + baseDir + url;
}

bool createDirectory(const std::string& path) {
    struct stat st;

    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }

    // Create directory recursively
    size_t pos = 0;
    while ((pos = path.find('/', pos + 1)) != std::string::npos) {
        std::string subdir = path.substr(0, pos);
        if (!subdir.empty() && stat(subdir.c_str(), &st) != 0) {
            if (mkdir(subdir.c_str(), 0755) != 0) {
                return false;
            }
        }
    }

    // Create final directory
    if (mkdir(path.c_str(), 0755) != 0 && errno != EEXIST) {
        return false;
    }

    return true;
}
