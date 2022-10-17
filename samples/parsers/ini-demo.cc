#include <gtest/gtest.h>
#include "misc/ini.h"

TEST(TestBugFix, TestEmptySection)
{
    parsers::INIParser ini;
    ini.SetValue("foo", "skey", "sval");
    ini.SetValue("", "rkey", "rval");
    ini.SetValue("bar", "skey", "sval");

    std::string output;
    ini.Dump(output);

    std::string expected = "rkey = rval\n"
                           "\n"
                           "\n"
                           "[foo]\n"
                           "skey = sval\n"
                           "\n"
                           "\n"
                           "[bar]\n"
                           "skey = sval\n";

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
    ASSERT_STREQ(expected.c_str(), output.c_str());
}

TEST(TestBugFix, TestMultiLineIgnoreTrailSpace0)
{
    parsers::INIParser ini(true, false, true);

    std::string input = "; multiline values\n"
                        "key = <<<EOS\n"
                        "This is a\n"
                        "multiline value\n"
                        "and it ends.\n"
                        "EOS\n"
                        "\n"
                        "[section]\n";
    ASSERT_EQ(ini.LoadText(input), 0);

    std::string output;
    ini.Dump(output);

    std::string expected = "; multiline values\n"
                           "\n"
                           "\n"
                           "key = <<<END_OF_TEXT\n"
                           "This is a\n"
                           "multiline value\n"
                           "and it ends.\n"
                           "END_OF_TEXT\n"
                           "\n"
                           "\n"
                           "[section]\n";

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
    ASSERT_STREQ(expected.c_str(), output.c_str());
}

TEST(TestBugFix, TestMultiLineIgnoreTrailSpace1)
{
    std::string input = "; multiline values\n"
                        "key = <<<EOS\n"
                        "This is a\n"
                        "multiline value\n"
                        "and it ends.\n"
                        "EOS \n"
                        "\n"
                        "[section]\n";

    bool multiline = true;
    parsers::INIParser ini(true, false, multiline);

    auto rc = ini.LoadText(input);
    ASSERT_EQ(rc, parsers::INI_OK);

    std::string output;
    ini.Dump(output);

    std::string expected = "; multiline values\n"
                           "\n"
                           "\n"
                           "key = <<<END_OF_TEXT\n"
                           "This is a\n"
                           "multiline value\n"
                           "and it ends.\n"
                           "END_OF_TEXT\n"
                           "\n"
                           "\n"
                           "[section]\n";

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
    ASSERT_STREQ(expected.c_str(), output.c_str());
}

TEST(TestBugFix, TestMultiLineIgnoreTrailSpace2)
{
    std::string input = "; multiline values\n"
                        "key = <<<EOS\n"
                        "This is a\n"
                        "multiline value\n"
                        "and it ends.\n"
                        "EOS  \n"
                        "\n"
                        "[section]\n";

    bool multiline = true;
    parsers::INIParser ini(true, false, multiline);

    auto rc = ini.LoadText(input);
    ASSERT_EQ(rc, parsers::INI_OK);

    std::string output;
    ini.Dump(output);

    std::string expected = "; multiline values\n"
                           "\n"
                           "\n"
                           "key = <<<END_OF_TEXT\n"
                           "This is a\n"
                           "multiline value\n"
                           "and it ends.\n"
                           "END_OF_TEXT\n"
                           "\n"
                           "\n"
                           "[section]\n";

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
    ASSERT_STREQ(expected.c_str(), output.c_str());
}

class TestRoundTrip : public ::testing::Test {
  protected:
    void SetUp() override;
    void TestMulti();
    void TestBOM(bool useBOM);

  protected:
    parsers::INIParser ini;
    std::string input;
    std::string output;
};

void TestRoundTrip::SetUp()
{
    ini.SetUnicode();
}

TEST_F(TestRoundTrip, TestStandard)
{
    input = "; File comment\n"
            "\n"
            "\n"
            "; Section 1 comment\n"
            "[section1]\n"
            "\n"
            "\n"
            "; Section 2 comment\n"
            "[section2]\n"
            "key1 = string\n"
            "key2 = true\n"
            "key3 = 3.1415\n";

    auto rc = ini.LoadText(input);
    ASSERT_EQ(rc, parsers::INI_OK);

    const char *result = ini.GetValue("section2", "key1");
    ASSERT_STREQ(result, "string");

    rc = ini.Dump(output);
    ASSERT_EQ(rc, parsers::INI_OK);

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
    ASSERT_STREQ(input.c_str(), output.c_str());
}

void TestRoundTrip::TestMulti()
{
    input = "[section]\n"
            "key = string1\n"
            "key = string2\n";

    auto rc = ini.LoadText(input);
    ASSERT_EQ(rc, parsers::INI_OK);

    rc = ini.Dump(output);
    ASSERT_EQ(rc, parsers::INI_OK);

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
}

TEST_F(TestRoundTrip, TestMultiGood)
{
    ini.SetMultiKey(true);
    TestMulti();
    ASSERT_STREQ(input.c_str(), output.c_str());
}

TEST_F(TestRoundTrip, TestMultiBad)
{
    std::string expected = "[section]\n"
                           "key = string2\n";

    ini.SetMultiKey(false);
    TestMulti();
    ASSERT_STRNE(input.c_str(), output.c_str());
    ASSERT_STREQ(expected.c_str(), output.c_str());
}

TEST_F(TestRoundTrip, TestSpacesTrue)
{
    input = "[section]\n"
            "key = string1\n";

    auto rc = ini.LoadText(input);
    ASSERT_EQ(rc, parsers::INI_OK);

    ini.SetSpaces(true);
    rc = ini.Dump(output);
    ASSERT_EQ(rc, parsers::INI_OK);

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());

    ASSERT_STREQ(input.c_str(), output.c_str());
}

TEST_F(TestRoundTrip, TestSpacesFalse)
{
    input = "[section]\n"
            "key = string1\n";

    auto rc = ini.LoadText(input);
    ASSERT_EQ(rc, parsers::INI_OK);

    ini.SetSpaces(false);
    rc = ini.Dump(output);
    ASSERT_EQ(rc, parsers::INI_OK);

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());

    ASSERT_STRNE(input.c_str(), output.c_str());

    std::string expected = "[section]\n"
                           "key=string1\n";

    ASSERT_STREQ(expected.c_str(), output.c_str());
}

void TestRoundTrip::TestBOM(bool useBOM)
{
    const char bom[] = "\xEF\xBB\xBF";
    const char input8[] = u8"[テスト1]\n"
                          u8"テスト2 = テスト3\n";

    input = bom;
    input += input8;

    ini.Reset();
    ini.SetUnicode(false);
    auto rc = ini.LoadText(input);
    ASSERT_EQ(rc, parsers::INI_OK);

    const char tesuto1[] = u8"テスト1";
    const char tesuto2[] = u8"テスト2";
    const char tesuto3[] = u8"テスト3";

    const char *result = ini.GetValue(tesuto1, tesuto2);
    ASSERT_STREQ(result, tesuto3);

    rc = ini.Dump(output, useBOM);
    ASSERT_EQ(rc, parsers::INI_OK);

    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
}

TEST_F(TestRoundTrip, TestWithBOM)
{
    TestBOM(true);

    ASSERT_STREQ(input.c_str(), output.c_str());
}

TEST_F(TestRoundTrip, TestWithoutBOM)
{
    TestBOM(false);

    ASSERT_STRNE(input.c_str(), output.c_str());

    std::string expected(input, 3);
    ASSERT_STREQ(expected.c_str(), output.c_str());
}

class TestUTF8 : public ::testing::Test {
  protected:
    void SetUp() override;

  protected:
    parsers::INIParser ini;
};

void TestUTF8::SetUp()
{
    ini.SetUnicode();
    auto err = ini.LoadFile("tests.ini");
    ASSERT_EQ(err, parsers::INI_OK);
}

TEST_F(TestUTF8, TestSectionAKeyAValA)
{
    const char *result = ini.GetValue("section1", "key1");
    ASSERT_STREQ(result, "value1");
}

TEST_F(TestUTF8, TestSectionAKeyAValU)
{
    const char tesuto2[] = u8"テスト2";
    const char *result = ini.GetValue("section2", "test2");
    ASSERT_STREQ(result, tesuto2);
}

TEST_F(TestUTF8, TestSectionAKeyUValA)
{
    const char tesuto[] = u8"テスト";
    const char *result = ini.GetValue("section2", tesuto);
    ASSERT_STREQ(result, "test");
}

TEST_F(TestUTF8, TestSectionAKeyUValU)
{
    const char tesuto2[] = u8"テスト2";
    const char tesutoni[] = u8"テスト二";
    const char *result = ini.GetValue("section2", tesuto2);
    ASSERT_STREQ(result, tesutoni);
}

TEST_F(TestUTF8, TestSectionUKeyAValA)
{
    const char kensa[] = u8"検査";
    const char *result = ini.GetValue(kensa, "key2");
    ASSERT_STREQ(result, "value2");
}

TEST_F(TestUTF8, TestSectionUKeyAValU)
{
    const char kensa[] = u8"検査";
    const char tesuto2[] = u8"テスト2";
    const char *result = ini.GetValue(kensa, "test2");
    ASSERT_STREQ(result, tesuto2);
}

TEST_F(TestUTF8, TestSectionUKeyUValA)
{
    const char kensa[] = u8"検査";
    const char tesuto[] = u8"テスト";
    const char *result = ini.GetValue(kensa, tesuto);
    ASSERT_STREQ(result, "test");
}

TEST_F(TestUTF8, TestSectionUKeyUValU)
{
    const char kensa[] = u8"検査";
    const char tesuto2[] = u8"テスト2";
    const char tesutoni[] = u8"テスト二";
    const char *result = ini.GetValue(kensa, tesuto2);
    ASSERT_STREQ(result, tesutoni);
}


// ### SIMPLE USAGE

TEST(TestSnippets, TestSimple)
{
    // simple demonstration

    parsers::INIParser ini;
    ini.SetUnicode();

    auto rc = ini.LoadFile("example.ini");
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_OK);

    const char *pv;
    pv = ini.GetValue("section", "key", "default");
    ASSERT_STREQ(pv, "value");

    ini.SetValue("section", "key", "newvalue");

    pv = ini.GetValue("section", "key", "default");
    ASSERT_STREQ(pv, "newvalue");
}

// ### LOADING DATA

TEST(TestSnippets, TestLoadFile)
{
    // load from a data file
    parsers::INIParser ini;
    auto rc = ini.LoadFile("example.ini");
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_OK);
}

TEST(TestSnippets, TestLoadString)
{
    // load from a string
    const std::string example = "[section]\nkey = value\n";
    parsers::INIParser ini;
    auto rc = ini.LoadText(example);
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_OK);
}


// ### GETTING SECTIONS AND KEYS

TEST(TestSnippets, TestSectionsAndKeys)
{
    const std::string example = "[section1]\n"
                                "key1 = value1\n"
                                "key2 = value2\n"
                                "\n"
                                "[section2]\n"
                                "[section3]\n";

    parsers::INIParser ini;
    auto rc = ini.LoadText(example);
    ASSERT_EQ(rc, parsers::INI_OK);



    // get all sections
    parsers::INIParser::Entries sections;
    ini.GetAllSections(sections);

    // get all keys in a section
    parsers::INIParser::Entries keys;
    ini.GetAllKeys("section1", keys);



    const char *expectedSections[] = {"section1", "section2", "section3", nullptr};
    const char *expectedKeys[] = {"key1", "key2", nullptr};

    parsers::INIParser::Entries::const_iterator it;
    int i;

    for (i = 0, it = sections.begin(); it != sections.end(); ++i, ++it) {
        ASSERT_NE(expectedSections[i], nullptr);
        ASSERT_STREQ(expectedSections[i], it->item);
    }
    ASSERT_EQ(expectedSections[i], nullptr);

    for (i = 0, it = keys.begin(); it != keys.end(); ++i, ++it) {
        ASSERT_NE(expectedKeys[i], nullptr);
        ASSERT_STREQ(expectedKeys[i], it->item);
    }
    ASSERT_EQ(expectedKeys[i], nullptr);
}


// ### GETTING VALUES

TEST(TestSnippets, TestGettingValues)
{
    const std::string example = "[section1]\n"
                                "key1 = value1\n"
                                "key2 = value2.1\n"
                                "key2 = value2.2\n"
                                "\n"
                                "[section2]\n"
                                "[section3]\n";

    bool utf8 = true;
    bool multiKey = true;
    parsers::INIParser ini(utf8, multiKey);
    auto rc = ini.LoadText(example);
    ASSERT_EQ(rc, parsers::INI_OK);


    // get the value of a key that doesn't exist
    const char *pv;
    pv = ini.GetValue("section1", "key99");
    ASSERT_EQ(pv, nullptr);

    // get the value of a key that does exist
    pv = ini.GetValue("section1", "key1");
    ASSERT_STREQ(pv, "value1");

    // get the value of a key which may have multiple
    // values. If hasMultiple is true, then there are
    // multiple values and just one value has been returned
    bool hasMulti;
    pv = ini.GetValue("section1", "key1", nullptr, &hasMulti);
    ASSERT_STREQ(pv, "value1");
    ASSERT_EQ(hasMulti, false);

    pv = ini.GetValue("section1", "key2", nullptr, &hasMulti);
    ASSERT_STREQ(pv, "value2.1");
    ASSERT_EQ(hasMulti, true);

    // get all values of a key with multiple values
    parsers::INIParser::Entries values;
    ini.GetAllValues("section1", "key2", values);

    // sort the values into a known order, in this case we want
    // the original load order
    values.sort(parsers::INIParser::Entry::LoadOrder());

    // output all of the items
    parsers::INIParser::Entries::const_iterator it;
    for (it = values.begin(); it != values.end(); ++it) {
        // printf("value = '%s'\n", it->item);
    }


    int i;
    const char *expectedValues[] = {"value2.1", "value2.2", nullptr};
    for (i = 0, it = values.begin(); it != values.end(); ++it, ++i) {
        ASSERT_NE(expectedValues[i], nullptr);
        ASSERT_STREQ(expectedValues[i], it->item);
    }
    ASSERT_EQ(expectedValues[i], nullptr);
}


// ### MODIFYING DATA

TEST(TestSnippets, TestModifyingData)
{
    bool utf8 = true;
    bool multiKey = false;
    parsers::INIParser ini(utf8, multiKey);
    int rc;


    // add a new section
    rc = ini.SetValue("section1", nullptr, nullptr);
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_INSERTED);

    // not an error to add one that already exists
    rc = ini.SetValue("section1", nullptr, nullptr);
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_UPDATED);

    // get the value of a key that doesn't exist
    const char *pv;
    pv = ini.GetValue("section2", "key1", "default-value");
    ASSERT_STREQ(pv, "default-value");

    // adding a key (the section will be added if needed)
    rc = ini.SetValue("section2", "key1", "value1");
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_INSERTED);

    // ensure it is set to expected value
    pv = ini.GetValue("section2", "key1", nullptr);
    ASSERT_STREQ(pv, "value1");

    // change the value of a key
    rc = ini.SetValue("section2", "key1", "value2");
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_UPDATED);

    // ensure it is set to expected value
    pv = ini.GetValue("section2", "key1", nullptr);
    ASSERT_STREQ(pv, "value2");
}


// ### DELETING DATA

TEST(TestSnippets, TestDeletingData)
{
    const std::string example = "[section1]\n"
                                "key1 = value1\n"
                                "key2 = value2\n"
                                "\n"
                                "[section2]\n"
                                "key1 = value1\n"
                                "key2 = value2\n"
                                "\n"
                                "[section3]\n";

    bool utf8 = true;
    parsers::INIParser ini(utf8);
    auto rc = ini.LoadText(example);
    ASSERT_EQ(rc, parsers::INI_OK);


    // deleting a key from a section. Optionally the entire
    // section may be deleted if it is now empty.
    bool done, deleteSectionIfEmpty = true;
    done = ini.Delete("section1", "key1", deleteSectionIfEmpty);
    ASSERT_EQ(done, true);
    done = ini.Delete("section1", "key1");
    ASSERT_EQ(done, false);

    // deleting an entire section and all keys in it
    done = ini.Delete("section2", nullptr);
    ASSERT_EQ(done, true);
    done = ini.Delete("section2", nullptr);
    ASSERT_EQ(done, false);
}


// ### SAVING DATA

TEST(TestSnippets, TestSavingData)
{
    bool utf8 = true;
    parsers::INIParser ini(utf8);
    int rc;


    // save the data to a string
    std::string data;
    rc = ini.Dump(data);
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_OK);

    // save the data back to the file
    rc = ini.DumpFile("example2.ini");
    if (rc < 0) { /* handle error */
    };
    ASSERT_EQ(rc, parsers::INI_OK);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
