#include <gtest/gtest.h>
#include "array_processor.h"

TEST(MinMaxTests, Basic)
{
  std::vector<int> v{1, 2, 3, 4, 5};
  auto r = compute_min_max(v);
  EXPECT_EQ(r.minValue, 1);
  EXPECT_EQ(r.maxValue, 5);
}

TEST(MinMaxTests, Negative)
{
  std::vector<int> v{-10, 0, 10, 20};
  auto r = compute_min_max(v);
  EXPECT_EQ(r.minValue, -10);
  EXPECT_EQ(r.maxValue, 20);
}

TEST(AverageTests, Basic)
{
  std::vector<int> v{1, 2, 3, 4, 5};
  EXPECT_DOUBLE_EQ(compute_average(v), 3.0);
}

TEST(AverageTests, Mixed)
{
  std::vector<int> v{-10, 0, 10, 20};
  EXPECT_DOUBLE_EQ(compute_average(v), 5.0);
}

TEST(AverageTests, Single)
{
  std::vector<int> v{7};
  EXPECT_DOUBLE_EQ(compute_average(v), 7.0);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
