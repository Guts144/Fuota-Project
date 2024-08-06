#include <iostream>
#include <memory>  // Include the memory header for smart pointers
#include "cloudConnector.hpp"  
#include "IcloudConnector.hpp" 

int main() {
    // Create a unique_ptr to a cloudConnector instance
    std::unique_ptr<cloud::IcloudConnector> connector = std::make_unique<cloud::cloudConnector>();

    // Use the connector to call the functions
    connector->Connect();
    connector->Subscribe("t/a");
    connector->Publish("t/a", "Hello, MQTT!");

    // Keep the main thread running to receive messages
    std::this_thread::sleep_for(std::chrono::seconds(40));
    connector->Disconnect();

    return 0;
}
