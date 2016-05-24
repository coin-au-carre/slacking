#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    // Create an Slacking instance. All parameters except the first are optional.
    auto& slack = slack::createInstance(mytoken);
    
    auto users = slack.users_list_magic();
    std::cout << users << '\n';
}
