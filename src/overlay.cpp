#include <iostream>
#include <fstream>
#include <chrono>
#include <signal.h>
#include <thread>

#include "window.h"


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

    X11Window* window = new X11Window(100, 100, 480, 640);

    XColor bgColor = window->createColor(0, 0, 0, 100);
    XColor redColor = window->createColor(255, 0, 0, 200);

    while (running)
    {
        window->clear();

        int y = 0;
        int i = 0;

        std::ifstream filein(argv[1]);
        for (std::string line; std::getline(filein, line) && i < LINE_LIMIT; ++i) {
            int w = window->getStringWidth(line);
            int h = window->getStringHeight(line);

            if (line.size()) {
                window->drawRect(0, y, w, h, bgColor);
            }
            window->drawString(0, y, line, redColor);

            y += h + 1;
        }

        window->flush();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    delete window;

    return 0;
}