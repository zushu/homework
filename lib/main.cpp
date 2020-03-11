//#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include "message.h"
#include "logging.h"
#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

// TODO: WRITE IN C++
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
        std::string bidder_name_temp;
        //std::cin >> bidder_name[i] >> num_args[i];
        std::cin >> bidder_name_temp >> num_args[i];
        bidder_name[i] = new char(bidder_name_temp.length());
        strcpy(bidder_name[i], bidder_name_temp.c_str());

        //args[i] = new std::string[num_args[i] + 1];
        args[i] = new char*[num_args[i] + 1];
        for (int j = 0; j < num_args[i] + 1; j++)
        {
            if (j == num_args[i])
            {
                args[i][num_args[i]] = NULL;
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
    /*for (int i = 0; i < num_bidders; i++)
    {
        std::cout << bidder_name[i] << " " <<num_args[i] << " ";
        for (int j = 0 ; j < num_args[i]; j++)
        {
            std::cout << args[i][j] << " ";
        }
        std::cout << "\n";
    }*/

    int fd[2];
    int fd_array[num_bidders][2];
    cm cm_array[num_bidders];
    sm sm_array[num_bidders];
    pid_t pids[num_bidders];
    pid_t pid;

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
            std::cout << "child " << i << " " << getpid() << " parentid: " << getppid() <<std::endl;
            close(fd_array[i][1]); // will use fd[0] to read and write
            dup2(fd_array[i][0], 0); // redirect stdin to read end
            //dup2(fd_array[i][0], 1); // redirect stdout to write end (same as the read end)
            close(fd_array[i][0]); // it is duplicated to stdin&stdout, so close
            //execvp(bidder_name[i], args[i]);
            //execl("/bin/echo", "echo", )
            char tmp2[6];
            //fgets(tmp2, 6*sizeof(char), stdin);
            read(0, tmp2, 6*sizeof(char));
            //dup2(1, 1);
            printf("%s\n", tmp2);
            exit(0);

        }

    }
    if (pid > 0)
    {
        for (int i = 0; i< num_bidders; i++)
        {
            std::cout << "parent loop 2: " << getpid() <<std::endl;
            close(fd_array[i][0]); // will use fd[1] to read and write
            dup2(fd_array[i][1], 0); // redirect stdin to read end
            //dup2(fd_array[i][1], 1); // redirect stdout to write end (same as the read end)
            //close(fd_array[i][1]); // it is duplicated to stdin&stdout, so close
            //write(fd_array[i][1], "hello", 5*sizeof(char));
            //write(0, "hello", 5*sizeof(char));
            char tmp[6] = "hello";
            write(fd_array[i][1], tmp, 5*sizeof(char));

            //std::cout << "waiting for child " << i << std::endl;
            wait(&child_status);
            std::cout << "child " << i << " finished with status " << child_status << std::endl;
        }
    }
    
    return 0;

}
