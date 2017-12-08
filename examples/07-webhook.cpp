#include "slacking.hpp"
#include <iostream>
#include <fstream>
 int main(int argc,char** argv)
{
	

    slack::Slacking slack("xxx-xxx"); // where "xxx-xxx" is your Slack API token
	slack.set_proxy("http://10.0.22.1:8080");
	slack.hook.Id = "XXXXXXXXX/YYYYYYYYY/ZZZZZZZZZZZZZZZZZZZZZZZZ";  // where "XXXXXXXXX/YYYYYYYYY/ZZZZZZZZZZZZZZZZZZZZZZZZ" is  key of web hook
    //slack.hook.channel_username_iconemoji("#buildmachine", "", "");
    slack.hook.postMessage("Slacking supports incoming webhooks");

   
}
