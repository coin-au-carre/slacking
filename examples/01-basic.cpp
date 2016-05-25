#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);
    {
        // Create an Slacking instance. The token must be fill and will be remembered. 
        slack::create(mytoken);
    }

    {
        // Calling API method is easy with some chosen free function 
        slack::api_test();
        // Or if you prefer you can take back the instance reference and use member function
        auto& slack = slack::instance();
        slack.api_test();
        // Helper member and free functions have the same signature
    }

    {
        // Get the instance and fill some permanent chat_postMessage parameters
        auto& slack = slack::instance();
        slack.chat_postMessage.channel = "#mychannel"; // required
        slack.chat_postMessage.username = "botname";   // optional
        slack.chat_postMessage("Hello there! Visit https://api.slack.com !");
    }

    {
        // We can also send via helper free function
        slack::chat_postMessage("Hello there again! Join <#C024BE7LR|general>!");
        // Note that Slacking remembers the chat_postMessage parameters
        // Here we just change the username permanently in the instance parameters
        slack::instance().chat_postMessage.username = "superbot";
        try {
            slack::chat_postMessage("Hello there in another chanel!", "#otherchannel");
        }
        catch(std::exception const& e) {
            std::cerr << "channel might not exist: " << e.what() << '\n';
        }
    }

    {
        auto users = slack::users_list();
        std::cout << users.dump(2) << '\n'; // will display users list information
    }

}

