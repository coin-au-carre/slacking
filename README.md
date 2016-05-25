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

+ [api.test](https://api.slack.com/methods/api.test)
+ [chat.postMessage](https://api.slack.com/methods/chat.postMessage)
+ [users.list](https://api.slack.com/methods/users.list)

More convenient helpers functions and structures are coming...
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
