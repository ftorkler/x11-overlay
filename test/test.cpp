#include "acutest.h"

#include "test_ansi.h"
#include "test_config.h"
#include "test_gui.h"

TEST_LIST = {
   { "should return correct color for ansi code (256 colors)", &TestAnsi::test_toColor },
   { "should return correct color for 8bit code (256 colors)", &TestAnsi::test_to8bitColor },
   { "should return correct font index for ansi code", &TestAnsi::test_toFontIndex },
   { "should return splitted text", &TestAnsi::test_split },
   { "should return splitted ansi control sequences", &TestAnsi::test_subsplit },
   { "should return whether text is an ansi control sequence", &TestAnsi::test_parseControlSequence },

   { "should parse comment in config line", &TestConfig::test_parseCommentLine },
   { "should parse section in config line", &TestConfig::test_parseSectionLine },
   { "should parse key-value pair in config line (integer)", &TestConfig::test_parseKeyValueLine_forInteger },
   { "should parse key-value pair in config line (string)", &TestConfig::test_parseKeyValueLine_forString },
   { "should parse key-value pair in config line (orientation)", &TestConfig::test_parseKeyValueLine_forOrientation },
   { "should parse config from file", &TestConfig::test_fromFile },

   { "should return trim for orientation", &TestGui::test_trimForOrientation },
   { "should return trim for removed linefeeds and applied tabs", &TestGui::test_trimLinefeedsAndApplyTabs },

   { nullptr, nullptr } /* zeroed record marking the end of the list */
};
