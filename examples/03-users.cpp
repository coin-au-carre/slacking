#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    // Create an Slacking instance. All parameters except the first are optional.
    auto& slack = slack::create(mytoken);
    
    auto users = slack.magic_users_list();
    std::cout << users << '\n';
}
