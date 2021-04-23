#include <iostream>
#include <fstream>
#include <chrono>
#include <signal.h>
#include <thread>

#include "gui.h"


#define LINE_LIMIT 100


bool running = true;

void sigHandler(int signum)
{
    running = false;
}

void catchSigterm()
{
   struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = sigHandler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, nullptr);
}

void checkArguments(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " <<  argv[0] << " [FILE]" << std::endl;
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    checkArguments(argc, argv);

    catchSigterm();

    Gui* gui = new Gui();

    while (running)
    {
        int i = 0;

        std::ifstream filein(argv[1]);
        for (std::string line; std::getline(filein, line) && i < LINE_LIMIT; ++i) {
            gui->drawMessage(line);
        }

        gui->flush();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    delete gui;

    return 0;
}