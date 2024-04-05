#include "pch.h"
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>


TEST(MyFirstTest, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

using namespace std;

TEST(MyFirstTest, Subtest_1) {
	ASSERT_TRUE(1 == 1);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}