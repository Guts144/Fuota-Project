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

// Flag to indicate when to stop the loop
volatile std::sig_atomic_t l_stopFlag = 0;

// Signal handler function
void signalHandler(int signum) {
    l_stopFlag = 1;  // Set the flag to true when Ctrl+C is pressed
}

void connectivityStack::cloudConnection() {
    // Register the signal handler
    std::signal(SIGINT, signalHandler);

    imageDownloader::downloader             p_downloader;
    // Create a unique_ptr to a cloudConnector instance
    std::unique_ptr<cloud::IcloudConnector> connector = std::make_unique<cloud::cloudConnector>();

    // Use the connector to call the functions
    connector->Connect();
    connector->Subscribe("t/a");
    // connector->Publish("t/a", "Hello, MQTT!");

    // Read the URL from the file
    std::ifstream inFile("/home/vboxuser/Fuota-Project/resources/url.txt");
    std::string   url;
    if (inFile.is_open()) {
        std::getline(inFile, url);
        inFile.close();
    } else {
        std::cerr << "Unable to open file for reading" << std::endl;
    }

    if (!url.empty()) {
        p_downloader.download(url);
    }

    // Infinite loop that will keep the program running
    while (!l_stopFlag) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Cleanup on exit
    connector->Disconnect();
    std::cout << "Disconnected cleanly" << std::endl;
}

inline std::string connectivityStack::getUserPath() {
    const char* homeDir = getenv("HOME");
    if (homeDir == nullptr) {
        struct passwd* password = getpwuid(getuid());
        homeDir                 = password->pw_dir;
    }
    return std::string(homeDir);
}
}  // namespace connectivityStack
