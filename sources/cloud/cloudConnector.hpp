#pragma once

#include <mqtt/async_client.h>
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

    private:
        void disableEcho();
        void enableEcho();
        inline std::string getCaCertPath();
        
        mqtt::async_client* client;
        mqtt::connect_options connOpts;
        const std::string SERVER_ADDRESS;
        const std::string CLIENT_ID;
    };
}  // namespace cloud 
