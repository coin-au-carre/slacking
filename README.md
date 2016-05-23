Slacking
--------

### Modern C++ people also uses Slack !

*Slacking* is a C++11 header only library for communicating with the [Slack Web API](https://api.slack.com/web).  
*Slacking* aims to be easy to use. 

### Requirements

+ C++11 compatible compiler. Tested with Clang and GCC. 
+ [libcurl](https://curl.haxx.se/libcurl/) (used by [C++ Requests](https://github.com/whoshuu/cpr))

Note: *Slacking* uses [CPR C++ Requests](https://github.com/whoshuu/cpr) and [Nlohmann Json](https://github.com/nlohmann/json) which are already included in the project. 
C++ Requests have been rewritten in order to be a header only library. Hence, you do not have to install CPR nor Nlohmann json. 

### Installation

Just copy the `include/slacking` header files in your project. That's all.  
(If you already have C++ Requests and Nlohmann Json installed then you could even just copy `slacking.hpp`)

### Usage

You can create a slack instance this way.
```c++
slack::createInstance("mytoken", "#mychannel", "botname", ":bird:"); // all parameters are optional.
```

Calling Slack API method is easy
```c++
slack::chat_postMessage("Hello there!"); // will send the message "Hello there!" as user "botname" in the channel #mychannel with the avatar :bird:
```

If you need top flexibility, then you can use the generic functions `slack::post` or `slack::get`.  
Everything written in [there](https://api.slack.com/methods) is possible from here. 
```c++
slack::post (   
                "chat.postMessage",
                { 
                    {"text"      , "Slacking is awesome!"   }, 
                    {"channel"   , "#mychannel"             }, 
                    {"username"  , "peach"                  }, 
                    {"icon_emoji", ":princess:"             } 
                }
            );
```

If you want verbose output, add the following compilation flag: `-DSLACKING_VERBOSE_OUTPUT=1`.

### Ongoing work

The library on `master` branch should always be functional. 
You can use the `slack::post` or `slack::get` methods to fully exploit the [Slack Web API methods](https://api.slack.com/methods).  
Following C++ helpers free functions and members methods are available in *Slacking* for convenience : 

+ [api.test](https://api.slack.com/methods/api.test)
+ [chat.postMessage](https://api.slack.com/methods/chat.postMessage)
+ [users.list](https://api.slack.com/methods/users.list)


If you need any features feel free to ask or contribute.


### Manage Slacking instance

There are two approaches to keep alive the *Slacking* session in your program so you can use it anytime, anywhere.

_Pass by reference the Slacking object_

The recommended approach is to pass the *Slacking* object by reference, store it when needed and call the wanted methods. 
You can store it via a [std::reference_wrapper](http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper) as shown in `examples/02-basic.cpp`. 

_Use Meyers singleton_

You can also use the singleton pattern, *Slacking* provides free functions `createInstance(const std::string& token)` for initialization and `instance()` when you need to call it. It should not be the recommended way but it is highly convenient. 


### Dependencies (included in project)

*Slacking* uses C++11 Requests and Nlohmann Json which are included in `external` folder. 
Note: C++11 Requests is not a header only library.


### Next steps

Add more convenient helpers and structures. For instance, add a `magic_users_list()` method which will return a vector of users structure. 




