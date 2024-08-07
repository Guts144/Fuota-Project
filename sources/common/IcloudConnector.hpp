#pragma once

#include <string>

namespace cloud {

class IcloudConnector {
 public:
    virtual void Connect() = 0;
    virtual void Disconnect() = 0;
    virtual void Publish(const std::string& topic, const std::string& payload) = 0;
    virtual void Subscribe(const std::string& topic) = 0;
};

}  // namespace cloud