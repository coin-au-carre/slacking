#include "slacking.hpp"

#include <fstream>

#include <iostream>

// Example 3
void more_elaborate_example() {
    // https://api.slack.com/messaging/composing/layouts#attachments
    auto &slack = slack::instance();
    slack.chat.channel_username_iconemoji("#general", "Support Bot", ":hamster:");

    auto json_attachments = R"([
        {
            "fallback": "New ticket from Bjarne Stroustrup - Ticket #2017: Still looking for reflection",
            "color": "#7CD197",
            "pretext": "New ticket from Bjarne Stroustrup",
            "title": "Ticket #2017: Still looking for reflection",
            "title_link": "https://www.youtube.com/watch?v=ND-TuW0KIgg",
            "text": "Help me adding reflection!",
            "image_url": "https://img.youtube.com/vi/ND-TuW0KIgg/2.jpg"
        }
    ])"_json;

    slack.chat.attachments = json_attachments; // equivalent to slack::instance().chat_postMessage.attachments = json_attachments.dump();
    auto result = slack.chat.postMessage();
}

int main() {
    std::cout << "Slacking starting" << std::endl;

    auto& slack = slack::create("xoxb-1843907906290-1856554650241-UBmRy7K4Yzumd2M1eYehUKaq"); // where "xxx-xxx" is your Slack API token

    slack.set_throw_exception(false); // feel free to disable exception (true by default)

    slack.chat.channel = "#general";

    slack.chat.postMessage("Hello there!"); // will send the message "Hello there!" in the channel #general

    // Example 1

    // You can also use the generic post slack approach. Parameters (except the token) will not be taken into account.
    // Everything from the Web Slack API is possible here!
    slack::post (   
        "chat.postMessage",
        { 
            {"text"      , "Slacking is awesome!" }, 
            {"channel"   , "#general"             }, 
            {"username"  , "peach"                }, 
            {"icon_emoji", ":princess:"           } 
        }
    );

    // Example 2
    // You can also use the generic post with a a Json approach. 
    auto json = R"({
        "text": "Slacking is simple!",
        "channel" : "#general",
        "username": "peach",
        "icon_emoji": ":princess:"
    })"_json;
    slack::post("chat.postMessage", json);


    // Example 3
    more_elaborate_example();

    std::cout << "Slacking example ended normally." << std::endl;
}
