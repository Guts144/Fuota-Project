#pragma once

#include "downloader.hpp"
#include "IcloudConnector.hpp"
#include "IraucManager.hpp"
#include <csignal>  // Include for signal handling

namespace connectivityStack {
extern volatile sig_atomic_t g_stopFlag;

class ConnectivityStack {
 public:
    ConnectivityStack()  = default;
    ~ConnectivityStack() = default;
    void                        cloudConnection();
    imageDownloader::Downloader p_downloader;

 private:
    void        getUserPath();
    static void signalHandler(int signum);
};
}  // namespace connectivityStack
