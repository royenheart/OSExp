#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include "../xtfs_struct.h"
}

class FILE_TYPE: public testing::Test {};

TEST_F(FILE_TYPE, NORMAL_EXE) {
    ASSERT_EQ(get_file_type(EXE_FILE), EXE_FILE);
}

TEST_F(FILE_TYPE, NORMAL_TEXT) {
    ASSERT_EQ(get_file_type(TEXT_FILE), TEXT_FILE);
}

TEST_F(FILE_TYPE, NORMAL_UNKNOWN) {
    ASSERT_EQ(get_file_type(UNKNOWN_FILE), UNKNOWN_FILE);
    ASSERT_EQ(get_file_type(UNKNOWN_FILE + 1), UNKNOWN_FILE);
}

TEST_F(FILE_TYPE, SPEC_EXE) {
    ASSERT_EQ(get_file_type(EXE_FILE | CIPHER), EXE_FILE | CIPHER);
    ASSERT_EQ(get_file_type(EXE_FILE | ZIP), EXE_FILE | ZIP);
    ASSERT_EQ(get_file_type(EXE_FILE | CIPHER | ZIP), EXE_FILE | CIPHER | ZIP);
}

TEST_F(FILE_TYPE, SPEC_TEXT) {
    ASSERT_EQ(get_file_type(TEXT_FILE | CIPHER), TEXT_FILE | CIPHER);
    ASSERT_EQ(get_file_type(TEXT_FILE | ZIP), TEXT_FILE | ZIP);
    ASSERT_EQ(get_file_type(TEXT_FILE | CIPHER | ZIP), TEXT_FILE | CIPHER | ZIP);
}

TEST_F(FILE_TYPE, SPEC_UNKNOWN) {
    ASSERT_EQ(get_file_type(UNKNOWN_FILE | CIPHER), UNKNOWN_FILE | CIPHER);
    ASSERT_EQ(get_file_type(UNKNOWN_FILE | ZIP), UNKNOWN_FILE | ZIP);
    ASSERT_EQ(get_file_type(UNKNOWN_FILE | CIPHER | ZIP), UNKNOWN_FILE | CIPHER | ZIP);
    ASSERT_EQ(get_file_type((UNKNOWN_FILE + 1) | CIPHER), (UNKNOWN_FILE) | CIPHER);
    ASSERT_EQ(get_file_type((UNKNOWN_FILE + 1) | ZIP), (UNKNOWN_FILE) | ZIP);
    ASSERT_EQ(get_file_type((UNKNOWN_FILE + 1) | CIPHER | ZIP), (UNKNOWN_FILE) | CIPHER | ZIP);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}