#include "connectivityStack.hpp"
#include "downloader.hpp"
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <pwd.h>
#include <unistd.h>
#include <csignal>  // Include for signal handling

namespace imageDownloader {

size_t downloader::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outFile   = static_cast<std::ofstream*>(userp);
    size_t         totalSize = size * nmemb;
    outFile->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

int downloader::ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal > 0) {
        double percent = (static_cast<double>(dlnow) / static_cast<double>(dltotal)) * 100.0;
        std::cout << "\rDownload progress: " << static_cast<int>(percent) << "%" << std::flush;
    }
    if (connectivityStack::g_stopFlag) {
        return 1;  // Return non-zero to indicate that the operation should be aborted
    }
    return 0;  // Return 0 to continue the download
}

bool downloader::downloadFileFromAzure(const std::string& url, const std::string g_userPath) {
    CURL*         curl;
    CURLcode      res;
    std::ofstream outFile(g_userPath + "/Fuota-Project/resources/imageUpdate.iso", std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing: " << g_userPath + "/Fuota-Project/resources/imageUpdate.iso" << std::endl;
        return false;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set options for libcurl
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Set the progress function
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, nullptr);  // Optional, can be used to pass user data to the callback
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);         // Enable progress reporting

        // Perform the file download
        res = curl_easy_perform(curl);
        while (connectivityStack::g_stopFlag && res == CURLE_OK) {
            // Abort the download if the stop flag is set
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            outFile.close();
            return false;
        }

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        // Check the content length
        double fileSize;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &fileSize);
        if (fileSize < 0) {
            std::cerr << "Failed to get file size" << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }
        std::cout << "File size: " << fileSize << " bytes" << std::endl;

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    outFile.close();

    if (connectivityStack::g_stopFlag) {
        std::cout << "Download was stopped by user" << std::endl;
        return false;
    }

    std::cout << "File downloaded successfully to " << g_userPath + "/Fuota-Project/resources/imageUpdate.iso" << std::endl;
    return true;
}
}  // namespace imageDownloader
