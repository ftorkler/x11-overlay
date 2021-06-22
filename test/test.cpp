#include "acutest.h"

#include "test_ansi.h"

TEST_LIST = {
   { "should return correct color for 8bit code (256 colors)", &TestAnsi::test_to_8bit_color },
   { "should return correct color for ansi code (256 colors)", &TestAnsi::test_to_color },
   { "should return splitted text", &TestAnsi::test_split },
   { NULL, NULL } /* zeroed record marking the end of the list */
};
