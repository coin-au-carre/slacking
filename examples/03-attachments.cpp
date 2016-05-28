#include "slacking.hpp"

#include <fstream>

void the_slacking_way() {
    auto &slack = slack::instance();
    slack.chat.channel_username_iconemoji("#testbot", "Support Bot", ":hamster:");

    auto json_attachments = R"([
        {
            "fallback": "New ticket from Bjarne Stroustrup - Ticket #2017: Still looking for reflection",
            "pretext": "New ticket from Bjarne Stroustrup",
            "title": "Ticket #2017: Still looking for reflection",
            "title_link": "https://www.youtube.com/watch?v=ND-TuW0KIgg",
            "text": "Help me adding reflection!",
            "color": "#7CD197",
            "image_url": "https://img.youtube.com/vi/ND-TuW0KIgg/2.jpg"
        },
        {
            "fallback": "Danger from the slacking way!",
            "text": "Danger from the slacking way!",
            "color": "danger"
        }
    ])"_json;

    slack.chat.attachments = json_attachments; 
    slack.chat.postMessage();
}


void the_hard_way() {
    auto attachments = std::string{R"([
        {
            "fallback": "New ticket from Bjarne Stroustrup - Ticket #2017: Still looking for reflection",
            "pretext": "New ticket from Bjarne Stroustrup",
            "title": "Ticket #2017: Still looking for reflection",
            "title_link": "https://www.youtube.com/watch?v=ND-TuW0KIgg",
            "text": "Help me adding reflection!",
            "color": "#7CD197", 
            "image_url": "https://img.youtube.com/vi/ND-TuW0KIgg/2.jpg"
        },
        {
            "fallback": "Danger from the hard way!",
            "text": "Danger from the hard way!",
            "color": "danger"
        }
    ])"};

    slack::post (   
                    "chat.postMessage",
                    { 
                        {"text"      , " " }, 
                        {"channel"   , "#testbot"            }, 
                        {"username"  , "Support Bot"         }, 
                        {"icon_emoji", ":hamster:"           },
                        {"attachments", attachments          }
                    }
                );
}



int main() {
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    slack::create(mytoken);

    the_slacking_way();
    the_hard_way();
}

