#pragma once

#include "Containers/Array.h"
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Containers");

TEST_CASE("[TArray]: Constructors and assignments")
{
    SUBCASE("Default constructor")
    {
        TArray<int32> u;
    }
}

TEST_SUITE_END();
