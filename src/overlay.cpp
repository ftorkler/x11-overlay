#include <iostream>
#include <fstream>
#include <chrono>
#include <signal.h>
#include <thread>

#include "config.h"
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
    Config config = Config::defaultConfig().overrideWith(Config::fromParameters(argc, argv));

    catchSigterm();

    gui = new Gui();
    gui->setOrientation(config.orientation);
    gui->setMouseOverDimming(config.dimming / 100.0f);
    gui->setMouseOverTolerance(config.mouseOverTolerance);
    gui->setScreenEdgeSpacing(config.screenEdgeSpacing);
    gui->setLineSpacing(config.lineSpacing);

    while (running)
    {
        unsigned long now = clockMillis();
        if (now - lastRead >= CHECK_FILE_INTERVAL_MS) {
            checkInputFile(config.inputFile);
            lastRead = now;
        }

        gui->flush();

        sleep(CHECK_GUI_INTERVAL_MS);
    }

    delete gui;

    return 0;
}