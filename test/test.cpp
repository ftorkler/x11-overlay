#include "acutest.h"

#include <iostream>
#include "color.h"

void check_color_fromAnsi8bit(int givenCode, Color expectedColor)
{
	Color color = Color::_fromAnsi8bit(givenCode);
	TEST_CHECK_(color == expectedColor, "fromAnsi8bit(%d) was (%d,%d,%d,%d), but expected to be (%d,%d,%d,%d)", 
		givenCode,
		color.r, color.g, color.b, color.a,
		expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a);
}

void check_color_fromAnsi(std::string givenCode, Color expectedColor)
{
	Color color = Color::fromAnsi(givenCode);
	TEST_CHECK_(color == expectedColor, "fromAnsi(ESC%s) was (%d,%d,%d,%d), but expected to be (%d,%d,%d,%d)", 
		givenCode.substr(1).c_str(),
		color.r, color.g, color.b, color.a,
		expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a);
}

void test_color_8bit()
{
	// for code not range [0..255] (fallback)
	check_color_fromAnsi8bit(-1, Color(255, 255, 255));
	check_color_fromAnsi8bit(256, Color(255, 255, 255));
	
	// for some random codes in range [0..15] (pallete)
	check_color_fromAnsi8bit(0, Color(0, 0, 0));
	check_color_fromAnsi8bit(7, Color(192, 192, 192));
	check_color_fromAnsi8bit(15, Color(255, 255, 255));
	
	// for some random code in range [16..231] (rgb)
	check_color_fromAnsi8bit(16, Color(0, 0, 0));
	check_color_fromAnsi8bit(22, Color(0, 95, 0));
	check_color_fromAnsi8bit(28, Color(0, 135, 0));
	check_color_fromAnsi8bit(34, Color(0, 175, 0));
	check_color_fromAnsi8bit(40, Color(0, 215, 0));
	check_color_fromAnsi8bit(46, Color(0, 255, 0));
	check_color_fromAnsi8bit(201, Color(255, 0, 255));
	check_color_fromAnsi8bit(207, Color(255, 95, 255));
	check_color_fromAnsi8bit(213, Color(255, 135, 255));
	check_color_fromAnsi8bit(219, Color(255, 175, 255));
	check_color_fromAnsi8bit(225, Color(255, 215, 255));
	check_color_fromAnsi8bit(231, Color(255, 255, 255));

	// for some random code in range [232..255] (gradient black to white)
	check_color_fromAnsi8bit(232, Color(8, 8, 8));
	check_color_fromAnsi8bit(240, Color(88, 88, 88));
	check_color_fromAnsi8bit(248, Color(168, 168, 168));
	check_color_fromAnsi8bit(255, Color(238, 238, 238));
}

void test_color_ansi()
{
	// 8bit
	check_color_fromAnsi("\e[38;5;2m", Color(0, 128, 0));
	check_color_fromAnsi("\e[38;5;23m", Color(0, 95, 95));
	check_color_fromAnsi("\e[38;5;234m", Color(28, 28, 28));

	// 24bit
	check_color_fromAnsi("\e[38;2;1;13;145m", Color(1, 13, 145));         // colorprofile <none>
	check_color_fromAnsi("\e[38;2;0;128;12;1m", Color(128, 12, 1));       // colorprofile '0' 
	check_color_fromAnsi("\e[38;2;1;128;128;128m", Color(128, 128, 128)); // colorprofile '1' 

	check_color_fromAnsi("\e[38;2;0;0;256m", Color(255, 255, 255));
	check_color_fromAnsi("\e[38;2;0;0;-1m", Color(255, 255, 255));
}

TEST_LIST = {
   { "should return correct color for 8bit code (256 colors)", test_color_8bit },
   { "should return correct color for ansi code (256 colors)", test_color_ansi },
   { NULL, NULL } /* zeroed record marking the end of the list */
};
