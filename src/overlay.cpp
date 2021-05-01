#include <iostream>
#include <fstream>
#include <chrono>
#include <signal.h>
#include <thread>

#include "gui.h"


#define LINE_LIMIT 100
#define CHECK_FILE_INTERVAL_MS 1000
#define CHECK_GUI_INTERVAL_MS 50

Gui* gui = nullptr;
bool running = true;
unsigned long lastRead = 0;


unsigned long clockMillis() 
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
}

void sleep(unsigned long millis)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

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
        std::cout << "Usage: " <<  argv[0] << " <FILE>" << std::endl;
        exit(0);
    }
}

void checkInputFile(const std::string& filename)
{
    gui->clearMessages();      
    
    std::ifstream filein(filename, std::ifstream::in);
    int i = 0;
    for (std::string line; std::getline(filein, line) && i < LINE_LIMIT; ++i) {
        gui->addMessage(line);
    }
    filein.close();
}

int main(int argc, char *argv[])
{
    checkArguments(argc, argv);

    catchSigterm();

    gui = new Gui();

    while (running)
    {
        unsigned long now = clockMillis();
        if (now - lastRead >= CHECK_FILE_INTERVAL_MS) {
            checkInputFile(argv[1]);
            lastRead = now;
        }

        gui->flush();

        sleep(CHECK_GUI_INTERVAL_MS);
    }

    delete gui;

    return 0;
}