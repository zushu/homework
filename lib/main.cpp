//#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string>
#include <string.h>
#include "message.h"
#include "logging.h"
#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

// TODO: WRITE IN C++
// TODO: NO NEED TO KEEP ALL SERVER MESSAGES, DELETE SM_ARRAY, MAYBE ALSO CM_ARRAY

void server(int fd_array[][2], 
            int starting_bid, int min_increment, int num_bidders,
            cm cm_array[], sm sm_array[]);
int main()
{
    // INPUT 
    int starting_bid, min_increment, num_bidders;
    std::cin >> starting_bid >> min_increment >> num_bidders;

    //std::string bidder_name[num_bidders];
    char* bidder_name[num_bidders];
    int num_args[num_bidders];
    //std::string* args[num_bidders];
    char** args[num_bidders];

    for (int i = 0; i < num_bidders; i++)
    {
        // TODO: FIX THIS UGLY MESS!!!
        std::string bidder_name_temp;
        //std::cin >> bidder_name[i] >> num_args[i];
        std::cin >> bidder_name_temp >> num_args[i];
        bidder_name[i] = new char[bidder_name_temp.length() + 8*sizeof(char)];
        strcpy(bidder_name[i], "../bin/");
        strcat(bidder_name[i], bidder_name_temp.c_str());
        //args[i] = new std::string[num_args[i] + 1];
        args[i] = new char*[num_args[i] + 2];
        args[i][0] = new char[bidder_name_temp.length() + 8*sizeof(char)];
        args[i][0] = bidder_name[i];
        for (int j = 1; j < num_args[i] + 2; j++)
        {
            if (j == (num_args[i] + 1))
            {
                args[i][num_args[i] + 1] = NULL;
                continue;
            }
            std::string arg_temp;
            //std::cin >> args[i][j];
            std::cin >> arg_temp;
            args[i][j] = new char[arg_temp.length()];
            strcpy(args[i][j], arg_temp.c_str());
        }
        //args[i][num_args[i]] = NULL;
    }

    // TESTING INPUT
    for (int i = 0; i < num_bidders; i++)
    {
        //std::cout << bidder_name[i] << " " <<num_args[i] << " ";
        for (int j = 0 ; j < num_args[i] + 2; j++)
        {
            std::cout << "arg: " << args[i][j] << " \n";
        }
        std::cout << "\n";
    }

    int fd[2];
    int fd_array[num_bidders][2];
    cm cm_array[num_bidders];
    sm sm_array[num_bidders];
    pid_t pids[num_bidders];
    pid_t pid;

    int stdout_copy = dup(1);

    int child_status;
    std::cout << "parent " << getpid() << std::endl; 
    for (int i = 0; i < num_bidders; i++)
    {
        if (PIPE(fd_array[i]) < 0)
        {
            perror("pipe error");
        }
    
    //for (int i = 0; i < num_bidders; i++)
    //{
        //pids[i] = fork();
        pid = fork();
        //if (pids[i] < 0)
        if (pid < 0)
        {
            perror("fork error");
        }
        
        //else if (pids[i] == 0)// child
        if (pid == 0)
        {
            //int stdout_copy = dup(1);
            std::cout << "child " << i << " " << getpid() << " parentid: " << getppid() <<std::endl;
            close(fd_array[i][1]); // will use fd[0] to read and write
            dup2(fd_array[i][0], 0); // redirect stdin to read end
            dup2(fd_array[i][0], 1); // redirect stdout to write end (same as the read end)
            //close(fd_array[i][0]);
            //dup2(stdout_copy, 1);
            //close(stdout_copy);
            //exit(0);
            execvp(bidder_name[i], args[i]);

        }

    }

    bool bidder_finished[num_bidders];
    if (pid > 0)
    {
        for (int i = 0; i< num_bidders; i++)
        {
            std::cout << "parent loop 2: " << getpid() <<std::endl;
            close(fd_array[i][0]); // will use fd[1] to read and write
            dup2(fd_array[i][1], 0); // redirect stdin to read end
            dup2(fd_array[i][1], 1); // redirect stdout to write end (same as the read end)
            //close(fd_array[i][1]);

            //dup2(stdout_copy, 1);
            //close(stdout_copy);
            //std::cout << "waiting for child " << i << std::endl;
            // TODO: NOT CORRECT, SERVER ALREADY HAS A LOOP, SHOULD BE OUTSIDE THIS LOOP
            
            //wait(&child_status);
            //std::cout << "child " << i << " finished with status " << child_status << std::endl;
        }
        server(fd_array, starting_bid, min_increment, num_bidders, cm_array, sm_array);
        //bool bidders_not_finished = 1;
        //while(bidders_not_finished)
        //{

        //}
    }
    
    for (int i = 0; i < num_bidders; i++)
    {
        delete bidder_name[i];
        delete[] args[i];
    }
    return 0;

}














// TODO: SEND RESPONSE MESSAGES
// SERVER CODE TO HANDLE MESSAGES
void server(int fd_array[][2], int starting_bid, int min_increment, int num_bidders, cm cm_array[], sm sm_array[])
{
    
    fd_set readset;
    bool open = true; // if there is at least one pipe open
    bool* bidder_is_open = new bool[num_bidders]; 
    int* bidder_delays = new int[num_bidders];
    int* bids = new int[num_bidders];
    int* bidder_status = new int[num_bidders];
    int max_fd = 0; // number of the file descriptor with the biggest number
    // TODO: CORRECTLY UPDATE highest_bid
    int highest_bid = 0;

    wi win_info;

    /*
    struct timeval 
    {
        long tv_sec; //second
        long tv_usec; // microseconds
    };*/
    struct timeval* timeout_value = new timeval;
    timeout_value = NULL; //

    int r; // return value of read call
    // initialize bidder pipes as open
    for (int i = 0; i < num_bidders; i++)
    {
        bidder_is_open[i] = true;
    }

    for (int i = 0; i < num_bidders; i++)
    {
        if (fd_array[i][1] > max_fd)
        {
            max_fd = fd_array[i][1];
        }
    }

    max_fd += 1;

    while(open)
    {
        FD_ZERO(&readset);
        for (int i = 0; i < num_bidders; i++)
        {
            if (bidder_is_open[i]) 
            {
                // add fd to readset if open
                FD_SET(fd_array[i][1], &readset);
            }
        }

        // block until there is data to read
        if (select(max_fd, &readset, NULL, NULL, timeout_value) < 0)
            perror("select");

        // escaped select, there is data to read
        for (int i = 0; i < num_bidders; i++)
        {
            if (FD_ISSET(fd_array[i][1], &readset))
            {
                r = read(fd_array[i][1], &(cm_array[i]), sizeof(cm));
                if (r == 0) // EOF
                {
                    bidder_is_open[i] = false;
                }
                else
                {
                    // do something accordingly
                    // first message
                    if (cm_array[i].message_id == CLIENT_CONNECT)
                    {
                        bidder_delays[i] = cm_array[i].params.delay;
                        // TODO: SEND MESSAGE BACK FROM SERVER
                        sm_array[i].message_id = SERVER_CONNECTION_ESTABLISHED;
                        // unique client id starts from 1 ??????
                        sm_array[i].params.start_info.client_id = i + 1;
                        sm_array[i].params.start_info.starting_bid = starting_bid;
                        sm_array[i].params.start_info.current_bid = highest_bid;
                        sm_array[i].params.start_info.minimum_increment = min_increment;

                        write(fd_array[i][1], &sm_array[i], sizeof(sm));


                    }
                    if (cm_array[i].message_id == CLIENT_BID)
                    {
                        bids[i] = cm_array[i].params.bid;
                        // TODO: CHECK BID, SEND APPROPRIATE MESSAGE, CHANGE MAX_BID IF NECESSARY
                        sm_array[i].message_id = SERVER_BID_RESULT;

                        // check bid
                        if (bids[i] < starting_bid)
                            sm_array[i].params.result_info.result = BID_LOWER_THAN_STARTING_BID;
                        else if (bids[i] < highest_bid)
                            sm_array[i].params.result_info.result = BID_LOWER_THAN_CURRENT;
                        else if (highest_bid - bids[i] < min_increment)
                            sm_array[i].params.result_info.result = BID_INCREMENT_LOWER_THAN_MINIMUM;
                        else
                        {
                            sm_array[i].params.result_info.result = BID_ACCEPTED;
                            // update highest bid
                            highest_bid = bids[i];
                            win_info.winner_id = i + 1;
                            win_info.winning_bid = highest_bid;
                        }
                        sm_array[i].params.result_info.current_bid = highest_bid;

                        // send message
                        write(fd_array[i][1], &sm_array[i], sizeof(sm));
                    }
                    // final message
                    if (cm_array[i].message_id == CLIENT_FINISHED)
                    {
                        bidder_status[i] = cm_array[i].params.status;
                        // TODO: CHECK STATUS
                        bidder_is_open[i] = false;

                    }

                }
            }
        }

        int max_delay = 0;
        for (int i = 0; i < num_bidders; i++)
        {
            if (bidder_delays[i] > max_delay)
            {
                max_delay = bidder_delays[i];
            }
        }
        timeout_value->tv_sec = 0;
        timeout_value->tv_usec = max_delay * 1000;

        open = false;
        for (int i = 0; i < num_bidders; i++)
        {
            if (bidder_is_open[i])
            {
                open = true;
                break;
            }
        }

    }

    int child_status;
    // out of loop, bidders are finished
    for (int i = 0; i < num_bidders; i++)
    {
        sm_array[i].message_id = SERVER_AUCTION_FINISHED;
        sm_array[i].params.winner_info = win_info;
        write(fd_array[i][1], &sm_array[i], sizeof(sm));
        wait(&child_status);
    }

    delete timeout_value;
    delete bidder_is_open; 
    delete bidder_delays;
    delete bids;
    delete bidder_status;
}
