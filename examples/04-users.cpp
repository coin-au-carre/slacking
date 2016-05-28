#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    auto& slack = slack::create(mytoken);
    
    // You can display all the JSON response
    std::cout << slack.users.list().dump(4) << std::endl; 

    // You can display filtered informations
    auto users = slack.users.list_magic(); // this is a vector so you can iterate on it
    std::cout << users << '\n';

    // Iterate on your users
    for (auto const& user : users) {
        std::cout << user.name << std::endl;
    }

    // Get precise informations on a specific user
    auto user_id = users[0].id;
    std::cout << user_id << std::endl;
    std::cout << slack.users.info(user_id).dump(4) << std::endl;
}
