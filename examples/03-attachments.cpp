#include "slacking.hpp"

#include <fstream>

void the_slacking_way() {
    slack::instance().chat_postMessage.channel_username_iconemoji("#testbot", "Support Bot", ":hamster:");

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
            "fallback": "Danger Danger! Bomb is launched!",
            "text": "Bomb is launched the slacking way!",
            "color": "danger"
        }
    ])"_json;

    slack::instance().chat_postMessage.attachments = json_attachments.dump();
    slack::chat_postMessage();
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
            "fallback": "Danger Danger! Bomb is launched!",
            "text": "Bomb is launched the hard way!",
            "color": "danger"
        }
    ])"};

    slack::post (   
                    "chat.postMessage",
                    { 
                        {"text"      , "" }, 
                        {"channel"   , "#testbot"            }, 
                        {"username"  , "Support Bot"         }, 
                        {"icon_emoji", ":hamster:"           },
                        {"attachments", attachments.c_str()  }
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

