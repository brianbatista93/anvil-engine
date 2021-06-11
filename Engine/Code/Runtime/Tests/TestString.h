#pragma once

#include "Containers/AnvilString.h"
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Containers");
TEST_CASE("[String] Constructors and Assignments")
{
    SUBCASE("Default constructor")
    {
        String s;

        CHECK_EQ(s.GetSize(), 0);
        CHECK_EQ(s.GetLength(), 0);
        CHECK_EQ(s.GetSizeInBytes(), 16);
        CHECK(s.IsEmpty());
    }

    SUBCASE("Character pointer constructor tchar")
    {
        String text = ATEXT("我喜欢咖啡");

        CHECK_EQ(text.GetSize(), 6);
        CHECK_EQ(text.GetLength(), 5);
        CHECK_EQ(text, ATEXT("我喜欢咖啡"));
    }

    SUBCASE("Character pointer constructor char")
    {
        String text = "I love coffee";
        CHECK_EQ(text.GetSize(), 14);
        CHECK_EQ(text.GetLength(), 13);
        CHECK_EQ(text, ATEXT("I love coffee"));
    }

    SUBCASE("Character pointer constructor char8")
    {
        String text = (const char8*)u8"我喜欢咖啡";

        CHECK_EQ(text.GetSize(), 6);
        CHECK_EQ(text.GetLength(), 5);
        CHECK_EQ(text, ATEXT("我喜欢咖啡"));
    }

    SUBCASE("Character pointer constructor wchar_t")
    {
        String text = L"我喜欢咖啡";

        CHECK_EQ(text.GetSize(), 6);
        CHECK_EQ(text.GetLength(), 5);
        CHECK_EQ(text, ATEXT("我喜欢咖啡"));
    }
}

TEST_CASE("[String] Append operations")
{
    String result;
    String english = ATEXT("I love coffee");
    String chinese = ATEXT("我喜欢咖啡");

    result = english;
    result += ATEXT('-');
    result += chinese;
    result += String(ATEXT(" 😍☕😍"));

    CHECK_EQ(result.GetSize(), 26);
    CHECK_EQ(result.GetLength(), 25);
    CHECK_EQ(result, ATEXT("I love coffee-我喜欢咖啡 😍☕😍"));
}

TEST_CASE("[String] Formatter")
{
    String formatted
      = String::Format(ATEXT("So here we are, {} little mice {1}{1} walking {:2.2f} miles in {} days."), 2, ATEXT("🐀"), 36.84f, 3);

    CHECK_EQ(formatted, ATEXT("So here we are, 2 little mice 🐀🐀 walking 36.84 miles in 3 days."));
}
TEST_SUITE_END();
