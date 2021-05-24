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
TEST_CASE("[String] Constructors and Assignments")
{
    String       a = ATEXT("Brian");
    TSet<String> set;
    set.Add(a);
    set.Add(a);
    set.Add(ATEXT("Anderson"));

    set.Remove(a);

    std::set<String> stdSet;
    stdSet.insert(a);
    stdSet.insert(a);
    stdSet.insert(ATEXT("Anderson"));
}
TEST_SUITE_END();
