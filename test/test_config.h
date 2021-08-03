#ifndef OVERLAY_TEST_CONFIG_H
#define OVERLAY_TEST_CONFIG_H

#include <string>

class TestConfig
{

public:

    static void test_parseCommentLine();
    static void test_parseSectionLine();
    static void test_parseKeyValueLine_forInteger();
    static void test_parseKeyValueLine_forString();
    static void test_parseKeyValueLine_forOrientation();
    static void test_fromFile();

private:

    static std::string getExecutablePath();
    static std::string getExecutableDir();
    
};

#endif // OVERLAY_TEST_CONFIG_H
