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

    SUBCASE("Initial copies constructor")
    {
        TArray<int32> u(5u, 29);

        CHECK_EQ(u.GetSize(), 5);
        CHECK_EQ(u.GetCapacity(), 5);
        CHECK_FALSE(u.IsEmpty());
        CHECK_EQ(u.Front(), 29);
        CHECK_EQ(u.Back(), 29);

        for (uint64 i = 0; i < 5; i++) {
            CHECK_EQ(u[i], 29);
        }
    }

    SUBCASE("Initial size constructor")
    {
        TArray<int32> u(5u);

        CHECK_EQ(u.GetSize(), 5);
        CHECK_EQ(u.GetCapacity(), 5);
        CHECK_FALSE(u.IsEmpty());
        CHECK_EQ(u.Front(), 0);
        CHECK_EQ(u.Back(), 0);

        for (uint64 i = 0; i < 5; i++) {
            CHECK_EQ(u[i], 0);
        }
    }

    SUBCASE("Range constructor")
    {
        std::vector<int32> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        TArray<int32>      u(v.begin(), v.end());

        CHECK_EQ(u.GetSize(), v.size());
        CHECK_EQ(u.GetCapacity(), v.capacity());
        CHECK_FALSE(u.IsEmpty());
        CHECK_EQ(u.Front(), v.front());
        CHECK_EQ(u.Back(), v.back());

        for (uint64 i = 0; i < v.size(); i++) {
            CHECK_EQ(u[i], v[i]);
        }
    }

    SUBCASE("Copy constructor")
    {
        TArray<int32> u(5);
        u[0] = 1;
        u[1] = 2;
        u[2] = 3;
        u[3] = 4;
        u[4] = 5;

        TArray<int32> v(u);

        CHECK_EQ(u.GetSize(), v.GetSize());
        CHECK_EQ(u.GetCapacity(), v.GetCapacity());
        CHECK_EQ(u.Front(), v.Front());
        CHECK_EQ(u.Back(), v.Back());

        for (uint64 i = 0; i < 5; i++) {
            CHECK_EQ(u[i], v[i]);
        }
    }

    SUBCASE("Move constructor")
    {
        TArray<int32> u(5);
        u[0] = 1;
        u[1] = 2;
        u[2] = 3;
        u[3] = 4;
        u[4] = 5;

        TArray<int32> v = std::move(u);

        CHECK_EQ(u.GetSize(), 0);
        CHECK_EQ(u.GetCapacity(), 0);
        CHECK(u.IsEmpty());

        CHECK_EQ(v.GetSize(), 5);
        CHECK_EQ(v.GetCapacity(), 5);
        CHECK_FALSE(v.IsEmpty());

        CHECK_EQ(v[0], 1);
        CHECK_EQ(v[1], 2);
        CHECK_EQ(v[2], 3);
        CHECK_EQ(v[3], 4);
        CHECK_EQ(v[4], 5);
    }

    SUBCASE("List constructor")
    {
        TArray<int32> u = {1, 2, 3, 4, 5};

        CHECK_EQ(u.GetSize(), 5);
        CHECK_EQ(u.GetCapacity(), 5);
        CHECK_FALSE(u.IsEmpty());

        CHECK_EQ(u[0], 1);
        CHECK_EQ(u[1], 2);
        CHECK_EQ(u[2], 3);
        CHECK_EQ(u[3], 4);
        CHECK_EQ(u[4], 5);
    }
}

TEST_CASE("[TArray]: Resize(), Insert(), and RemoveAt()")
{
    TArray<int32> arr;
    arr.Resize(2);
    CHECK(arr.GetSize() == 2);
    arr.Insert(0, 1);
    CHECK(int32(arr[0]) == 1);
    arr.Insert(0, 2);
    CHECK(int32(arr[0]) == 2);
    arr.RemoveAt(0);
    CHECK(int32(arr[0]) == 0);
}

TEST_SUITE_END();
