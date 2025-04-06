#include "test_gui.h"

#include "acutest.h"
#include <algorithm>
#include <string>

#include "gui.h"

namespace
{
    void TEST_TRIM(const Gui::Orientation orientation, std::string given, std::string expected)
    {
        std::string result = Gui::trimForOrientation(orientation, Gui::trimLinefeedsAndApplyTabs(given));
        std::replace(given.begin(), given.end(), '\r', '~');
        std::replace(given.begin(), given.end(), '\n', '^');
        std::replace(given.begin(), given.end(), '\t', '>');
        TEST_CHECK_(result == expected, "given '%s': expected '%s', but was '%s'", given.c_str(), expected.c_str(), result.c_str());
    }
}

void TestGui::test_trimForOrientation()
{
    // Original: "<L>TEXT<R>"  (<L/R> indentation)
    // West:     "<L>TEXT<R>"
    // Center:   "TEXT"
    // East:     "<R>TEXT<L>"

    TEST_CASE("trim nothing for orientation W");
    TEST_TRIM(Gui::Orientation::W, "TEXT",      "TEXT");
    TEST_TRIM(Gui::Orientation::W, "T",         "T");
    TEST_TRIM(Gui::Orientation::W, "TEXT    ",  "TEXT    ");
    TEST_TRIM(Gui::Orientation::W, " TEXT    ", " TEXT    ");
    TEST_TRIM(Gui::Orientation::W, " TEXT",     " TEXT");
    TEST_TRIM(Gui::Orientation::W, " ",         " ");

    TEST_CASE("trim left and right for orientation CENTER");
    TEST_TRIM(Gui::Orientation::CENTER, "TEXT",      "TEXT");
    TEST_TRIM(Gui::Orientation::CENTER, "T",         "T");
    TEST_TRIM(Gui::Orientation::CENTER, "TEXT    ",  "TEXT");
    TEST_TRIM(Gui::Orientation::CENTER, " TEXT    ", "TEXT");
    TEST_TRIM(Gui::Orientation::CENTER, " TEXT",     "TEXT");
    TEST_TRIM(Gui::Orientation::CENTER, " ",         " ");

    TEST_CASE("trim by swapping left and right for orientation E");
    TEST_TRIM(Gui::Orientation::E, "TEXT",      "TEXT");
    TEST_TRIM(Gui::Orientation::E, "T",         "T");
    TEST_TRIM(Gui::Orientation::E, " ",         " ");
    TEST_TRIM(Gui::Orientation::E, "TEXT    ",  "    TEXT");
    TEST_TRIM(Gui::Orientation::E, " TEXT    ", "    TEXT ");
    TEST_TRIM(Gui::Orientation::E, " TEXT",     "TEXT ");
}

void TestGui::test_trimLinefeedsAndApplyTabs()
{
    TEST_CASE("trim carriage return (CR) and line feed (LF)");
    TEST_TRIM(Gui::Orientation::W, "TEXT",    "TEXT");
    TEST_TRIM(Gui::Orientation::W, "TEXT\r",   "TEXT");
    TEST_TRIM(Gui::Orientation::W, "TEXT\n",   "TEXT");
    TEST_TRIM(Gui::Orientation::W, "TEXT\r\n", "TEXT");

    TEST_CASE("indent tabulator (HT) with spaces");
    TEST_TRIM(Gui::Orientation::W, "\tTEXT",         "    TEXT");
    TEST_TRIM(Gui::Orientation::W, "\tTEXT\r\n",     "    TEXT");
    TEST_TRIM(Gui::Orientation::W, " \tTEXT\r\n",    "    TEXT");
    TEST_TRIM(Gui::Orientation::W, "  \tTEXT\r\n",   "    TEXT");
    TEST_TRIM(Gui::Orientation::W, "   \tTEXT\r\n",  "    TEXT");
    TEST_TRIM(Gui::Orientation::W, "    \tTEXT\r\n", "        TEXT");
    TEST_TRIM(Gui::Orientation::W, "\t\tTEXT\r\n",   "        TEXT");
    TEST_TRIM(Gui::Orientation::W, "  \t\tTEXT\r\n", "        TEXT");
}
