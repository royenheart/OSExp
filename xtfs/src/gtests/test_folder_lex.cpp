#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include "../lex/folder_lex.h"
#include "../xtfs_struct.h"
}

class FOLDER_LEX: public testing::Test {
protected:
    char **str = NULL;
    int ret;
    virtual void SetUp() {}

    virtual void TearDown() {}
};

TEST_F(FOLDER_LEX, IDENTIFY_FORM1) {
    ret = get_folders("a/sd/s///g-44//", &str);
    ASSERT_FALSE(str == NULL);
    ASSERT_STREQ("a", str[0]);
    ASSERT_STREQ("sd", str[1]);
    ASSERT_STREQ("s", str[2]);
    ASSERT_STREQ("g-44", str[3]);
    ASSERT_NE(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, IDENTIFY_FORM2) {
    ret = get_folders("//dff-45//77dsdAsf23/dd_fgg", &str);
    ASSERT_FALSE(str == NULL);
    ASSERT_STREQ("dff-45", str[0]);
    ASSERT_STREQ("77dsdAsf23", str[1]);
    ASSERT_STREQ("dd_fgg", str[2]);
    ASSERT_NE(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, IDENTIFY_FORM3) {
    ret = get_folders("/DDD44//\"232dsf 34dfg\"//", &str);
    ASSERT_FALSE(str == NULL);
    ASSERT_STREQ("DDD44", str[0]);
    ASSERT_STREQ("\"232dsf 34dfg\"", str[1]);
    ASSERT_NE(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, IDENTIFY_FORM4) {
    ret = get_folders("/hello.md", &str);
    ASSERT_FALSE(str == NULL);
    ASSERT_STREQ("hello.md", str[0]);
    ASSERT_NE(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, ROOT) {
    ret = get_folders("/", &str);
    ASSERT_EQ(ret, NOT_FOUND);
}

TEST_F(FOLDER_LEX, IDENTIFY_NULL1) {
    ret = get_folders("/DDD 44//\"232dsf 34dfg\"//", &str);
    ASSERT_EQ(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, IDENTIFY_NULL2) {
    ret = get_folders("/DDD-44//=3=sd213//", &str);
    ASSERT_EQ(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, IDENTIFY_NULL3) {
    ret = get_folders("", &str);
    ASSERT_EQ(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, IDENTIFY_NULL4) {
    ret = get_folders("//sdadasdasdas-sadasdasdasdasdsd343cxsdsdadasdasdas-sadasdasdasdasdsd343cxsd/", &str);
    ASSERT_EQ(ret, ERROR_PARSE);
}

TEST_F(FOLDER_LEX, IDENTIFY_NULL5) {
    ret = get_folders("s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/"
                      "s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/"
                      "s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/"
                      "s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/", &str);
    ASSERT_EQ(ret, ERROR_PARSE);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}