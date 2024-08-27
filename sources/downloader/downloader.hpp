#pragma once

#include <curl/curl.h>
#include <cstddef>
#include <string>

namespace imageDownloader {
class downloader {
 public:
    downloader()  = default;
    ~downloader() = default;
    bool downloadFileFromAzure(const std::string& url, const std::string g_userPath);

 private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static int    ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
};
}  // namespace imageDownloader
