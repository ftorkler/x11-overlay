#include <chrono>
#include <signal.h>
#include <thread>

#include "window.h"

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

int main(int argc, char *argv[])
{
    catchSigterm();

    X11Window* window = new X11Window(100, 100, 480, 640);

    XColor bgColor = window->createColor(0, 0, 0, 100);
    XColor redColor = window->createColor(255, 0, 0, 200);

    while (running)
    {
        window->clear();
        window->drawRect(0, 0, 350, 30, bgColor);
        window->drawRect(0, 50, 350, 30, bgColor);
        window->drawString(0, 0, "message", redColor);
        window->drawString(0, 50, "message 2", redColor);
        window->flush();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    delete window;

    return 0;
}