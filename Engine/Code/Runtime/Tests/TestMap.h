#pragma once

#include "Containers/AnvilString.h"
#include "Containers/Map.h"
#include <doctest/doctest.h>

struct MyMapStruct
{
    const char* Name;
    int32       Age;
    float       Weight;
};

TEST_SUITE_BEGIN("Containers");
TEST_CASE("[Map] Constructors and Assignments")
{
    SUBCASE("Default constructor")
    {
        TMap<String, MyMapStruct> map;
        CHECK_EQ(map.GetCount(), 0);
        CHECK(map.IsEmpty());
    }
}

TEST_CASE("[Map] Inserting and Removing")
{
    SUBCASE("Default constructor")
    {
        TMap<String, MyMapStruct> map;

        MyMapStruct brian;
        brian.Name   = "Brian";
        brian.Age    = 27;
        brian.Weight = 72.55f;

        map.Add(ATEXT("Brian"), brian);
    }
}
TEST_SUITE_END();
