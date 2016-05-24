Slacking  
========

[![Build Status](https://travis-ci.org/coin-au-carre/slacking.svg?branch=master)](https://travis-ci.org/coin-au-carre/slacking)

Modern C++ people also uses Slack !
-----------------------------------

*Slacking* is a lightweight **C++11 header only library** for communicating with the [Slack Web API](https://api.slack.com/web).  
*Slacking* aims to be easy to use. 

Example usage
-------------

You can create a slack instance this way.
```c++
auto& slack = slack::create("xxx-xxx-xxx-xxx"); // where "xxx-xxx-xxx-xxx" is your Slack API token
slack.chat_postMessage.channel = "#general"; // set a default channel
```

Sending a message is easy
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

Check out the [examples](examples/) for more possibilities.  

Requirements
------------

+ C++11 compatible compiler. Tested with Clang (3.5, 3.6, 3.7) and GCC (4.9, 5).
+ [Curl](https://curl.haxx.se/libcurl/) (which you probably already have).

Note: *Slacking* uses the awesome [C++ Requests](https://github.com/whoshuu/cpr) (CPR) and [Nlohmann Json](https://github.com/nlohmann/json) which are already included in the project. 
CPR have been rewritten in order to be a header only library. Hence, you do not have to install anything! 

Installation
------------

Just copy the `include/slacking` header files in your project. That's all.  


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

There are several approaches to keep alive the *Slacking* session in your program so you can use it anytime, anywhere.

####Pass by reference the Slacking object (or by pointer)

The recommended approach is to pass the *Slacking* object by reference, store it, and call it when needed. 
You can store it via a [std::reference_wrapper](http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper) as shown in [examples/02-basic.cpp](examples/02-basic.cpp). Is is useful is you want to manage several Slacking instances. 


####Use Meyers singleton

*Slacking* provides free convenient functions : `slack::create(const std::string& token)` and `slack::instance()`.
Initialize Slacking with 
```c++
auto slack& = slack::create("xxx-xxx-xxx-xxx");
```
When you are in other scope and you don't have the `slack` reference anymore you can have it again with :  
```c++
auto slack& = slack::instance();
```
It might not be the recommended way but as you can see, since we generally handle only one instance, it is highly convenient (see [examples/01-basic.cpp](examples/01-basic.cpp)). This is what I personnally use. 


### Build the examples

```
mkdir build && cd build
cmake .. && make
examples/[whatever]
```

In your project, if you want a verbose output like when running the examples, add the following compilation flag: `-DSLACKING_VERBOSE_OUTPUT=1`.

