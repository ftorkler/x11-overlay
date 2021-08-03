#include "acutest.h"

#include "test_ansi.h"
#include "test_config.h"

TEST_LIST = {
   { "should return correct color for 8bit code (256 colors)", &TestAnsi::test_to_8bit_color },
   { "should return correct color for ansi code (256 colors)", &TestAnsi::test_to_color },
   { "should return splitted text", &TestAnsi::test_split },
   { "should return splitted ansi control sequences", &TestAnsi::test_subsplit },
   { "should return whether text is an ansi control sequence", &TestAnsi::test_parse_control_sequence },

   { "should parse comment in config line", &TestConfig::test_parseCommentLine },
   { "should parse section in config line", &TestConfig::test_parseSectionLine },
   { "should parse key-value pair in config line (integer)", &TestConfig::test_parseKeyValueLine_forInteger },
   { "should parse key-value pair in config line (string)", &TestConfig::test_parseKeyValueLine_forString },
   { "should parse key-value pair in config line (orientation)", &TestConfig::test_parseKeyValueLine_forOrientation },
   { "should parse config from file", &TestConfig::test_fromFile },
   
   { NULL, NULL } /* zeroed record marking the end of the list */
};
