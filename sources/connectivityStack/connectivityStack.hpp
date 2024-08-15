#pragma once

#include "downloader.hpp"
#include "IcloudConnector.hpp"
#include "IraucManager.hpp"

namespace connectivityStack {

class connectivityStack {
 public:
    connectivityStack()  = default;
    ~connectivityStack() = default;
    void                        cloudConnection();
    inline std::string          getUserPath();
    imageDownloader::downloader p_downloader;
};

}  // namespace connectivityStack
