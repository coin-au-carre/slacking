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
        slack_.get().chat_postMessage("Foo class ctor");
        // slack_.get().post(...) method is also available as member function
    }
}; 

// Pass by ref in parameter function
void bar(slack::Slacking& slack) {
    // 
    try {
        slack.chat_postMessage("bar() function is called"); // ? is not possible
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

    // Create an Slacking instance. All parameters except the first are optional.
    slack::Slacking slack_instance{mytoken, "#mychannel", "botname", ":bird:"};
    bar(slack_instance);
    Foo foo{slack_instance};
}

