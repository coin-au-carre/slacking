#include "slacking.hpp"

#include <fstream>

int main() {

    auto& slack = slack::createInstance("xxxx-xxxx"); // where "xxx-xxx" is your Slack API token
    slack.chat_postMessage.channel = "#general";

    {
        slack::chat_postMessage("Hello there!"); // will send the message "Hello there!" in the channel #general
    }

    {
        // You can also use the generic post slack approach. Parameters (except the token) will not be taken into account.
        // Everything from the Web Slack API is possible here!
        slack::post (   
                        "chat.postMessage",
                        { 
                            {"text"      , "Slacking is awesome!"   }, 
                            {"channel"   , "#general"              }, 
                            {"username"  , "peach"                  }, 
                            {"icon_emoji", ":princess:"             } 
                        }
                    );
    }
}
