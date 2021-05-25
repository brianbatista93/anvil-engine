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
        set.Add(ATEXT("Abacate"));
        set.Add(ATEXT("Pera"));
        set.Add(ATEXT("Banana"));
        set.Add(ATEXT("Uva"));
        set.Add(ATEXT("Abacaxi"));
        set.Add(ATEXT("Açaí"));
    }
}
TEST_SUITE_END();
