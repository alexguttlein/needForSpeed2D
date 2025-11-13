#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::HasSubstr;
using ::testing::AllOf;


namespace {
    TEST(FooTest, Check) {
        EXPECT_EQ((4), (int)(4+5));
    }
}
