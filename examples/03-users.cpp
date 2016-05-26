#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    auto& slack = slack::create(mytoken);
    
    // You can display all the JSON response
    std::cout << slack.users_list() << std::endl; 

    // You can display filtered informations
    auto users = slack.magic_users_list(); // this is a vector so you can iterate
    std::cout << users << '\n';

    // Iterate on your users
    for (auto const& user : users) {
        std::cout << user.name << std::endl;
    }
}
