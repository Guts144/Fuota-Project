#include "cloudConnector.hpp"
#include "constants.hpp"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

namespace cloud {

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
        : SERVER_ADDRESS(constant::SERVER_ADR),
          CLIENT_ID(constant::CLIENT_ID) {
        
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
        sslopts.set_trust_store(getCaCertPath()); // Path to the CA certificate file
        connOpts.set_ssl(sslopts);
    }

    cloudConnector::~cloudConnector() {
        delete client;
    }

    // Function to disable echoing of characters in terminal
    void cloudConnector::disableEcho() {
        termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    // Function to enable echoing of characters in terminal
    void cloudConnector::enableEcho() {
        termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    // Function to get the path to the CA certificate file
    inline std::string cloudConnector::getCaCertPath() {
        const char* homeDir = getenv("HOME");
        if (homeDir == nullptr) {
            struct passwd* password = getpwuid(getuid());
            homeDir = password->pw_dir;
        }
        return std::string(homeDir) + "/Fuota-Project/CAcert.pem";
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
