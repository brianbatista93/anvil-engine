#pragma once

#include "Containers/Array.h"
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Containers");

TEST_CASE("[TArray]: Constructors and assignments")
{
    SUBCASE("Default constructor")
    {
        TArray<int32> u;

        CHECK_EQ(u.GetSize(), 0);
        CHECK_EQ(u.GetCapacity(), 0);
        CHECK(u.IsEmpty());
    }

    SUBCASE("Initial sized constructor")
    {
        struct MyStruct
        {
            int32 a;
            float b;
            char  c[8];
        };

        TArray<MyStruct> u(5);

        CHECK_EQ(u.GetSize(), 5);
        CHECK_EQ(u.GetCapacity(), 5);
        CHECK_FALSE(u.IsEmpty());
    }

    SUBCASE("Copy constructor")
    {
        TArray<int32> tu(10);
        TArray<int32> tv(tu);

        struct MyStruct
        {
            int32 a = 29;
            float b = 0.7f;
            char  c[8];

            MyStruct() {}

            MyStruct(const MyStruct& other)
              : a(other.a)
              , b(other.b)
            {}
        };

        TArray<MyStruct> u(10);
        TArray<MyStruct> v(u);

        printf("");
    }
}

TEST_SUITE_END();
