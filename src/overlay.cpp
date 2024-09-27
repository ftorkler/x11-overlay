#include <iostream>
#include <fstream>
#include <chrono>
#include <signal.h>
#include <thread>

#include "config.h"
#include "gui.h"
#include "inotify.h"


#define LINE_LIMIT 100
#define CHECK_GUI_INTERVAL_MS 50

Gui* gui = nullptr;
bool running = true;


void sleepMillis(unsigned long millis)
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

void loadInputFile(const std::string& filename)
{
    gui->clearMessages();
    
    std::ifstream filein(filename, std::ifstream::in);
    int i = 0;
    for (std::string line; std::getline(filein, line) && i < LINE_LIMIT; ++i) {
        gui->addMessage(line);
    }
    filein.close();
}

Config readConfig(int argc, char* argv[])
{
    Config configFromParameters = Config::fromParameters(argc, argv);   
    Config configFromFile = configFromParameters.configFile != "" 
        ? Config::fromFile(configFromParameters.configFile, false) 
        : Config::fromFile(Config::getDefaultConfigFilePath(), true);

    Config config = Config::defaultConfig()
        .overrideWith(configFromFile)
        .overrideWith(configFromParameters);

    if (config.inputFile.empty()) {
        std::cout << "ERROR: parameter 'INPUT_FILE' needs a value" << std::endl;
        std::cout << std::endl;
        Config::exitWithUsage(1);
    }

    if (config.verbose) {
        config.print(std::cout);
    }

    return config;
}

int main(int argc, char* argv[])
{
    catchSigterm();

    Config config = readConfig(argc, argv);
    Inotify inotify = Inotify(config.inputFile);

    gui = new Gui();
    // Postioning
    gui->setOrientation(config.orientation);
    gui->setScreenEdgeSpacing(config.screenEdgeSpacing);
    gui->setLineSpacing(config.lineSpacing);
    gui->setMonitorIndex(config.monitorIndex);
    // Font
    gui->setFont(config.fontName + "-" + std::to_string(config.fontSize));
    // Colors
    gui->setColorProfile(config.colorProfile);
    gui->setDefaultForgroundColor(config.defaultForegroundColor);
    gui->setDefaultBackgroundColor(config.defaultBackgroundColor);
    // Behavior
    gui->setDimming(config.dimming / 100.0f);
    gui->setMouseOverDimming(config.mouseOverDimming / 100.0f);
    gui->setMouseOverTolerance(config.mouseOverTolerance);

    loadInputFile(config.inputFile);

    while (running)
    {
        if (inotify.hasFileBeenRewritten()) {
            loadInputFile(config.inputFile);
        }

        gui->flush();
        sleepMillis(CHECK_GUI_INTERVAL_MS);
    }

    delete gui;

    return 0;
}