//#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

// TODO: WRITE IN C++
int main()
{
    // INPUT 
    int starting_bid, min_increment, num_bidders;
    std::cin >> starting_bid >> min_increment >> num_bidders;
    std::string bidder_name[num_bidders];
    int num_args[num_bidders];
    int* args[num_bidders];

    for (int i = 0; i < num_bidders; i++)
    {
        std::cin >> bidder_name[i] >> num_args[i];
        args[i] = new int[num_args[i]];
        for (int j = 0; j < num_args[i]; j++)
        {
            
            std::cin >> args[i][j];
        }
    }

    // TESTING INPUT
    /*for (int i = 0; i < num_bidders; i++)
    {
        std::cout << bidder_name[i] << " " <<num_args[i] << " ";
        for (int j = 0 ; j < num_args[i]; j++)
        {
            std::cout << args[i][j] << " ";
        }
        std::cout << "\n";
    }*/


}
