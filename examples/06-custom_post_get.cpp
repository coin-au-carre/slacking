#include "slacking.hpp"

#include <fstream>

int main() {
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    auto& slack = slack::create(mytoken);
    slack.chat.channel = "#testbot";

    // You can create the Json before hand
    auto json = R"({
        "text": "*bold* `code` _italic_ ~strike~",
        "channel" : "#testbot",
        "username": "peach",
        "mrkdwn": true,
        "icon_emoji": ":princess:"
    })"_json;

    slack::post("chat.postMessage", json);

    // Or you can call it directly
    slack::post (   
                "chat.postMessage",
                { 
                    {"text"      , "*bold* `code` _italic_ ~strike~" }, 
                    {"channel"   , "#testbot"             }, 
                    {"username"  , "peach"                }, 
                    {"icon_emoji", ":princess:"           } 
                }
        );
}

