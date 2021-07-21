#include "test_ansi.h"

#include "acutest.h"
#include <algorithm>
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

void check_to_color(std::string givenCode, Color expectedColor, bool increaseIntensity = false)
{
	Color color = Ansi::toColor(givenCode, increaseIntensity);
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
	// 3-bit (standard foreground colors)
	check_to_color("\e[30m", Color(0,0,0));       // 00: #000000 black
	check_to_color("\e[31m", Color(128,0,0));     // 01: #800000 red
	check_to_color("\e[32m", Color(0,128,0));     // 02: #008000 green
	check_to_color("\e[33m", Color(128,128,0));   // 03: #808000 yelow
	check_to_color("\e[34m", Color(0,0,128));     // 04: #000080 blue
	check_to_color("\e[35m", Color(128,0,128));   // 05: #800080 magenta
	check_to_color("\e[36m", Color(0,128,128));   // 06: #008080 cyan
	check_to_color("\e[37m", Color(192,192,192)); // 07: #c0c0c0 white
	check_to_color("\e[30m", Color(128,128,128), true); // -> \e[90m
	check_to_color("\e[36m", Color(0,255,255), true);   // -> \e[96m

	// 3-bit (high-intensity foreground colors)
	check_to_color("\e[90m", Color(128,128,128)); // 08: #808080 bright black
	check_to_color("\e[91m", Color(255,0,0));     // 09: #ff0000 bright red
	check_to_color("\e[92m", Color(0,255,0));     // 10: #00ff00 bright green
	check_to_color("\e[93m", Color(255,255,0));   // 11: #ffff00 bright yelow
	check_to_color("\e[94m", Color(0,0,255));     // 12: #0000ff bright blue
	check_to_color("\e[95m", Color(255,0,255));   // 13: #ff00ff bright magenta
	check_to_color("\e[96m", Color(0,255,255));   // 14: #00ffff bright cyan
	check_to_color("\e[97m", Color(255,255,255)); // 15: #ffffff bright white
	check_to_color("\e[90m", Color(128,128,128), true); // -> no effect

	// 3-bit (standard background colors)
	check_to_color("\e[40m", Color(0,0,0));       // 00: #000000 black
	check_to_color("\e[41m", Color(128,0,0));     // 01: #800000 red
	check_to_color("\e[42m", Color(0,128,0));     // 02: #008000 green
	check_to_color("\e[43m", Color(128,128,0));   // 03: #808000 yelow
	check_to_color("\e[44m", Color(0,0,128));     // 04: #000080 blue
	check_to_color("\e[45m", Color(128,0,128));   // 05: #800080 magenta
	check_to_color("\e[46m", Color(0,128,128));   // 06: #008080 cyan
	check_to_color("\e[47m", Color(192,192,192)); // 07: #c0c0c0 white
	check_to_color("\e[40m", Color(128,128,128), true); // -> \e[100m

	// 3-bit (high-intensity background colors)
	check_to_color("\e[100m", Color(128,128,128)); // 08: #808080 bright black
	check_to_color("\e[101m", Color(255,0,0));     // 09: #ff0000 bright red
	check_to_color("\e[102m", Color(0,255,0));     // 10: #00ff00 bright green
	check_to_color("\e[103m", Color(255,255,0));   // 11: #ffff00 bright yelow
	check_to_color("\e[104m", Color(0,0,255));     // 12: #0000ff bright blue
	check_to_color("\e[105m", Color(255,0,255));   // 13: #ff00ff bright magenta
	check_to_color("\e[106m", Color(0,255,255));   // 14: #00ffff bright cyan
	check_to_color("\e[107m", Color(255,255,255)); // 15: #ffffff bright white
	check_to_color("\e[100m", Color(128,128,128), true); // -> no effect

	// 8bit
	check_to_color("\e[38;5;2m", Color(0, 128, 0));
	check_to_color("\e[38;5;23m", Color(0, 95, 95));
	check_to_color("\e[38;5;234m", Color(28, 28, 28));
	check_to_color("\e[48;5;2m", Color(0, 128, 0));
	check_to_color("\e[48;5;23m", Color(0, 95, 95));
	check_to_color("\e[48;5;234m", Color(28, 28, 28));

	// 24bit
	check_to_color("\e[38;2;1;13;145m", Color(1, 13, 145));         // colorprofile <none>
	check_to_color("\e[38;2;0;128;12;1m", Color(128, 12, 1));       // colorprofile '0' 
	check_to_color("\e[38;2;1;128;128;128m", Color(128, 128, 128)); // colorprofile '1' 
	check_to_color("\e[48;2;1;13;145m", Color(1, 13, 145));         // colorprofile <none>
	check_to_color("\e[48;2;0;128;12;1m", Color(128, 12, 1));       // colorprofile '0' 
	check_to_color("\e[48;2;1;128;128;128m", Color(128, 128, 128)); // colorprofile '1' 


	check_to_color("\e[38;2;0;0;256m", Color(255, 255, 255)); 		// out of range -> fallback color
	check_to_color("\e[38;2;0;0;-1m", Color(255, 255, 255));  		// out of range -> fallback color
	check_to_color("\e[38;2;0;0;255m", Color(0, 0, 255));			// in range 
	check_to_color("\e[38;2;0;0;0m", Color(0, 0, 0));				// in range
	check_to_color("\e[48;2;0;0;255m", Color(0, 0, 255));			// in range 
	check_to_color("\e[48;2;0;0;0m", Color(0, 0, 0));				// in range
}


void TEST_EQUAL(std::string given, std::string expected)
{
	std::replace(given.begin(), given.end(), '\e', '^');
	std::replace(expected.begin(), expected.end(), '\e', '^');
    TEST_CHECK_(given == expected, "expected '%s', but was '%s'", expected.c_str(), given.c_str());
}

void TestAnsi::test_split() 
{
    std::vector<std::string> tokens;
    
	/* don't split text */
	TEST_CASE("don't split text");
    tokens = Ansi::split("text without colors");
    TEST_ASSERT(tokens.size() == 1);
    TEST_CHECK(tokens[0] == "text without colors");

	/* split text with simple ansi control sequence inbetween */
    tokens = Ansi::split("text with \e[38;5;2mred\e[0m color");
	TEST_CASE("split text with simple ansi control sequence inbetween");
    TEST_ASSERT(tokens.size() == 5);
    TEST_EQUAL(tokens[0], "text with ");
    TEST_EQUAL(tokens[1], "\e[38;5;2m");
    TEST_EQUAL(tokens[2], "red");
    TEST_EQUAL(tokens[3], "\e[0m");
    TEST_EQUAL(tokens[4], " color");

	/* split text with simple ansi control sequence at beginning */
	tokens = Ansi::split("\e[38;5;2mtext with red\e[0m color");
	TEST_CASE("split text with simple ansi control sequence at beginning");
    TEST_ASSERT(tokens.size() == 4);
    TEST_EQUAL(tokens[0], "\e[38;5;2m");
	TEST_EQUAL(tokens[1], "text with red");
    TEST_EQUAL(tokens[2], "\e[0m");
    TEST_EQUAL(tokens[3], " color");

	/* split text with simple ansi control sequence at beginning and end */
	tokens = Ansi::split("\e[38;5;2mtext with red color\e[0m");
	TEST_CASE("split text with simple ansi control sequence at beginning");
    TEST_ASSERT(tokens.size() == 3);
    TEST_EQUAL(tokens[0], "\e[38;5;2m");
	TEST_EQUAL(tokens[1], "text with red color");
    TEST_EQUAL(tokens[2], "\e[0m");

	/* split complex ansi control sequence into simple ansi control sequences */
	TEST_CASE("split complex into simple ansi control sequences");
	tokens = Ansi::split("text \e[0;1;33;43m.");
    TEST_ASSERT(tokens.size() == 6);
	TEST_EQUAL(tokens[0], "text ");
	TEST_EQUAL(tokens[1], "\e[0m");
	TEST_EQUAL(tokens[2], "\e[1m");
	TEST_EQUAL(tokens[3], "\e[33m");
	TEST_EQUAL(tokens[4], "\e[43m");
	TEST_EQUAL(tokens[5], ".");
}

void checkNoSplit(std::string text)
{
	std::vector<std::string> tokens;
	Ansi::subsplit(text, &tokens);
    TEST_ASSERT(tokens.size() == 1);
    TEST_CHECK(tokens[0] == text);
}

void TestAnsi::test_subsplit() 
{
	std::vector<std::string> tokens;
    
	/* don't split text */
	TEST_CASE("don't split text");
    Ansi::subsplit("normal text", &tokens);
    TEST_ASSERT(tokens.size() == 1);
    TEST_CHECK(tokens[0] == "normal text");
	tokens.clear();

	/* don't split text */
	TEST_CASE("don't split simple ansi control sequences");
    checkNoSplit("normal text");
    checkNoSplit("\e[0m");
    checkNoSplit("\e[38;2;255;255;255m");

	/* split complex ansi sequence into two simple ones */
	TEST_CASE("split complex ansi sequence into two simple ones");
	Ansi::subsplit("\e[0;1m", &tokens);
    TEST_ASSERT(tokens.size() == 2);
    TEST_CHECK(tokens[0] == "\e[0m");
    TEST_CHECK(tokens[1] == "\e[1m");
	tokens.clear();

	Ansi::subsplit("\e[38;5;3;0m", &tokens);
    TEST_ASSERT(tokens.size() == 2);
    TEST_CHECK(tokens[0] == "\e[38;5;3m");
    TEST_CHECK(tokens[1] == "\e[0m");
	tokens.clear();
}

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

void TEST_EQUAL(Ansi::Sequence given, Ansi::Sequence expected)
{
    TEST_CHECK_(given == expected, "expected '%d', but was '%d'", as_integer(expected), as_integer(given));
}

void TestAnsi::test_parse_control_sequence()
{
	TEST_EQUAL(Ansi::parseControlSequence("\e[30m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[31m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[32m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[33m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[34m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[35m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[36m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[37m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[38;5;2m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[38;2;255;255;255m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[90m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[91m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[92m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[93m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[94m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[95m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[96m"), Ansi::Sequence::FOREGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[97m"), Ansi::Sequence::FOREGROUND_COLOR);

	TEST_EQUAL(Ansi::parseControlSequence("\e[40m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[41m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[42m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[43m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[44m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[45m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[46m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[47m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[48;5;2m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[48;2;255;255;255m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[100m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[101m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[102m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[103m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[104m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[105m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[106m"), Ansi::Sequence::BACKGROUND_COLOR);
	TEST_EQUAL(Ansi::parseControlSequence("\e[107m"), Ansi::Sequence::BACKGROUND_COLOR);

	TEST_EQUAL(Ansi::parseControlSequence("\e[1m"), Ansi::Sequence::INCREASE_INTENSITY);

	TEST_EQUAL(Ansi::parseControlSequence("\e[0m"), Ansi::Sequence::RESET);
	TEST_EQUAL(Ansi::parseControlSequence("\e[0;0m"), Ansi::Sequence::RESET);
	TEST_EQUAL(Ansi::parseControlSequence("\e[39m"), Ansi::Sequence::RESET_FOREGROUND);
	TEST_EQUAL(Ansi::parseControlSequence("\e[49m"), Ansi::Sequence::RESET_BACKGROUND);

	TEST_EQUAL(Ansi::parseControlSequence(""), Ansi::Sequence::NONE);
	TEST_EQUAL(Ansi::parseControlSequence("text"), Ansi::Sequence::NONE);
	TEST_EQUAL(Ansi::parseControlSequence("\e[m"), Ansi::Sequence::NONE);
	TEST_EQUAL(Ansi::parseControlSequence("\e[38;5;2"), Ansi::Sequence::NONE);
	TEST_EQUAL(Ansi::parseControlSequence("e[38;5;2m"), Ansi::Sequence::NONE);

	TEST_EQUAL(Ansi::parseControlSequence("\e[8m"), Ansi::Sequence::UNKNOWN);
	TEST_EQUAL(Ansi::parseControlSequence("\e[88m"), Ansi::Sequence::UNKNOWN);
	TEST_EQUAL(Ansi::parseControlSequence("\e[888m"), Ansi::Sequence::UNKNOWN);
	TEST_EQUAL(Ansi::parseControlSequence("\e[888888888888888888888888m"), Ansi::Sequence::UNKNOWN);
}