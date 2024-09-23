#include <iostream>
#include <memory>
#include <fstream>
#include <csignal>
#include <pwd.h>
#include <fstream>
#include "connectivityStack.hpp"
#include "cloudConnector.hpp"
#include "IcloudConnector.hpp"
#include "constants.hpp"

namespace connectivityStack {
// Define the external flag variable
volatile std::sig_atomic_t g_stopFlag = 0;
std::string                userPath;

void ConnectivityStack::getUserPath() {
    const char* homeDir = getenv("HOME");
    if (homeDir == nullptr) {
        struct passwd* password = getpwuid(getuid());
        homeDir                 = password->pw_dir;
    }
    userPath = std::string(homeDir);
}

// Signal handler function
void ConnectivityStack::signalHandler(int signum) {
    g_stopFlag = 1;  // Set the flag to true when Ctrl+C is pressed
}

// Callback function to be called when a new MQTT message is received
void onMessageReceived(const std::string& message, imageDownloader::Downloader& p_downloader) {
    // Assuming the message contains the URL
    std::string url = message;
    if (!url.empty()) {
        p_downloader.downloadFileFromAzure(url, userPath);
    }
}

void ConnectivityStack::cloudConnection() {
    getUserPath();
    // Register the signal handler
    std::signal(SIGINT, signalHandler);

    // Initialize the downloader
    imageDownloader::Downloader p_downloader;

    // unique_ptr to cloudConnector instance
    std::unique_ptr<cloud::IcloudConnector> connector = std::make_unique<cloud::CloudConnector>(userPath);

    // Set up the callback for message reception
    connector->setMessageCallback([&p_downloader](const std::string& message) { onMessageReceived(message, p_downloader); });

    connector->Connect();
    connector->Subscribe("t/a");

    // Infinite loop that will keep the program running
    while (!g_stopFlag) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Cleanup on exit
    connector->Disconnect();
    std::cout << "Disconnected cleanly" << std::endl;
}

}  // namespace connectivityStack
