#include "gtest/gtest.h"
#include "utils/input_parsing.h"

// CheckFileName without postfix
TEST(InputParsingTest, FileNameEmpty) {
    EXPECT_EQ(CheckFileName("", "", false), false);
}

TEST(InputParsingTest, FileNameTooLong) {
    std::string long_name(256, 'a');
    EXPECT_EQ(CheckFileName(long_name, "", false), false);
}

TEST(InputParsingTest, FileNameForbidden3CharsLong) {
    EXPECT_EQ(CheckFileName("AUX", "", false), false);
}

TEST(InputParsingTest, FileNameForbidden4CharsLong) {
    EXPECT_EQ(CheckFileName("LPT1", "", false), false);
}

TEST(InputParsingTest, FileNameWithForbiddenChars) {
    EXPECT_EQ(CheckFileName("file<name", "", false), false);
}

TEST(InputParsingTest, FileNameWithTab) {
    EXPECT_EQ(CheckFileName("file\tname", "", false), false);
}

TEST(InputParsingTest, FileNameWithNewline) {
    EXPECT_EQ(CheckFileName("file\nname", "", false), false);
}

TEST(InputParsingTest, FileNameEndsWithSpace) {
    EXPECT_EQ(CheckFileName("file ", "", false), false);
}

TEST(InputParsingTest, FileNameEndsWithDot) {
    EXPECT_EQ(CheckFileName("file.", "", false), false);
}

TEST(InputParsingTest, FileNameValid) {
    EXPECT_EQ(CheckFileName("valid_name", "", false), true);
}

// CheckFileName with postfix
TEST(InputParsingTest, FileNameWithCorrectPostfix) {
    EXPECT_EQ(CheckFileName("file.txt", ".txt", false), true);
}

TEST(InputParsingTest, FileNameWithWrongPostfix) {
    EXPECT_EQ(CheckFileName("file.bin", ".txt", false), false);
}

TEST(InputParsingTest, FileNameTooShortForPostfix) {
    EXPECT_EQ(CheckFileName("a", ".txt", false), false);
}

// CheckIfSizeT
TEST(InputParsingTest, SizeTEmptyString) {
    EXPECT_EQ(CheckIfSizeT("", false), false);
}

TEST(InputParsingTest, SizeTNegative) {
    EXPECT_EQ(CheckIfSizeT("-1", false), false);
}

TEST(InputParsingTest, SizeTNonDigit) {
    EXPECT_EQ(CheckIfSizeT("12a34", false), false);
}

TEST(InputParsingTest, SizeTTooLarge) {
    EXPECT_EQ(CheckIfSizeT("999999999999999999999999", false), false);
}

TEST(InputParsingTest, SizeTValid) {
    EXPECT_EQ(CheckIfSizeT("12345", false), true);
}

TEST(InputParsingTest, SizeTMax) {
    EXPECT_EQ(CheckIfSizeT(std::to_string(SIZE_MAX), false), true);
}

// CheckIfInt
TEST(InputParsingTest, IntEmptyString) {
    EXPECT_EQ(CheckIfInt("", false), false);
}

TEST(InputParsingTest, IntOnlyMinus) {
    EXPECT_EQ(CheckIfInt("-", false), false);
}

TEST(InputParsingTest, IntPositive) {
    EXPECT_EQ(CheckIfInt("123", false), true);
}

TEST(InputParsingTest, IntNegative) {
    EXPECT_EQ(CheckIfInt("-123", false), true);
}

TEST(InputParsingTest, IntNonDigit) {
    EXPECT_EQ(CheckIfInt("12-34", false), false);
}

TEST(InputParsingTest, IntTooLarge) {
    EXPECT_EQ(CheckIfInt("999999999999999999999999", false), false);
}

TEST(InputParsingTest, IntMax) {
    EXPECT_EQ(CheckIfInt(std::to_string(INT_MAX), false), true);
}

TEST(InputParsingTest, IntMin) {
    EXPECT_EQ(CheckIfInt(std::to_string(INT_MIN), false), true);
}

// CheckIfDouble
TEST(InputParsingTest, DoubleEmptyString) {
    EXPECT_EQ(CheckIfDouble("", false), false);
}

TEST(InputParsingTest, DoubleOnlyDot) {
    EXPECT_EQ(CheckIfDouble(".", false), false);
}

TEST(InputParsingTest, DoubleScientific) {
    EXPECT_EQ(CheckIfDouble("1.23e-4", false), true);
}

TEST(InputParsingTest, DoubleNegative) {
    EXPECT_EQ(CheckIfDouble("-3.14", false), true);
}

TEST(InputParsingTest, DoubleNoLeadingZero) {
    EXPECT_EQ(CheckIfDouble(".5", false), true);
}

TEST(InputParsingTest, DoubleNoTrailingZero) {
    EXPECT_EQ(CheckIfDouble("0.", false), true);
}

TEST(InputParsingTest, DoubleInvalid) {
    EXPECT_EQ(CheckIfDouble("abc", false), false);
}

TEST(InputParsingTest, DoubleMax) {
    EXPECT_EQ(CheckIfDouble(std::to_string(DBL_MAX), false), true);
}

TEST(InputParsingTest, DoubleMin) {
    EXPECT_EQ(CheckIfDouble(std::to_string(DBL_MIN), false), true);
}