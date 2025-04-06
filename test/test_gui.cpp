#include "test_gui.h"

#include "acutest.h"
#include <algorithm>
#include <string>

#include "gui.h"

namespace
{
    void TEST_TRIM(const Gui* gui, std::string given, std::string expected)
    {
        std::string result = gui->trimForOrientation(gui->trimLinefeedsAndApplyTabs(given));
        std::replace(given.begin(), given.end(), '\r', '~');
        std::replace(given.begin(), given.end(), '\n', '^');
        std::replace(given.begin(), given.end(), '\t', '>');
        TEST_CHECK_(result == expected, "given '%s': expected '%s', but was '%s'", given.c_str(), expected.c_str(), result.c_str());
    }
}

void TestGui::test_trimForOrientation()
{
    Gui gui = Gui();

    // Original: "<L>TEXT<R>"  (<L/R> indentation)
    // West:     "<L>TEXT<R>"
    // Center:   "TEXT"
    // East:     "<R>TEXT<L>"

    TEST_CASE("trim nothing for orientation W");
    gui.setOrientation(Gui::Orientation::W);
    TEST_TRIM(&gui, "TEXT",      "TEXT");
    TEST_TRIM(&gui, "T",         "T");
    TEST_TRIM(&gui, "TEXT    ",  "TEXT    ");
    TEST_TRIM(&gui, " TEXT    ", " TEXT    ");
    TEST_TRIM(&gui, " TEXT",     " TEXT");
    TEST_TRIM(&gui, " ",         " ");

    TEST_CASE("trim left and right for orientation CENTER");
    gui.setOrientation(Gui::Orientation::CENTER);
    TEST_TRIM(&gui, "TEXT",      "TEXT");
    TEST_TRIM(&gui, "T",         "T");
    TEST_TRIM(&gui, "TEXT    ",  "TEXT");
    TEST_TRIM(&gui, " TEXT    ", "TEXT");
    TEST_TRIM(&gui, " TEXT",     "TEXT");
    TEST_TRIM(&gui, " ",         " ");

    TEST_CASE("trim by swapping left and right for orientation E");
    gui.setOrientation(Gui::Orientation::E);
    TEST_TRIM(&gui, "TEXT",      "TEXT");
    TEST_TRIM(&gui, "T",         "T");
    TEST_TRIM(&gui, " ",         " ");
    TEST_TRIM(&gui, "TEXT    ",  "    TEXT");
    TEST_TRIM(&gui, " TEXT    ", "    TEXT ");
    TEST_TRIM(&gui, " TEXT",     "TEXT ");
}

void TestGui::test_trimLinefeedsAndApplyTabs()
{
    Gui gui = Gui();
    gui.setOrientation(Gui::Orientation::W);

    TEST_CASE("trim carriage return (CR) and line feed (LF)");
    TEST_TRIM(&gui, "TEXT",    "TEXT");
    TEST_TRIM(&gui, "TEXT\r",   "TEXT");
    TEST_TRIM(&gui, "TEXT\n",   "TEXT");
    TEST_TRIM(&gui, "TEXT\r\n", "TEXT");

    TEST_CASE("indent tabulator (HT) with spaces");
    TEST_TRIM(&gui, "\tTEXT",         "    TEXT");
    TEST_TRIM(&gui, "\tTEXT\r\n",     "    TEXT");
    TEST_TRIM(&gui, " \tTEXT\r\n",    "    TEXT");
    TEST_TRIM(&gui, "  \tTEXT\r\n",   "    TEXT");
    TEST_TRIM(&gui, "   \tTEXT\r\n",  "    TEXT");
    TEST_TRIM(&gui, "    \tTEXT\r\n", "        TEXT");
    TEST_TRIM(&gui, "\t\tTEXT\r\n",   "        TEXT");
    TEST_TRIM(&gui, "  \t\tTEXT\r\n", "        TEXT");
}
