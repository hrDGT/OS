#include <gtest/gtest.h>
#include "marker_processor.h"

TEST(TryMarkTests, MarksZeroCell)
{
  int data[5] = {0, 0, 0, 0, 0};
  EXPECT_TRUE(try_mark(data, 2, 1));
  EXPECT_EQ(data[2], 1);
}

TEST(TryMarkTests, RefusesOccupiedCell)
{
  int data[5] = {0, 42, 0, 0, 0};
  EXPECT_FALSE(try_mark(data, 1, 3));
  EXPECT_EQ(data[1], 42);
}

TEST(TryMarkTests, DoesNotTouchOtherCells)
{
  int data[5] = {0, 0, 0, 0, 0};
  try_mark(data, 3, 2);
  for (int i = 0; i < 5; i++)
    if (i != 3) EXPECT_EQ(data[i], 0);
}

TEST(TryMarkTests, FirstCell)
{
  int data[3] = {0, 1, 1};
  EXPECT_TRUE(try_mark(data, 0, 5));
  EXPECT_EQ(data[0], 5);
}

TEST(TryMarkTests, LastCell)
{
  int data[3] = {1, 1, 0};
  EXPECT_TRUE(try_mark(data, 2, 7));
  EXPECT_EQ(data[2], 7);
}

TEST(CountMarkedTests, CountsCorrectId)
{
  int data[6] = {1, 2, 1, 3, 1, 0};
  EXPECT_EQ(count_marked(data, 6, 1), 3);
}

TEST(CountMarkedTests, ZeroWhenNoneMarked)
{
  int data[4] = {2, 3, 4, 0};
  EXPECT_EQ(count_marked(data, 4, 1), 0);
}

TEST(CountMarkedTests, AllMarkedBySameThread)
{
  int data[4] = {5, 5, 5, 5};
  EXPECT_EQ(count_marked(data, 4, 5), 4);
}

TEST(CountMarkedTests, SingleElement)
{
  int data[1] = {3};
  EXPECT_EQ(count_marked(data, 1, 3), 1);
  EXPECT_EQ(count_marked(data, 1, 9), 0);
}

TEST(CountMarkedTests, IgnoresZeroCells)
{
  int data[5] = {0, 0, 0, 0, 0};
  EXPECT_EQ(count_marked(data, 5, 0), 5);
  EXPECT_EQ(count_marked(data, 5, 1), 0);
}

TEST(ClearMarksTests, ClearsOnlyOwnId)
{
  int data[5] = {1, 2, 1, 3, 1};
  clear_marks(data, 5, 1);
  EXPECT_EQ(data[0], 0);
  EXPECT_EQ(data[1], 2);
  EXPECT_EQ(data[2], 0);
  EXPECT_EQ(data[3], 3);
  EXPECT_EQ(data[4], 0);
}

TEST(ClearMarksTests, NoOpWhenNothingToClean)
{
  int data[4] = {2, 3, 4, 5};
  clear_marks(data, 4, 1);
  EXPECT_EQ(data[0], 2);
  EXPECT_EQ(data[1], 3);
  EXPECT_EQ(data[2], 4);
  EXPECT_EQ(data[3], 5);
}

TEST(ClearMarksTests, ClearsFullArray)
{
  int data[4] = {7, 7, 7, 7};
  clear_marks(data, 4, 7);
  for (int i = 0; i < 4; i++)
    EXPECT_EQ(data[i], 0);
}

TEST(ClearMarksTests, CountDropsToZeroAfterClear)
{
  int data[5] = {3, 1, 3, 2, 3};
  clear_marks(data, 5, 3);
  EXPECT_EQ(count_marked(data, 5, 3), 0);
}

TEST(ClearMarksTests, OtherThreadCountsUnchanged)
{
  int data[6] = {1, 2, 1, 2, 1, 2};
  clear_marks(data, 6, 1);
  EXPECT_EQ(count_marked(data, 6, 2), 3);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
