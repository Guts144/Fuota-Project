#pragma once

#include <string>
#include <functional>

namespace cloud {

class IcloudConnector {
 public:
    virtual void Connect()                                                     = 0;
    virtual void Disconnect()                                                  = 0;
    virtual void Publish(const std::string& topic, const std::string& payload) = 0;
    virtual void Subscribe(const std::string& topic)                           = 0;

    // Add this method to allow setting a custom message callback
    virtual void setMessageCallback(std::function<void(const std::string&)> callback) = 0;

    virtual ~IcloudConnector() = default;
};

}  // namespace cloud
