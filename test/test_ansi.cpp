#include "test_ansi.h"

#include "acutest.h"
#include <string>
#include <vector>

#include "ansi.h"
#include "color.h"

namespace
{
	const Color FALLBACK_COLOR = Color(162, 198, 182, 163); // random fallback color

    void TEST_EQUAL(const std::string& given, const std::string& expected)
    {
        TEST_CHECK_(given == expected, "expected '%s', but was '%s'",
        	Ansi::toPrintable(expected).c_str(),
        	Ansi::toPrintable(given).c_str());
    }

    void TEST_ASSERT_EQUAL(int given, int expected)
    {
        TEST_ASSERT_(given == expected, "expected '%d', but was '%d'", expected, given);
    }

	void check_to_8bit_color(int givenCode, Color expectedColor)
	{
		Color color = Ansi::_to8bitColor(givenCode, FALLBACK_COLOR);
		TEST_CHECK_(color == expectedColor, "fromAnsi8bit(%d) was (%d,%d,%d,%d), but expected to be (%d,%d,%d,%d)",
			givenCode,
			color.r, color.g, color.b, color.a,
			expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a);
	}

	void check_to_color(const std::string& givenCode, Color expectedColor, bool increaseIntensity = false)
	{
		Color color = Ansi::toColor(givenCode, FALLBACK_COLOR, increaseIntensity);
		TEST_CHECK_(color == expectedColor, "fromAnsi('%s') was (%d,%d,%d,%d), but expected to be (%d,%d,%d,%d)",
			Ansi::toPrintable(givenCode).c_str(),
			color.r, color.g, color.b, color.a,
			expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a);
	}

	void check_to_font_index(const std::string& givenCode, unsigned int expectedIndex)
	{
	    unsigned int index = Ansi::toFontIndex(givenCode);
	    TEST_CHECK_(index == expectedIndex, "toFontIndex('%s') was '%d', but expected to be '%d'", Ansi::toPrintable(givenCode).c_str(), index, expectedIndex);
	}

	void check_ansi_sequence(const std::string& givenText, Ansi::Sequence expectedSequence)
	{
		Ansi::Sequence ansiSequence = Ansi::parseControlSequence(givenText);
		TEST_CHECK_(ansiSequence == expectedSequence, "parseControlSequence('%s') was '%d', but expected to be '%d'", Ansi::toPrintable(givenText).c_str(), ansiSequence, expectedSequence);
	}

	void check_no_split(const std::string& givenText)
    {
    	std::vector<std::string> tokens;
    	Ansi::subsplit(givenText, &tokens);
    	TEST_ASSERT(!tokens.empty());
    	TEST_EQUAL(tokens[0], givenText);
    	TEST_ASSERT_EQUAL(tokens.size(), 1);
    }

	std::vector<std::string> provideFuzzyInput_NoneSequence()
    {
    	return {
    		"",
			"  ",
			"m",
			"text",
			"e[38;5;2m",
			"[38;0m",
			"\em",
			"\e[",
			"\e]",
			"\e0;0m",
			"\e[38;5;2",
			"\e[11m1"
		};
    }

	std::vector<std::string> provideFuzzyInput_UnknownSequence()
    {
    	return {
    		"\e[m",
			"\e[8m",
			"\e[88m",
			"\e[888m",
			"\e[888888888888888888888888m",
			"\e[\rm",
			"\e[\r\rm",
			"\e[ m",
			"\e[Am",
			"\e[zm",
			"\e[xym",
			"\e[99m",
			"\e[XYZm"
		};
    }

	std::vector<std::string> provideFuzzyInput()
    {
    	std::vector<std::string> result;
    	std::vector<std::string> a = provideFuzzyInput_NoneSequence();
    	std::vector<std::string> b = provideFuzzyInput_UnknownSequence();
    	result.insert(result.end(), a.begin(), a.end());
    	result.insert(result.end(), b.begin(), b.end());
    	return result;
    }
}

void TestAnsi::test_toColor()
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
	TEST_CASE("8bit - regular in range");
	check_to_color("\e[38;5;2m", Color(0, 128, 0));
	check_to_color("\e[38;5;23m", Color(0, 95, 95));
	check_to_color("\e[38;5;234m", Color(28, 28, 28));
	check_to_color("\e[48;5;2m", Color(0, 128, 0));
	check_to_color("\e[48;5;23m", Color(0, 95, 95));
	check_to_color("\e[48;5;234m", Color(28, 28, 28));

	TEST_CASE("8bit - use fallback color when out of range");
	check_to_color("\e[38;5;256m", FALLBACK_COLOR);
	check_to_color("\e[38;5;-1m", FALLBACK_COLOR);

	TEST_CASE("8bit - use fallback color when code is empty");
	check_to_color("\e[38;5;m", FALLBACK_COLOR);

	TEST_CASE("8bit - use fallback color when code is missing");
	check_to_color("\e[38;5m", FALLBACK_COLOR);

	// 24bit
	TEST_CASE("24bit - regular in range");
	check_to_color("\e[38;2;1;12;123m", Color(1, 12, 123));
	check_to_color("\e[48;2;1;12;123m", Color(1, 12, 123));
	check_to_color("\e[38;2;0;0;255m", Color(0, 0, 255));
	check_to_color("\e[38;2;0;0;0m", Color(0, 0, 0));
	check_to_color("\e[48;2;0;0;255m", Color(0, 0, 255));
	check_to_color("\e[48;2;0;0;0m", Color(0, 0, 0));

	TEST_CASE("24bit - use fallback color when out of range");
	check_to_color("\e[38;2;0;0;256m", FALLBACK_COLOR);
	check_to_color("\e[38;2;0;0;-1m", FALLBACK_COLOR);
	check_to_color("\e[48;2;0;0;256m", FALLBACK_COLOR);
	check_to_color("\e[48;2;0;0;-1m", FALLBACK_COLOR);

	TEST_CASE("24bit - empty code is treated as '0'");
	check_to_color("\e[38;2;;;m", Color(0, 0, 0));
	check_to_color("\e[38;2;255;;m", Color(255, 0, 0));
	check_to_color("\e[38;2;;255;m", Color(0, 255, 0));
	check_to_color("\e[38;2;;;255m", Color(0, 0, 255));
	check_to_color("\e[48;2;;;m", Color(0, 0, 0));
	check_to_color("\e[48;2;255;;m", Color(255, 0, 0));
	check_to_color("\e[48;2;;255;m", Color(0, 255, 0));
	check_to_color("\e[48;2;;;255m", Color(0, 0, 255));

	TEST_CASE("24bit - ignore additional codes");
	check_to_color("\e[38;2;1;128;12;1m", Color(1, 128, 12));
	check_to_color("\e[38;2;0;128;12;1m", Color(0, 128, 12));
	check_to_color("\e[38;2;;128;12;1m", Color(0, 128, 12));
	check_to_color("\e[48;2;1;128;12;1m", Color(1, 128, 12));
	check_to_color("\e[48;2;0;128;12;1m", Color(0, 128, 12));
	check_to_color("\e[48;2;;128;12;1m", Color(0, 128, 12));

	TEST_CASE("24bit - use fallback color when missing code(s)");
	check_to_color("\e[38;2;111;222m", FALLBACK_COLOR);
	check_to_color("\e[38;2;111m", FALLBACK_COLOR);
	check_to_color("\e[38;2m", FALLBACK_COLOR);
	check_to_color("\e[48;2;111;222m", FALLBACK_COLOR);
	check_to_color("\e[48;2;111m", FALLBACK_COLOR);
	check_to_color("\e[48;2m", FALLBACK_COLOR);

	// fuzzy
	TEST_CASE("general - use fallback color when sequence is invalid - fuzzy testing");
	for (const auto& input : provideFuzzyInput()) {
		check_to_color(input, FALLBACK_COLOR);
	}
}

void TestAnsi::test_to8bitColor()
{
	// for code not range [0..255] (fallback)
	check_to_8bit_color(-1, FALLBACK_COLOR);
	check_to_8bit_color(256, FALLBACK_COLOR);

	// for some random codes in range [0..15] (palette)
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

void TestAnsi::test_toFontIndex()
{
    TEST_CASE("handle all possible font selections");
    check_to_font_index("\e[10m", 0);
    check_to_font_index("\e[11m", 1);
    check_to_font_index("\e[12m", 2);
    check_to_font_index("\e[13m", 3);
    check_to_font_index("\e[14m", 4);
    check_to_font_index("\e[15m", 5);
    check_to_font_index("\e[16m", 6);
    check_to_font_index("\e[17m", 7);
    check_to_font_index("\e[18m", 8);
    check_to_font_index("\e[19m", 9);

    TEST_CASE("handle invalid font selection range");
    check_to_font_index("\e[9m",  0);
    check_to_font_index("\e[09m", 0);
    check_to_font_index("\e[20m", 0);
    check_to_font_index("\e[100m", 0);

	TEST_CASE("handle invalid font selection format - fuzzy testing");
	for (const auto& input : provideFuzzyInput()) {
		check_to_font_index(input, 0);
	}
}

void TestAnsi::test_split()
{
    std::vector<std::string> tokens;

    /* handle single char */
    TEST_CASE("handle single char");
    tokens = Ansi::split("a");
    TEST_ASSERT_EQUAL(tokens.size(), 1);
    TEST_EQUAL(tokens[0], "a");

    /* handle single special char */
    TEST_CASE("handle single special char");
    tokens = Ansi::split("\e");
    TEST_ASSERT_EQUAL(tokens.size(), 1);
    TEST_EQUAL(tokens[0], "\e");

	/* don't split text */
	TEST_CASE("don't split text");
    tokens = Ansi::split("text without colors");
    TEST_ASSERT_EQUAL(tokens.size(), 1);
    TEST_CHECK(tokens[0] == "text without colors");

	/* split text with simple ansi control sequence inbetween */
	TEST_CASE("split text with simple ansi control sequence inbetween");
    tokens = Ansi::split("text with \e[38;5;2mred\e[0m color");
    TEST_ASSERT_EQUAL(tokens.size(), 5);
    TEST_EQUAL(tokens[0], "text with ");
    TEST_EQUAL(tokens[1], "\e[38;5;2m");
    TEST_EQUAL(tokens[2], "red");
    TEST_EQUAL(tokens[3], "\e[0m");
    TEST_EQUAL(tokens[4], " color");

	/* split text with simple ansi control sequence at beginning */
	TEST_CASE("split text with simple ansi control sequence at beginning");
	tokens = Ansi::split("\e[38;5;2mtext with red\e[0m color");
    TEST_ASSERT_EQUAL(tokens.size(), 4);
    TEST_EQUAL(tokens[0], "\e[38;5;2m");
	TEST_EQUAL(tokens[1], "text with red");
    TEST_EQUAL(tokens[2], "\e[0m");
    TEST_EQUAL(tokens[3], " color");

	/* split text with simple ansi control sequence at beginning and end */
	TEST_CASE("split text with simple ansi control sequence at beginning");
	tokens = Ansi::split("\e[38;5;2mtext with red color\e[0m");
    TEST_ASSERT_EQUAL(tokens.size(), 3);
    TEST_EQUAL(tokens[0], "\e[38;5;2m");
	TEST_EQUAL(tokens[1], "text with red color");
    TEST_EQUAL(tokens[2], "\e[0m");

	/* split complex ansi control sequence into simple ansi control sequences */
	TEST_CASE("split complex into simple ansi control sequences");
	tokens = Ansi::split("text \e[0;1;33;43m.");
    TEST_ASSERT_EQUAL(tokens.size(), 6);
	TEST_EQUAL(tokens[0], "text ");
	TEST_EQUAL(tokens[1], "\e[0m");
	TEST_EQUAL(tokens[2], "\e[1m");
	TEST_EQUAL(tokens[3], "\e[33m");
	TEST_EQUAL(tokens[4], "\e[43m");
	TEST_EQUAL(tokens[5], ".");

	/* split ansi control sequences containing Operating System Command (OSC) and String Terminator (ST) */
	TEST_CASE("split ansi control sequences containing Operating System Command (OSC) and String Terminator (ST)");
	tokens = Ansi::split("\e]8;;http://example.com\e\\This is a link\e]8;;\e\\");
	TEST_EQUAL(tokens[0], "\e]8;;http://example.com\e\\This is a link\e]8;;\e\\");

	/* split mixed ansi control sequences containing Operating System Command (OSC) and String Terminator (ST) */
	TEST_CASE("split mixed ansi control sequences containing terminal hyperlinks (OSC 8)");
	tokens = Ansi::split("Link \e[0;94mExample:\e[0m \e]8;;http://example.com\e\\This is a link\e]8;;\e\\ \e[0;94mEnd\e[0m");
	TEST_EQUAL(tokens[0], "Link ");
	TEST_EQUAL(tokens[1], "\e[0m");
	TEST_EQUAL(tokens[2], "\e[94m");
	TEST_EQUAL(tokens[3], "Example:");
	TEST_EQUAL(tokens[4], "\e[0m");
	TEST_EQUAL(tokens[5], " \e]8;;http://example.com\e\\This is a link\e]8;;\e\\ ");
	TEST_EQUAL(tokens[6], "\e[0m");
	TEST_EQUAL(tokens[7], "\e[94m");
	TEST_EQUAL(tokens[8], "End");
	TEST_EQUAL(tokens[9], "\e[0m");
}

void TestAnsi::test_subsplit()
{
	std::vector<std::string> tokens;

	/* don't split text */
	TEST_CASE("don't split text");
    Ansi::subsplit("normal text", &tokens);
    TEST_ASSERT_EQUAL(tokens.size(), 1);
    TEST_CHECK(tokens[0] == "normal text");
	tokens.clear();

	/* don't split text */
	TEST_CASE("don't split simple ansi control sequences");
    check_no_split("normal text");
    check_no_split("\e[0m");
    check_no_split("\e[38;2;255;255;255m");

	TEST_CASE("do not split malformed ansi sequence - fuzzy");
	for (auto input : provideFuzzyInput()) {
		check_no_split(input);
	}

	/* split complex ansi sequence into two simple ones */
	TEST_CASE("split complex ansi sequence into two simple ones");
	Ansi::subsplit("\e[0;1m", &tokens);
    TEST_ASSERT_EQUAL(tokens.size(), 2);
    TEST_EQUAL(tokens[0], "\e[0m");
    TEST_EQUAL(tokens[1], "\e[1m");
	tokens.clear();

	Ansi::subsplit("\e[38;5;3;0m", &tokens);
    TEST_ASSERT_EQUAL(tokens.size(), 2);
    TEST_EQUAL(tokens[0], "\e[38;5;3m");
    TEST_EQUAL(tokens[1], "\e[0m");
	tokens.clear();

	Ansi::subsplit("\e[99;123m", &tokens);
	TEST_ASSERT_EQUAL(tokens.size(), 2);
	TEST_EQUAL(tokens[0], "\e[99m");
	TEST_EQUAL(tokens[1], "\e[123m");
	tokens.clear();

	Ansi::subsplit("\e[ABC;XYZm", &tokens);
	TEST_ASSERT_EQUAL(tokens.size(), 2);
	TEST_EQUAL(tokens[0], "\e[ABCm");
	TEST_EQUAL(tokens[1], "\e[XYZm");
	tokens.clear();
}

void TestAnsi::test_parseControlSequence()
{
	check_ansi_sequence("\e[30m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[31m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[32m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[33m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[34m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[35m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[36m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[37m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[38;5;2m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[38;2;255;255;255m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[90m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[91m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[92m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[93m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[94m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[95m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[96m", Ansi::Sequence::FOREGROUND_COLOR);
	check_ansi_sequence("\e[97m", Ansi::Sequence::FOREGROUND_COLOR);

	check_ansi_sequence("\e[40m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[41m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[42m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[43m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[44m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[45m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[46m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[47m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[48;5;2m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[48;2;255;255;255m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[100m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[101m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[102m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[103m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[104m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[105m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[106m", Ansi::Sequence::BACKGROUND_COLOR);
	check_ansi_sequence("\e[107m", Ansi::Sequence::BACKGROUND_COLOR);

	check_ansi_sequence("\e[1m", Ansi::Sequence::INCREASE_INTENSITY);

	check_ansi_sequence("\e[0m", Ansi::Sequence::RESET);
	check_ansi_sequence("\e[0;0m", Ansi::Sequence::RESET);
	check_ansi_sequence("\e[39m", Ansi::Sequence::RESET_FOREGROUND);
	check_ansi_sequence("\e[49m", Ansi::Sequence::RESET_BACKGROUND);

	for (const auto& input : provideFuzzyInput_NoneSequence()) {
		check_ansi_sequence(input, Ansi::Sequence::NONE);
	}

	for (const auto& input : provideFuzzyInput_UnknownSequence()) {
		check_ansi_sequence(input, Ansi::Sequence::UNKNOWN);
	}
}
