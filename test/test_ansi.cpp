#include "test_ansi.h"

#include "acutest.h"
#include <string>

#include "ansi.h"
#include "color.h"

void check_to_8bit_color(int givenCode, Color expectedColor)
{
	Color color = Ansi::_to8bitColor(givenCode);
	TEST_CHECK_(color == expectedColor, "fromAnsi8bit(%d) was (%d,%d,%d,%d), but expected to be (%d,%d,%d,%d)", 
		givenCode,
		color.r, color.g, color.b, color.a,
		expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a);
}

void check_to_color(std::string givenCode, Color expectedColor)
{
	Color color = Ansi::toColor(givenCode);
	TEST_CHECK_(color == expectedColor, "fromAnsi(ESC%s) was (%d,%d,%d,%d), but expected to be (%d,%d,%d,%d)", 
		givenCode.substr(1).c_str(),
		color.r, color.g, color.b, color.a,
		expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a);
}

void TestAnsi::test_to_8bit_color()
{
	// for code not range [0..255] (fallback)
	check_to_8bit_color(-1, Color(255, 255, 255));
	check_to_8bit_color(256, Color(255, 255, 255));
	
	// for some random codes in range [0..15] (pallete)
	check_to_8bit_color(0, Color(0, 0, 0));
	check_to_8bit_color(7, Color(192, 192, 192));
	check_to_8bit_color(15, Color(255, 255, 255));
	
	// for some random code in range [16..231] (rgb)
	check_to_8bit_color(16, Color(0, 0, 0));
	check_to_8bit_color(22, Color(0, 95, 0));
	check_to_8bit_color(28, Color(0, 135, 0));
	check_to_8bit_color(34, Color(0, 175, 0));
	check_to_8bit_color(40, Color(0, 215, 0));
	check_to_8bit_color(46, Color(0, 255, 0));
	check_to_8bit_color(201, Color(255, 0, 255));
	check_to_8bit_color(207, Color(255, 95, 255));
	check_to_8bit_color(213, Color(255, 135, 255));
	check_to_8bit_color(219, Color(255, 175, 255));
	check_to_8bit_color(225, Color(255, 215, 255));
	check_to_8bit_color(231, Color(255, 255, 255));

	// for some random code in range [232..255] (gradient black to white)
	check_to_8bit_color(232, Color(8, 8, 8));
	check_to_8bit_color(240, Color(88, 88, 88));
	check_to_8bit_color(248, Color(168, 168, 168));
	check_to_8bit_color(255, Color(238, 238, 238));
}

void TestAnsi::test_to_color()
{
	// 8bit
	check_to_color("\e[38;5;2m", Color(0, 128, 0));
	check_to_color("\e[38;5;23m", Color(0, 95, 95));
	check_to_color("\e[38;5;234m", Color(28, 28, 28));

	// 24bit
	check_to_color("\e[38;2;1;13;145m", Color(1, 13, 145));         // colorprofile <none>
	check_to_color("\e[38;2;0;128;12;1m", Color(128, 12, 1));       // colorprofile '0' 
	check_to_color("\e[38;2;1;128;128;128m", Color(128, 128, 128)); // colorprofile '1' 

	check_to_color("\e[38;2;0;0;256m", Color(255, 255, 255));
	check_to_color("\e[38;2;0;0;-1m", Color(255, 255, 255));
}


void TEST_EQUAL(std::string given, std::string expected)
{
    TEST_CHECK_(given == expected, "expected '%s', but was '%s'", expected.c_str(), given.c_str());
}

void TestAnsi::test_split() {
    std::vector<std::string> tokens;
    
    tokens = Ansi::split("text without colors");
    TEST_ASSERT(tokens.size() == 1);
    TEST_CHECK(tokens[0] == "text without colors");

    tokens = Ansi::split("text with \e[38;5;2mred\e[0m color");
    TEST_ASSERT(tokens.size() == 5);
    TEST_EQUAL(tokens[0], "text with ");
    TEST_EQUAL(tokens[1], "\e[38;5;2m");
    TEST_EQUAL(tokens[2], "red");
    TEST_EQUAL(tokens[3], "\e[0m");
    TEST_EQUAL(tokens[4], " color");
}
