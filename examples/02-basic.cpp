#include "slacking.hpp"

#include <fstream>
#include <functional>

// A class which handles a slack instance by reference
class Foo {
    std::reference_wrapper<slack::Slacking> slack_;
    // Or
    // Slacking& slack_;

public:
    Foo(slack::Slacking& slack) : slack_{slack} {
        slack_.get().chat.postMessage("Foo class ctor"); // parameters given in bar() should be kept
        // slack_.instance().post(...) method is also available as member function
    }
}; 

// Pass by ref in parameter function
void bar(slack::Slacking& slack) {
    // You can try catch API method if an error occur
    try {
        slack.chat.channel_username_iconemoji("#mychannel", "botname", ":bird:"); // for lazy people
        slack.chat.postMessage("bar() function is called");
    }
    catch(std::exception const& e) {
        std::cerr << e.what() << '\n';
    }
}


int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    // Create slack instance
    slack::Slacking slack_instance{mytoken};
    bar(slack_instance);
    Foo foo{slack_instance};

    // You can create other slack instances with different tokens and parameters
    {
        slack::Slacking another_slack_instance{"xxxx-xxxxxxxxx-xxxx"};
        try {
            another_slack_instance.chat.postMessage("this should throw since token is invalid", "#channelisnecessaryhere");
        }
        catch(std::exception const& e) {
            std::cerr << "02-basic failed purposely because of " << e.what() << '\n';
        }
    }
}

