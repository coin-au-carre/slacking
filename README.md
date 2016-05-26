Slacking  
========

[![Build Status](https://travis-ci.org/coin-au-carre/slacking.svg?branch=master)](https://travis-ci.org/coin-au-carre/slacking)

Modern C++ people also loves Slack !
------------------------------------

*Slacking* is a lightweight **C++11 header only library** for communicating with the [Slack Web API](https://api.slack.com/web).  
*Slacking* aims to be easy to use. *Slacking* requires to have an [API token](https://api.slack.com/docs/oauth-test-tokens).

Example usage
-------------

Initialize a slack instance:
```c++
auto& slack = slack::create("xxx-xxx-xxx-xxx"); // "xxx-xxx-xxx-xxx" is your Slack API token
slack.chat_postMessage.channel = "#general";    // set a default channel
```

Sending a message in Slack is easy:
```c++
slack::chat_postMessage("Hello there!"); // will send the message "Hello there!" in the channel #general with the registered token
```

If you need maximum control, you can use the generic functions `slack::post` or `slack::get`.  
Everything available in [Web Slack API](https://api.slack.com/methods) is possible from here.
```c++
slack::post (   
                "chat.postMessage",
                {
                    {"text"      , "Slacking is awesome!"   },
                    {"channel"   , "#mychannel"             },
                    {"username"  , "peach"                  },
                    {"icon_emoji", ":princess:"             }
                } // note that "token" is not needed here and is the only "registred" parameter
            ); // it is automatically inserted when using slack::post()
```

Check out the [examples](examples/) for more illustrations.  


A more elaborated example
-------------------------

You can make richly-formmated messages with [attachments](https://api.slack.com/docs/attachments).

```c++
slack::instance().chat_postMessage.channel_username_iconemoji("#mychannel", "Support Bot", ":hamster:");

auto json_attachments = R"([
    {
        "fallback": "New ticket from Bjarne Stroustrup - Ticket #2017: Still looking for reflection",
        "pretext": "New ticket from Bjarne Stroustrup",
        "title": "Ticket #2017: Still looking for reflection",
        "title_link": "https://www.youtube.com/watch?v=ND-TuW0KIgg",
        "text": "Help me adding reflection!",
        "color": "#7CD197",
        "image_url": "https://img.youtube.com/vi/ND-TuW0KIgg/2.jpg"
    }
])"_json;

slack::instance().chat_postMessage.attachments = json_attachments.dump();
std::cout << slack::chat_postMessage() << std::endl;
```

You will see in Slack the following: 
<!-- ![Slacking attachments](https://raw.githubusercontent.com/coin-au-carre/slacking/master/doc/showcase_attachments.png) -->
![Slacking attachments](doc/showcase_attachments.png?raw=true "Slacking attachments")

The output will give a JSON response sent back by Slack:
```
{"channel":"C1AUF9AN4","message":{"attachments":[{"color":"7CD197","fallback":"New ticket from Bjarne Stroustrup - Ticket #2017: Still looking for reflection","id":1,"image_bytes":4820,"image_height":90,"image_url":"https://img.youtube.com/vi/ND-TuW0KIgg/2.jpg","image_width":120,"pretext":"New ticket from Bjarne Stroustrup","text":"Help me adding reflection!","title":"Ticket #2017: Still looking for reflection","title_link":"https://www.youtube.com/watch?v=ND-TuW0KIgg"}],"bot_id":"B20LJ4Y12","icons":{"emoji":":hamster:","image_64":"https://slack.global.ssl.fastly.net/d4bf/img/emoji_2015_2/apple/1f439.png"},"subtype":"bot_message","text":" ","ts":"1464251666.000063","type":"message","username":"Support Bot"},"ok":true,"ts":"1464251666.000063"}
```

Requirements
------------

+ C++11 compatible compiler. Tested with Clang (3.5, 3.6, 3.7) and GCC (4.9, 5).
+ [Curl](https://curl.haxx.se/libcurl/) (which you probably already have).

Note: *Slacking* uses the awesome [C++ Requests](https://github.com/whoshuu/cpr) (CPR) and [Nlohmann Json](https://github.com/nlohmann/json) which are already included in the project.
CPR have been rewritten in order to be a header only library. Hence, you don't have to install anything!

Installation
------------

Just copy the `include/slacking` folder in your project. Add `#include "slacking.hpp"` in your project files. That's all.  


Ongoing work
------------

The library on `master` branch should always be functional.
You can use the `slack::post` or `slack::get` methods to fully exploit the [Slack Web API methods](https://api.slack.com/methods).  
Following C++ helpers free functions and members methods are available in *Slacking* for convenience :

+ [api.test](https://api.slack.com/methods/api.test) (see [examples/01-basic.cpp](examples/01-basic.cpp))
+ [chat.postMessage](https://api.slack.com/methods/chat.postMessage) (see [examples/00-showcase.cpp](examples/00-showcase.cpp), [examples/01-basic.cpp](examples/01-basic.cpp), [examples/02-basic.cpp](examples/02-basic.cpp), [examples/03-attachments.cpp](examples/04-attachments.cpp))
+ [users.list](https://api.slack.com/methods/users.list), [users.info](https://api.slack.com/methods/users.info) (see [examples/04-users.cpp](examples/03-users.cpp))
+ [channels.list](https://api.slack.com/methods/users.list), [channels.info](https://api.slack.com/methods/channels.info) (see [examples/05-channels.cpp](examples/05-channels.cpp))

Try out the "magic" functions for grabbing ready-to-use structures. 
This is an ongoing work so more convenient helpers functions and structures might come in the near future...  
If you need any features feel free to ask and contribute.


Manage Slacking instance
------------------------

Here are two approaches to keep alive the *Slacking* session in your program so you can use it anytime, anywhere. 

####Use Meyers singleton

*Slacking* provides free convenient functions : `slack::create(const std::string& token)` and `slack::instance()`.
Initialize the Slacking instance with:
```c++
auto slack& = slack::create("xxx-xxx-xxx-xxx");
```
And when you are in another scope and you have lost the `slack` reference, you can grab it again with :  
```c++
auto slack& = slack::instance();
```
It might not be the recommended way but since we generally want to handle only Slack instance (one handle), it is highly convenient. You can refer to the example usage and  [examples/01-basic.cpp](examples/01-basic.cpp).

####Pass by reference (or by pointer)

An other approach is to pass the *Slacking* instance by reference, store it, and call the appropriate methods when needed.

```c++
void foo(slack::Slacking& slack) {
    slack.chat_postMessage("I am slacking!", "#random");
}

int main() {
    slack::Slacking slack_instance{"xxx-xxx-xxx-xxx"};
    foo(slack_instance);
}
```

You can use a [std::reference_wrapper](http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper) as shown in [examples/02-basic.cpp](examples/02-basic.cpp). This strategy is useful if you have to manage several Slacking instances.

### Build the examples

```
mkdir build && cd build
cmake .. && make
examples/[whatever]
```

In your project, if you want a verbose output like when running the examples, add the following compilation flag:  
`-DSLACKING_VERBOSE_OUTPUT=1`.
