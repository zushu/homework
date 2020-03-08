#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>

// TODO: WRITE IN C++
int main()
{
    int starting_bid, min_increment, num_bidders;
    scanf(" %d %d %d", &starting_bid, &min_increment, &num_bidders);
    // TODO: MALLOC ?
    char* bidder_name[num_bidders];
    int num_args[num_bidders];
    int* args[num_bidders];

    for (int i = 0; i < num_bidders; i++)
    {
//        int num_args;
        scanf(" %s %d", bidder_name[i], &(num_args[i]));
        /*for (int j = 0; j < num_args; j++)
        {
            scanf
        }*/
    }
}
