#include "spider.hpp"
#include <curl/curl.h>
#include <fstream>

// Callback function for writing received data to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Callback function for writing received data to a file
static size_t WriteFileCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* file = (std::ofstream*)userp;
    file->write((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string downloadPage(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 Spider/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to download " << url << ": " << curl_easy_strerror(res) << "\n";
            readBuffer.clear();
        }

        curl_easy_cleanup(curl);
    }

    return readBuffer;
}

void downloadImage(const std::string& imageUrl, const std::string& savePath) {
    CURL* curl;
    CURLcode res;

    std::string filename = getFileNameFromUrl(imageUrl);
    std::string filepath = savePath;

    // Ensure savePath ends with /
    if (filepath.back() != '/') {
        filepath += '/';
    }
    filepath += filename;

    // Create directory if it doesn't exist
    createDirectory(savePath);

    std::ofstream outFile(filepath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << "\n";
        return;
    }

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, imageUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 Spider/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to download image " << imageUrl << ": " << curl_easy_strerror(res) << "\n";
        } else {
            std::cout << "Downloaded: " << filename << "\n";
        }

        curl_easy_cleanup(curl);
    }

    outFile.close();
}
