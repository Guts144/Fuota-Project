#include "cloudConnector.hpp"
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace cloud {

    // Function to disable echoing of characters in terminal
    void disableEcho() {
        termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    // Function to enable echoing of characters in terminal
    void enableEcho() {
        termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    class callback : public virtual mqtt::callback {
    public:
        void connection_lost(const std::string& cause) override {
            std::cout << "\nConnection lost";
            if (!cause.empty())
                std::cout << ". Cause: " << cause << std::endl;
        }

        void message_arrived(mqtt::const_message_ptr msg) override {
            std::cout << "\nMessage arrived" << std::endl;
            std::cout << "Topic: " << msg->get_topic() << std::endl;
            std::cout << "Payload: " << msg->to_string() << std::endl;
        }

        void delivery_complete(mqtt::delivery_token_ptr token) override {
            std::cout << "\nDelivery complete for token: "
                      << (token ? token->get_message_id() : -1) << std::endl;
        }
    };

    cloudConnector::cloudConnector()
        : SERVER_ADDRESS("ssl://80023f4bb4494e2ab22556f738a67541.s1.eu.hivemq.cloud:8883"),
          CLIENT_ID("80023f4bb4494e2ab22556f738a67541") {
        
        std::string username;
        std::string password;
        // Prompt the user for the username and password
        std::cout << "Enter username: ";
        std::cin >> username;
        std::cout << "Enter password: ";
        disableEcho();
        std::cin >> password;
        enableEcho();
        client = new mqtt::async_client(SERVER_ADDRESS, CLIENT_ID);
        connOpts.set_clean_session(true);
        connOpts.set_user_name(username);
        connOpts.set_password(password);
        connOpts.set_connect_timeout(60);
        // Set SSL options
        mqtt::ssl_options sslopts;
        sslopts.set_trust_store("/home/vboxuser/FUOTA-project/isrgrootx1.pem"); // Path to the CA certificate file
        connOpts.set_ssl(sslopts);
    }

    cloudConnector::~cloudConnector() {
        delete client;
    }

    void cloudConnector::Connect() {
        static callback cb;
        client->set_callback(cb);

        try {
            std::cout << "Connecting to the MQTT server..." << std::endl;
            mqtt::token_ptr conntok = client->connect(connOpts);
            conntok->wait();
            std::cout << "Connected!" << std::endl;
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error connecting to the MQTT server: " << exc.what() << std::endl;
        }
    }

    void cloudConnector::Disconnect() {
        try {
            std::cout << "Disconnecting from the MQTT server..." << std::endl;
            client->disconnect()->wait();
            std::cout << "Disconnected!" << std::endl;
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error disconnecting from the MQTT server: " << exc.what() << std::endl;
        }
    }

    void cloudConnector::Publish(const std::string& topic, const std::string& payload) {
        try {
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            pubmsg->set_qos(1);
            client->publish(pubmsg)->wait_for(30000);
            std::cout << "Message published!" << std::endl;
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error publishing message: " << exc.what() << std::endl;
        }
    }

    void cloudConnector::Subscribe(const std::string& topic) {
        try {
            std::cout << "Subscribing to topic '" << topic << "'..." << std::endl;
            client->subscribe(topic, 1)->wait();
            std::cout << "Subscribed to topic '" << topic << "' with QoS 1" << std::endl;
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error subscribing to topic: " << exc.what() << std::endl;
        }
    }
}
