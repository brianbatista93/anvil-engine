#pragma once

#include "Containers/Array.h"
#include <doctest/doctest.h>

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

TEST_SUITE_BEGIN("Containers");

TEST_CASE("[TArray]")
{
    SUBCASE("Default constructor")
    {
        TArray<int32> u;

        CHECK_EQ(u.GetSize(), 0);
        CHECK_EQ(u.GetCapacity(), 0);
        CHECK(u.IsEmpty());
    }

    SUBCASE("Array pointer constructor")
    {
        int32* v = new int32[5] {1, 2, 3, 4, 5};

        TArray<int32> u(v, v + 3);

        CHECK_EQ(u.GetSize(), 3);
        CHECK_EQ(u.GetCapacity(), 3);
        CHECK_FALSE(u.IsEmpty());

        CHECK_EQ(u[0], 1);
        CHECK_EQ(u[1], 2);
        CHECK_EQ(u[2], 3);

        SafeDeleteArray(v);
    }

    SUBCASE("Copy and List constructor")
    {
        TArray<int32> u = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        TArray<int32> v(u);

        CHECK_EQ(u.GetSize(), 10);
        CHECK_EQ(u.GetCapacity(), 10);
        CHECK_FALSE(u.IsEmpty());

        CHECK_EQ(v.GetSize(), 10);
        CHECK_EQ(v.GetCapacity(), 10);
        CHECK_FALSE(v.IsEmpty());

        for (uint32 i = 0; i < 10; i++) {
            CHECK_EQ(u[i], i + 1);
            CHECK_EQ(v[i], u[i]);
        }
    }

    SUBCASE("Move constructor")
    {
        TArray<int32> u = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        TArray<int32> v(std::move(u));

        CHECK_EQ(u.GetSize(), 0);
        CHECK_EQ(u.GetCapacity(), 0);
        CHECK(u.IsEmpty());

        CHECK_EQ(v.GetSize(), 10);
        CHECK_EQ(v.GetCapacity(), 10);
        CHECK_FALSE(v.IsEmpty());

        for (uint32 i = 0; i < 10; i++) {
            CHECK_EQ(v[i], i + 1);
        }
    }

    SUBCASE("Copy assignment constructor")
    {
        TArray<int32> u = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        TArray<int32> v = {11, 12, 13};

        v = u;

        CHECK_EQ(u.GetSize(), 10);
        CHECK_EQ(u.GetCapacity(), 10);
        CHECK_FALSE(u.IsEmpty());

        CHECK_EQ(v.GetSize(), 10);
        CHECK_EQ(v.GetCapacity(), 10);
        CHECK_FALSE(v.IsEmpty());

        for (uint32 i = 0; i < 10; i++) {
            CHECK_EQ(u[i], i + 1);
            CHECK_EQ(v[i], u[i]);
        }
    }

    SUBCASE("Move assignment constructor")
    {
        TArray<int32> u = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        TArray<int32> v = {11, 12, 13};

        v = std::move(u);

        CHECK_EQ(u.GetSize(), 0);
        CHECK_EQ(u.GetCapacity(), 0);
        CHECK(u.IsEmpty());

        CHECK_EQ(v.GetSize(), 10);
        CHECK_EQ(v.GetCapacity(), 10);
        CHECK_FALSE(v.IsEmpty());

        for (uint32 i = 0; i < 10; i++) {
            CHECK_EQ(v[i], i + 1);
        }
    }

    SUBCASE("Initial sized constructor")
    {
        TArray<MyStruct> u(5);

        CHECK_EQ(u.GetSize(), 5);
        CHECK_EQ(u.GetCapacity(), 5);
        CHECK_FALSE(u.IsEmpty());
    }

    SUBCASE("Insertions")
    {
        TArray<MyStruct> u;

        MyStruct sa;
        sa.a = 1;
        sa.b = 2.0f;

        MyStruct sb;
        sb.a = 10;
        sb.b = 20.0f;

        MyStruct sc;
        sc.a = 100;
        sc.b = 200.0f;

        MyStruct sd;
        sd.a = 29;
        sd.b = 19.93f;

        u.Emplace(sa);
        u.Emplace(sb);
        u.Emplace(sc);
        u.Emplace(sd);

        CHECK_EQ(u[0].a, 1);
        CHECK_EQ(u[1].a, 10);
        CHECK_EQ(u[2].a, 100);
        CHECK_EQ(u[3].a, 29);

        u.Add(sa);
        u.Add(sb);
        u.Add(std::move(sc));
        u.Add(sd);

        CHECK_EQ(u[4].a, 1);
        CHECK_EQ(u[5].a, 10);
        CHECK_EQ(u[6].a, 100);
        CHECK_EQ(u[7].a, 29);

        CHECK_EQ(u.GetSize(), 8);
        CHECK_EQ(u.GetCapacity(), 22);
    }

    SUBCASE("Remove At")
    {
        TArray<int32> u = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        u.RemoveAt(3);
        u.RemoveAt(3);
        u.RemoveAt(6);

        CHECK_EQ(u.GetSize(), 7);
        CHECK_EQ(u.GetCapacity(), 10);
        CHECK_FALSE(u.IsEmpty());

        CHECK_EQ(u[0], 1);
        CHECK_EQ(u[1], 2);
        CHECK_EQ(u[2], 3);
        CHECK_EQ(u[3], 6);
        CHECK_EQ(u[4], 7);
        CHECK_EQ(u[5], 8);
        CHECK_EQ(u[6], 10);
    }

    SUBCASE("Remove At Ranged")
    {
        TArray<int32> u = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        u.RemoveAt(3, 3);

        CHECK_EQ(u.GetSize(), 7);
        CHECK_EQ(u.GetCapacity(), 10);
        CHECK_FALSE(u.IsEmpty());

        CHECK_EQ(u[0], 1);
        CHECK_EQ(u[1], 2);
        CHECK_EQ(u[2], 3);
        CHECK_EQ(u[3], 7);
        CHECK_EQ(u[4], 8);
        CHECK_EQ(u[5], 9);
        CHECK_EQ(u[6], 10);
    }

    SUBCASE("Resize")
    {
        TArray<int32> u = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        u.AddSlots(5);

        CHECK_EQ(u.GetSize(), 5);
        CHECK_EQ(u.GetCapacity(), 10);

        u.AddSlots(15);

        CHECK_EQ(u.GetSize(), 15);
        CHECK_EQ(u.GetCapacity(), 15);
    }
}

TEST_SUITE_END();
