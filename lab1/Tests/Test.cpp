#include "gtest/gtest.h"
#include "utils/input_parsing.h"

TEST(InputParsingTest, FileNameTestWithoutPostfix) 
{
  EXPECT_EQ(CheckFileName("CON"), false);
  EXPECT_EQ(CheckFileName("..."), false);
  EXPECT_EQ(CheckFileName("test"), true);
  EXPECT_EQ(CheckFileName("<file"), false);
  EXPECT_EQ(CheckFileName(" "), false);
}

TEST(InputParsingTest, FileNameTestWithPostfix)
{
	EXPECT_EQ(CheckFileName("CON.txt", ".txt"), true);
	EXPECT_EQ(CheckFileName("....bin", ".txt"), false);
	EXPECT_EQ(CheckFileName("test", "test.txt"), false);
	EXPECT_EQ(CheckFileName("<file.txt", ".txt"), true);
	EXPECT_EQ(CheckFileName(" ", ".txt"), false);
}

TEST(InputParsingTest, SizeTTest) 
{
	EXPECT_EQ(CheckIfSizeT("999999999999999999999999999999999999999999999999999999999999"), false);
	EXPECT_EQ(CheckIfSizeT("-100"), false);
	EXPECT_EQ(CheckIfSizeT("abc"), false);
	EXPECT_EQ(CheckIfSizeT(std::to_string(SIZE_MAX)), true);
	EXPECT_EQ(CheckIfSizeT("456"), true);
}

TEST(InputParsingTest, IntTest) 
{
	EXPECT_EQ(CheckIfInt("-100"), true);
	EXPECT_EQ(CheckIfInt("674"), true);
	EXPECT_EQ(CheckIfInt("99999999999999999999999999999999999"), false);
	EXPECT_EQ(CheckIfInt("abd"), false);
	EXPECT_EQ(CheckIfInt(std::to_string(INT_MAX)), true);
	EXPECT_EQ(CheckIfInt(std::to_string(INT_MIN)), true);
}

TEST(InputParsingTest, DoubleTest) 
{
	EXPECT_EQ(CheckIfDouble("0."), true);
	EXPECT_EQ(CheckIfDouble(".8"), true);
	EXPECT_EQ(CheckIfDouble("0.000000001"), true);
	EXPECT_EQ(CheckIfDouble("5000000.1"), true);
	EXPECT_EQ(CheckIfDouble(std::to_string(DBL_MAX)), true);
	EXPECT_EQ(CheckIfDouble(std::to_string(DBL_MIN)), true);
	EXPECT_EQ(CheckIfDouble("-0.8"), true);
	EXPECT_EQ(CheckIfDouble("jdhfkjdshfks"), false);
}