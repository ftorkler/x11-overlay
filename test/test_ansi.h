#ifndef OVERLAY_TEST_ANSI_H
#define OVERLAY_TEST_ANSI_H

class TestAnsi
{

public:

    static void test_split();
    static void test_subsplit();
    static void test_parse_control_sequence();
	static void test_to_8bit_color();
	static void test_to_color();

};

#endif // OVERLAY_TEST_ANSI_H
