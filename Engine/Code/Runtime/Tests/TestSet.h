#pragma once

#include "Containers/AnvilString.h"
#include "Containers/Set.h"
#include <doctest/doctest.h>

namespace std {
template<>
struct hash<String>
{
    std::size_t operator()(const String& k) const { return std::hash<std::string>()(*k); }
};
} // namespace std

TEST_SUITE_BEGIN("Containers");
TEST_CASE("[Set] Constructors and Assignments")
{
    SUBCASE("Default constructor")
    {
        TSet<String> set;
        CHECK_EQ(set.GetCount(), 0);
        CHECK(set.IsEmpty());
    }
}

TEST_CASE("[Set] Inserting and Removing")
{
    SUBCASE("Default constructor")
    {
        TSet<String> set;
        set.Add(ATEXT("honeydew"));
        set.Add(ATEXT("grapefruit"));
        set.Add(ATEXT("grape"));
        set.Add(ATEXT("passon_fruit"));
        set.Add(ATEXT("orange"));
        set.Add(ATEXT("cantaloupe"));
        set.Add(ATEXT("apricot"));
        set.Add(ATEXT("pomegranate"));
        set.Add(ATEXT("avocado"));
        set.Add(ATEXT("raspberry"));
        set.Add(ATEXT("kiwi"));
        set.Add(ATEXT("watermelon"));
        set.Add(ATEXT("jujube"));
        set.Add(ATEXT("blackberry"));
        set.Add(ATEXT("strawberry"));
        set.Add(ATEXT("apple"));
        set.Add(ATEXT("tangerine"));
        set.Add(ATEXT("kumquat"));
        set.Add(ATEXT("lime"));
        set.Add(ATEXT("peach"));
        set.Add(ATEXT("tomato"));
        set.Add(ATEXT("boysenberry"));
        set.Add(ATEXT("huckleberry"));
        set.Add(ATEXT("clementine"));
        set.Add(ATEXT("papaya"));
        set.Add(ATEXT("star fruit"));
        set.Add(ATEXT("cherry"));
        set.Add(ATEXT("satsuma"));
        set.Add(ATEXT("fig"));
        set.Add(ATEXT("pear"));
        set.Add(ATEXT("blueberry"));
        set.Add(ATEXT("banana"));
    }
}
TEST_SUITE_END();
