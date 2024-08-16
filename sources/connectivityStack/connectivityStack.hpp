#pragma once

#include "downloader.hpp"
#include "IcloudConnector.hpp"
#include "IraucManager.hpp"
#include <csignal>  // Include for signal handling

namespace connectivityStack {
extern volatile sig_atomic_t l_stopFlag;  // Declare the flag as extern

class connectivityStack {
 public:
    connectivityStack()  = default;
    ~connectivityStack() = default;
    void                        cloudConnection();
    inline std::string          getUserPath();
    imageDownloader::downloader p_downloader;
};
}  // namespace connectivityStack
