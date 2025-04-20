#include "test_config.h"

#include "acutest.h"
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "ansi.h"
#include "color.h"
#include "config.h"
#include "gui.h"

void TestConfig::test_parseCommentLine()
{
	TEST_CASE("test parseCommentLine: starting with ';'");
	TEST_CHECK(Config::parseCommentLine(";this is a comment") == true);
	TEST_CHECK(Config::parseCommentLine("  ;  this is a comment") == true);
	TEST_CHECK(Config::parseCommentLine("	;this is a comment") == true);

	TEST_CASE("test parseCommentLine: starting with '#'");
	TEST_CHECK(Config::parseCommentLine("#this is a comment") == true);
	TEST_CHECK(Config::parseCommentLine("  #  this is a comment") == true);
	TEST_CHECK(Config::parseCommentLine("	#this is a comment") == true);

	TEST_CASE("test parseCommentLine: no comment line");
	TEST_CHECK(Config::parseCommentLine("key=value ;some comment") == false);
	TEST_CHECK(Config::parseCommentLine("key=value #some comment") == false);
}


void TestConfig::test_parseSectionLine()
{
	std::string section;

	TEST_CASE("test test_parseSectionLine (regular)");
	TEST_CHECK(Config::parseSectionLine("[test]", section) == true);
	TEST_CHECK(section == "test");

	TEST_CASE("test test_parseSectionLine (with spaces)");
	TEST_CHECK(Config::parseSectionLine("  [  test  ]  ", section) == true);
	TEST_CHECK(section == "test");
}

void TestConfig::test_parseKeyValueLine_forInteger()
{
	Config config = Config::defaultConfig();
	int defaultMonitorIndex = config.monitorIndex;
	std::string section = "Positioning";

	TEST_CASE("test parseKeyValueLine: MonitorIndex (invalid value)");
	TEST_EXCEPTION(Config::parseKeyValueLine("MonitorIndex=abc", section, config), std::runtime_error);
	TEST_CHECK(config.monitorIndex == defaultMonitorIndex);

	TEST_CASE("test parseKeyValueLine: MonitorIndex (commented value)");
	TEST_CHECK(Config::parseKeyValueLine("#MonitorIndex=123", section, config) == false);
	TEST_CHECK(config.monitorIndex == defaultMonitorIndex);

	TEST_CASE("test parseKeyValueLine: MonitorIndex (empty value)");
	TEST_CHECK(Config::parseKeyValueLine("MonitorIndex=", section, config) == false);
	TEST_CHECK(config.monitorIndex == defaultMonitorIndex);

	TEST_CASE("test parseKeyValueLine: MonitorIndex (only spaces and tabs)");
	TEST_CHECK(Config::parseKeyValueLine("MonitorIndex=  	 ", section, config) == false);
	TEST_CHECK(config.monitorIndex == defaultMonitorIndex);

	TEST_CASE("test parseKeyValueLine: MonitorIndex (with spaces and tabs)");
	TEST_CHECK(Config::parseKeyValueLine("  MonitorIndex 	=  321   ", section, config) == true);
	TEST_CHECK(config.monitorIndex == 321);

	TEST_CASE("test parseKeyValueLine: MonitorIndex (regular)");
	TEST_CHECK(Config::parseKeyValueLine("MonitorIndex=123", section, config) == true);
	TEST_CHECK(config.monitorIndex == 123);
}


void TestConfig::test_parseKeyValueLine_forString()
{
	Config config = Config::defaultConfig();
	std::string defaultInputFile = config.inputFile;
	std::string section = "";

	TEST_CASE("test parseKeyValueLine: InputFile (empty value)");
	TEST_CHECK(Config::parseKeyValueLine("InputFile=", section, config) == false);
	TEST_CHECK(config.inputFile == defaultInputFile);

	TEST_CASE("test parseKeyValueLine: InputFile (only spaces and tabs)");
	TEST_CHECK(Config::parseKeyValueLine("InputFile= 	  ", section, config) == false);
	TEST_CHECK(config.inputFile == defaultInputFile);

	TEST_CASE("test parseKeyValueLine: InputFile (with spaces and tabs)");
	TEST_CHECK(Config::parseKeyValueLine("  InputFile 	=  /var/log/some_filename with spaces.log   ", section, config) == true);
	TEST_CHECK(config.inputFile == "/var/log/some_filename with spaces.log");

	TEST_CASE("test parseKeyValueLine: InputFile (regular)");
	TEST_CHECK(Config::parseKeyValueLine("InputFile=/var/log/some_filename.log", section, config) == true);
	TEST_CHECK(config.inputFile == "/var/log/some_filename.log");
}

void TestConfig::test_parseKeyValueLine_forOrientation()
{
	Config config = Config::defaultConfig();
	int defaultOrientation = config.orientation;
	std::string section = "Positioning";

	TEST_CASE("test parseKeyValueLine: Orientation (invalid value)");
	TEST_EXCEPTION(Config::parseKeyValueLine("Orientation=abc", section, config), std::runtime_error);
	TEST_CHECK(config.orientation == defaultOrientation);

	TEST_CASE("test parseKeyValueLine: Orientation (empty value)");
	TEST_CHECK(Config::parseKeyValueLine("Orientation=", section, config) == false);
	TEST_CHECK(config.orientation == defaultOrientation);

	TEST_CASE("test parseKeyValueLine: Orientation (only spaces and tabs)");
	TEST_CHECK(Config::parseKeyValueLine("Orientation=   ", section, config) == false);
	TEST_CHECK(config.orientation == defaultOrientation);

	TEST_CASE("test parseKeyValueLine: Orientation (with spaces and tabs)");
	TEST_CHECK(Config::parseKeyValueLine("  Orientation	 =  CENTER", section, config) == true);
	TEST_CHECK(config.orientation == Gui::Orientation::CENTER);

	TEST_CASE("test parseKeyValueLine: Orientation (regular)");
	TEST_CHECK(Config::parseKeyValueLine("Orientation=SE", section, config) == true);
	TEST_CHECK(config.orientation == Gui::Orientation::SE);
}

void TestConfig::test_fromFile() {
	Config defaultConfig = Config::defaultConfig();
	Config fileConfig = Config::fromFile(getExecutableDir() + "../test/resources/test-x11-overlayrc");

	TEST_CASE("test fromFile (regular)");
	TEST_CHECK(fileConfig.monitorIndex != defaultConfig.monitorIndex);
	TEST_CHECK(fileConfig.monitorIndex == 1);
	TEST_CHECK(fileConfig.orientation != defaultConfig.orientation);
	TEST_CHECK(fileConfig.orientation ==  Gui::Orientation::SE);
	TEST_CHECK(fileConfig.screenEdgeSpacing != defaultConfig.screenEdgeSpacing);
	TEST_CHECK(fileConfig.screenEdgeSpacing ==  10);
	TEST_CHECK(fileConfig.lineSpacing != defaultConfig.lineSpacing);
	TEST_CHECK(fileConfig.lineSpacing ==  20);
	TEST_CHECK(fileConfig.fontName[0] != defaultConfig.fontName[0]);
	TEST_CHECK(fileConfig.fontName[0] == "Mx437 IBM VGA 8x16");
	TEST_CHECK(fileConfig.fontSize[0] != defaultConfig.fontSize[0]);
	TEST_CHECK(fileConfig.fontSize[0] == 24);
	TEST_CHECK(fileConfig.colorProfile != defaultConfig.colorProfile);
	TEST_CHECK(fileConfig.colorProfile == Ansi::Profile::XP);
	TEST_CHECK(fileConfig.defaultForegroundColor != defaultConfig.defaultForegroundColor);
	TEST_CHECK(fileConfig.defaultForegroundColor == Color(255, 0, 0, 200));
	TEST_CHECK(fileConfig.defaultBackgroundColor != defaultConfig.defaultBackgroundColor);
	TEST_CHECK(fileConfig.defaultBackgroundColor == Color(11, 22, 33, 100));
	TEST_CHECK(fileConfig.inputFile != defaultConfig.inputFile);
	TEST_CHECK(fileConfig.inputFile ==  "/some/inputFile.txt");

	fileConfig = Config::fromFile(getExecutableDir() + "../test/resources/test-x11-overlayrc_colorprofile-after-color");
	TEST_CASE("test fromFile (colorprofile after color)");
	TEST_CHECK(fileConfig.colorProfile == Ansi::Profile::XP);
	TEST_CHECK(fileConfig.defaultForegroundColor != defaultConfig.defaultForegroundColor);
	TEST_CHECK(fileConfig.defaultForegroundColor == Color(255, 0, 0, 200));
}

std::string TestConfig::getExecutablePath() {
	char buffer[1024];
	auto length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (length >= 0) {
    	buffer[length] = '\0';
		return std::string(buffer);
	}
	throw std::runtime_error("cannot retrieve executable path");
}

std::string TestConfig::getExecutableDir() {
	std::string path = getExecutablePath();
	auto pos = path.find_last_of('/');
	return path.substr(0, pos + 1);
}
