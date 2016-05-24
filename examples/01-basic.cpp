#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);
    {
        // Create an Slacking instance. The token is compulsory to fill and will be remembered. 
        slack::createInstance(mytoken);
    }

    {
        // Calling API method is easy with some chosen free function 
        slack::apiTest();
        // Or if you prefer you can take back the instance reference and use member function
        auto& slack = slack::instance();
        slack.apiTest();
        // Helper member and free functions have the same signature
    }

    {
        // Get the instance and fill some chat_postMessage parameters
        auto& slack = slack::instance();
        slack.chat_postMessage.channel = "#mychannel"; // required
        slack.chat_postMessage.username = "botname";   // optional
        slack.chat_postMessage("Hello there!");
    }

    {
        // We can also send via helper free function
        slack::chat_postMessage("Hello there again!");
        try {
            slack::chat_postMessage("Hello there in another chanel!", "#otherchannel");
        }
        catch(std::exception const& e) {
            std::cerr << "channel might not exist: " << e.what() << '\n';
        }
        // Note that Slacking remembers the chat_postMessage parameters
    }

    {
        // You can also use the generic post slack approach. Parameters (except the token) will not be taken into account.
        // Everything from the Web Slack API is possible here!
        slack::post (   
                        "chat.postMessage",
                        { 
                            {"text"      , "Slacking is awesome!"   }, 
                            {"channel"   , "#mychannel"              }, 
                            {"username"  , "peach"                  }, 
                            {"icon_emoji", ":princess:"             } 
                        }
                    );
    }

    {
        auto users = slack::users_list();
        std::cout << users.dump(2) << '\n'; // will display users list information
    }

}

