// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// processing_post_data program
#include "server.h"

int main(int argc, char **argv)
{
    Jade::Server server;
    server.run();
    std::cout << "Press any key to quit\n";
    // The program will finish when the user enters a character. If the call to getchar() is omitted
    // then the cleanup code for the sever daemon will run immediately. There are better ways to do
    // this of course but it is fine for a simple example program such as this one.
    getchar();
}