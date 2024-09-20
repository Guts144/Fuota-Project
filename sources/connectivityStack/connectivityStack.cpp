#include <iostream>
#include <memory>   // Include the memory header for smart pointers
#include <fstream>  // Include the file stream header for reading the URL file
#include <csignal>  // Include for signal handling
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

void connectivityStack::getUserPath() {
    const char* homeDir = getenv("HOME");
    if (homeDir == nullptr) {
        struct passwd* password = getpwuid(getuid());
        homeDir                 = password->pw_dir;
    }
    userPath = std::string(homeDir);
}

// Signal handler function
void connectivityStack::signalHandler(int signum) {
    g_stopFlag = 1;  // Set the flag to true when Ctrl+C is pressed
}

// Callback function to be called when a new MQTT message is received
void onMessageReceived(const std::string& message, imageDownloader::downloader& p_downloader) {
    // Assuming the message contains the URL
    std::string url = message;
    if (!url.empty()) {
        p_downloader.downloadFileFromAzure(url, userPath);
    }
}

void connectivityStack::cloudConnection() {
    getUserPath();
    // Register the signal handler
    std::signal(SIGINT, signalHandler);

    // Initialize the downloader
    imageDownloader::downloader p_downloader;

    // Create a unique_ptr to a cloudConnector instance
    std::unique_ptr<cloud::IcloudConnector> connector = std::make_unique<cloud::cloudConnector>(userPath);

    // Set up the callback for message reception
    connector->setMessageCallback([&p_downloader](const std::string& message) { onMessageReceived(message, p_downloader); });

    // Use the connector to call the functions
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
