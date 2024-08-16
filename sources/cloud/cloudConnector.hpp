#pragma once

#include <mqtt/async_client.h>
#include <functional>
#include "IcloudConnector.hpp"

namespace cloud {
class cloudConnector : public IcloudConnector {
 public:
    cloudConnector();
    ~cloudConnector();

    void Connect() override;
    void Disconnect() override;
    void Publish(const std::string& topic, const std::string& payload) override;
    void Subscribe(const std::string& topic) override;
    void setMessageCallback(std::function<void(const std::string&)> callback) override;  // New method

    static std::string getUserPath();

 private:
    void disableEcho();
    void enableEcho();

    mqtt::async_client*                     client;
    mqtt::connect_options                   connOpts;
    std::function<void(const std::string&)> messageCallback;  // Store the callback

    const std::string SERVER_ADDRESS;
    const std::string CLIENT_ID;
};
}  // namespace cloud
