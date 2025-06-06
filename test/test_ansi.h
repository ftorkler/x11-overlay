#ifndef OVERLAY_TEST_ANSI_H
#define OVERLAY_TEST_ANSI_H

class TestAnsi
{

public:

    static void test_split();
    static void test_subsplit();
    static void test_parseControlSequence();
	static void test_toColor();
	static void test_to8bitColor();
	static void test_toFontIndex();

};

#endif // OVERLAY_TEST_ANSI_H
