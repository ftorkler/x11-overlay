#include <chrono>
#include <thread>

#include "window.h"

int main(int argc, char *argv[])
{
    X11Window* window = new X11Window(100, 100, 480, 640);

    while (true)
    {
        window->clear();
        window->flush();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}