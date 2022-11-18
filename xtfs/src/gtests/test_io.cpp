#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C" {
#include "../io.h"
}

class IO_BASIC: public testing::Test {
  protected:
    FILE* file = NULL;
    char* data = NULL;
    const size_t SIZE = 4096;
    virtual void SetUp() {
        file = fopen("gtests/gtest.img", "rw+");
        data = (char*)malloc(SIZE * sizeof(char));
        memset(data, 1, SIZE * sizeof(char));
    }

    virtual void TearDown() {
        fclose(file);
        free(data);
    }
};

TEST_F(IO_BASIC, WRITE) {
    ASSERT_EQ(write_file(file, 0, data, SIZE), 0);
}

TEST_F(IO_BASIC, READ) {
    ASSERT_EQ(read_file(file, 0, data, SIZE), 0);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}