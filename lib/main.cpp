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
//#include <limits.h>
#include "message.h"
#include "logging.h"

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

// TODO: WRITE IN C
// TODO: NO NEED TO KEEP ALL SERVER MESSAGES, DELETE SM_ARRAY, MAYBE ALSO CM_ARRAY

void server(int fd_array[][2], 
            int starting_bid, int min_increment, int num_bidders, pid_t pids[]);
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
        std::cin >> bidder_name_temp >> num_args[i];
        bidder_name[i] = new char[bidder_name_temp.length() + 8*sizeof(char)];
        strcpy(bidder_name[i], "../bin/");
        strcat(bidder_name[i], bidder_name_temp.c_str());
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
            std::cin >> arg_temp;
            args[i][j] = new char[arg_temp.length()];
            strcpy(args[i][j], arg_temp.c_str());
        }
    }

    int fd[2];
    int fd_array[num_bidders][2];
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
    
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
        }
        if (pid == 0)
        {
            pids[i] = getpid();
            close(fd_array[i][1]); // will use fd[0] to read and write
            dup2(fd_array[i][0], 0); // redirect stdin to read end
            dup2(fd_array[i][0], 1); // redirect stdout to write end (same as the read end)
            close(fd_array[i][0]);
            execvp(bidder_name[i], args[i]);

        }

    }

    bool bidder_finished[num_bidders];
    if (pid > 0)
    {
        for (int i = 0; i< num_bidders; i++)
        {
            close(fd_array[i][0]); // will use fd[1] to read and write
            //dup2(fd_array[i][1], 0); // redirect stdin to read end
            //dup2(fd_array[i][1], 1); // redirect stdout to write end (same as the read end)
            //close(fd_array[i][1]);
        }

        server(fd_array, starting_bid, min_increment, num_bidders, pids);
    }
    
    for (int i = 0; i < num_bidders; i++)
    {
        delete bidder_name[i];
        delete[] args[i];
    }
    return 0;

}

// SERVER CODE TO HANDLE MESSAGES
void server(int fd_array[][2], int starting_bid, int min_increment, int num_bidders, pid_t pids[])
{   
    fd_set readset;
    bool open = true; // if there is at least one pipe open
    bool* bidder_is_open = new bool[num_bidders]; 
    int* bidder_delays = new int[num_bidders];
    int cl_bid;
    int* bidder_status = new int[num_bidders];
    int max_fd = 0; // number (id) of the file descriptor with the biggest number
    // TODO: CORRECTLY UPDATE highest_bid
    int highest_bid = 0;
    wi win_info;
    cm cl_message; // client message
    sm sv_message; // server message
    struct timeval timeout_value = {0, 0};
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
        //printf("open\n");
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
        if (timeout_value.tv_usec == 0)
        {
            //printf("timeout_value.tv_usec == 0\n");
            if (select(max_fd, &readset, NULL, NULL, NULL) < 0)
            perror("select1");
        } 
        else
        {   
            //printf("timeout_value.tv_usec not zero\n");
            if (select(max_fd, &readset, NULL, NULL, &timeout_value) < 0)
                perror("select");
        }

        //printf("selectden cixdim\n");

        // escaped select, there is data to read
        for (int i = 0; i < num_bidders; i++)
        {
            if (FD_ISSET(fd_array[i][1], &readset))
            {
                //printf("FD_ISSET\n");
                r = read(fd_array[i][1], &cl_message, sizeof(cm));

                //printf("read from client %d: %d\n", i, cm_array[i].message_id);
                if (r == 0) // EOF
                {
                    bidder_is_open[i] = false;
                }
                else
                {
                    // do something accordingly
                    // first message
                    if (cl_message.message_id == CLIENT_CONNECT)
                    {      

                        bidder_delays[i] = cl_message.params.delay;
                        // TODO: SEND MESSAGE BACK FROM SERVER
                        sv_message.message_id = SERVER_CONNECTION_ESTABLISHED;
                        // unique client id starts from 1 ??????
                        sv_message.params.start_info.client_id = i + 1;
                        sv_message.params.start_info.starting_bid = starting_bid;
                        sv_message.params.start_info.current_bid = highest_bid;
                        sv_message.params.start_info.minimum_increment = min_increment;

                        // print client message
                        ii input_msg;
                        input_msg.type = CLIENT_CONNECT;
                        input_msg.pid = pids[i]; 
                        input_msg.info.delay = cl_message.params.delay; // delay
                        print_input(&input_msg, i+1);

                        write(fd_array[i][1], &sv_message, sizeof(sm));

                        // print server message
                        oi output_msg;
                        output_msg.type = SERVER_CONNECTION_ESTABLISHED;
                        output_msg.pid = getpid();
                        output_msg.info.start_info = {i+1, starting_bid, highest_bid, min_increment};
                        print_output(&output_msg, i+1);
                    }
                    else if (cl_message.message_id == CLIENT_BID)
                    {
                        //printf("CLIENT_BID\n");
                        cl_bid = cl_message.params.bid;
                        //printf(" cl_bid %d\n",  cl_bid);
                        //printf("highest_bid: %d'\n", highest_bid);
                        // TODO: CHECK BID, SEND APPROPRIATE MESSAGE, CHANGE MAX_BID IF NECESSARY
                        //sm sm_temp;
                        sv_message.message_id = SERVER_BID_RESULT;

                        // check bid
                        if ( cl_bid < starting_bid)
                        {
                            //printf(" cl_bid < starting_bid\n");
                            sv_message.params.result_info.result = BID_LOWER_THAN_STARTING_BID;
                        }
                        else if ( cl_bid < highest_bid)
                        {
                            //printf(" cl_bid < highest_bid\n");
                            sv_message.params.result_info.result = BID_LOWER_THAN_CURRENT;
                        }
                        else if (cl_bid - highest_bid < min_increment)
                        {
                            //printf("highest_bid -  cl_bid < min_increment\n");
                            sv_message.params.result_info.result = BID_INCREMENT_LOWER_THAN_MINIMUM;
                        }
                        else
                        {
                            //printf("bid else\n");
                            sv_message.params.result_info.result = BID_ACCEPTED;
                            // update highest bid
                            highest_bid = cl_bid;
                            win_info.winner_id = i + 1;
                            win_info.winning_bid = highest_bid;
                        }
                        sv_message.params.result_info.current_bid = highest_bid;

                        // print client message
                        ii input_msg;
                        input_msg.type = CLIENT_BID;
                        input_msg.pid = pids[i]; 
                        input_msg.info.bid = cl_bid; // bid
                        print_input(&input_msg, i+1);

                        write(fd_array[i][1], &sv_message, sizeof(sm));

                        // print server message
                        oi output_msg;
                        output_msg.type = SERVER_BID_RESULT;
                        output_msg.pid = getpid();
                        output_msg.info.result_info = sv_message.params.result_info;
                        print_output(&output_msg, i+1);
                    }
                    // final message
                    else if (cl_message.message_id == CLIENT_FINISHED)
                    {
                        //printf("CLIENT_FINISHED\n");
                        bidder_status[i] = cl_message.params.status;
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
        timeout_value.tv_sec = 0;
        timeout_value.tv_usec = max_delay * 1000;

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

    oi output_msg;
    output_msg.type = SERVER_AUCTION_FINISHED;
    output_msg.pid = getpid();
    output_msg.info.winner_info = win_info;
    print_server_finished(win_info.winner_id, win_info.winning_bid);
    int child_status;
    // out of loop, bidders are finished
    for (int i = 0; i < num_bidders; i++)
    {
        //sm sm_temp;
        sv_message.message_id = SERVER_AUCTION_FINISHED;
        sv_message.params.winner_info = win_info;
        write(fd_array[i][1], &sv_message, sizeof(sm));
        //int child_status;
        wait(&child_status);
        ii input_msg;
        input_msg.type = CLIENT_FINISHED;
        input_msg.pid = pids[i]; 
        input_msg.info.status = child_status; // status
        print_client_finished(i+1, child_status, bidder_status[i]);
    }

    delete bidder_is_open; 
    delete bidder_delays;
    delete bidder_status;
}
