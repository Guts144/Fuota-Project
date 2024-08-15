#include "downloader.hpp"
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <regex>
#include <map>
#include <pwd.h>
#include <fstream>
#include <unistd.h>

namespace imageDownloader {

inline std::string downloader::getUserPath() {
    const char* homeDir = getenv("HOME");
    if (homeDir == nullptr) {
        struct passwd* password = getpwuid(getuid());
        homeDir                 = password->pw_dir;
    }
    return std::string(homeDir);
}

int downloader::download(const std::string& url) {
    std::string fileId = url.substr(url.find("/d/") + 3);
    fileId             = fileId.substr(0, fileId.find('/'));

    std::string outputFilePath = getUserPath() + "/Fuota-Project/resources/downloaded_file.iso";  // Change as needed
    if (downloadFileFromGoogleDrive(fileId, outputFilePath)) {
        std::cout << "Download completed successfully." << std::endl;
    } else {
        std::cerr << "Download failed." << std::endl;
    }

    return 0;
}

size_t downloader::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outFile   = static_cast<std::ofstream*>(userp);
    size_t         totalSize = size * nmemb;
    outFile->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

bool downloader::downloadFileFromGoogleDrive(const std::string& fileId, const std::string& outputFilePath) {
    std::cout << "sahha1" << std::endl;
    CURL*         curl;
    CURLcode      res;
    std::string   url = "https://drive.google.com/uc?export=download&id=" + fileId;
    std::ofstream outFile(outputFilePath, std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing: " << outputFilePath << std::endl;
        return false;
    }
    std::cout << "sahha2" << std::endl;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // Follow redirects if any

        // Perform the file download
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        curl_easy_cleanup(curl);
    }
    std::cout << "sahha3" << std::endl;
    curl_global_cleanup();
    outFile.close();

    std::cout << "File downloaded successfully to " << outputFilePath << std::endl;
    return true;
}
}  // namespace imageDownloader
