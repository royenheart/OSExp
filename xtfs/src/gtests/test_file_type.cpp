/**
 * @file test_file_type.cpp
 * @author RoyenHeart
 * @brief 测试文件类型相关函数
 * @version 1.0.0
 * @date 2022-12-05
 *
 * @copyright Copyright (c) 2022
 *
 */

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

TEST_F(FILE_TYPE, IS_SPEC_TEST) {
    ASSERT_EQ(is_spec_format(TEXT_FILE, CIPHER), 1);
    ASSERT_EQ(is_spec_format(TEXT_FILE, CIPHER | ZIP), 1);
    ASSERT_EQ(is_spec_format(TEXT_FILE | CIPHER, CIPHER), 0);
    ASSERT_EQ(is_spec_format(TEXT_FILE | ZIP, ZIP), 0);
    ASSERT_EQ(is_spec_format(TEXT_FILE | CIPHER, ZIP), 1);
    ASSERT_EQ(is_spec_format(TEXT_FILE | ZIP, CIPHER), 1);
    ASSERT_EQ(is_spec_format(TEXT_FILE | CIPHER | ZIP, CIPHER | ZIP), 0);
    ASSERT_EQ(is_spec_format(TEXT_FILE | CIPHER | ZIP, CIPHER), 0);
    ASSERT_EQ(is_spec_format(TEXT_FILE | CIPHER | ZIP, ZIP), 0);
}

TEST_F(FILE_TYPE, DIR_FILE_TEST) {
    ASSERT_EQ(get_file_type(DIR_FILE), DIR_FILE);
}

#ifdef DEBUG
TEST_F(FILE_TYPE, ILLEGAL_TYPE) {
    ASSERT_EQ(get_file_type(DIR_FILE | CIPHER), NO_FILE);
    ASSERT_EQ(get_file_type(DIR_FILE | ZIP), NO_FILE);
    ASSERT_EQ(get_file_type(DIR_FILE | CIPHER | ZIP), NO_FILE);
    ASSERT_EQ(get_file_type(NO_FILE), NO_FILE);
    ASSERT_EQ(get_file_type(NOT_FOUND), NO_FILE);
    ASSERT_EQ(get_file_type(MAX_TYPE_NUM + 1), NO_FILE);
}
#endif

TEST_F(FILE_TYPE, SAME_TYPE_CLASS_TEST) {
    ASSERT_EQ(is_same_type_class(TEXT_FILE, EXE_FILE), 1);
    ASSERT_EQ(is_same_type_class(TEXT_FILE, TEXT_FILE), 1);
    ASSERT_EQ(is_same_type_class(TEXT_FILE, EXE_FILE | CIPHER), 1);
    ASSERT_EQ(is_same_type_class(TEXT_FILE, DIR_FILE), 0);
    ASSERT_EQ(is_same_type_class(TEXT_FILE | CIPHER, DIR_FILE), 0);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}