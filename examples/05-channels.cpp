#include "slacking.hpp"

#include <fstream>

int main() {
    // Load the token from a file. You can also specify directly the token in your code as a string.
    std::string mytoken;
    std::ifstream infile("token.txt");
    std::getline(infile, mytoken);

    slack::create(mytoken);
    
    // You can display all the JSON response
    std::cout << slack::instance().channels.list() << std::endl; 

    // You can display filtered informations
    auto channels = slack::channels().list_magic();
    std::cout << channels << '\n';

    // Iterate on channels
    for (auto const& channel : channels) {
        std::cout << channel.name << std::endl;
    }

    // Get precise informations on a specific channel
    auto channel_id = channels[0].id;
    std::cout << slack::channels().info(channel_id) << std::endl;
}


