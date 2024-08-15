#pragma once

#include <cstddef>
#include <string>

namespace imageDownloader {

class downloader {
 public:
    downloader()  = default;
    ~downloader() = default;
    int                download(const std::string& url);
    inline std::string getUserPath();

 private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    bool          downloadFileFromGoogleDrive(const std::string& fileId, const std::string& outputFilePath);
};

}  // namespace imageDownloader
