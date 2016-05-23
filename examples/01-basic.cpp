#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);
    {
        // Create an Slacking instance. All parameters except the first are optional.
        slack::createInstance(mytoken, "#mychannel", "botname", ":bird:");
    }

    {
        // Calling API method is easy with some chosen free function 
        slack::apiTest();
        // Or if you prefer you can take back the instance reference and use member function
        auto& slack_instance = slack::instance();
        slack_instance.apiTest();
        // Helper member and free functions have the same signature
    }

    {
        // Send a message with the parameters given in createInstance
        slack::chat_postMessage("Hello there!");
    }

    {
        // You can also use the generic post slack approach. Everything is possible here!
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

