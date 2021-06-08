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
        String text = ATEXT("Olá, meu nome é Brian, eu sou de Uberlândia e gosto de maçãs.");

        CHECK_EQ(text.GetSize(), 62);
    }

    SUBCASE("Character pointer constructor char")
    {
        String text = "Olá, meu nome é Brian, eu sou de Uberlândia e gosto de maçãs.";

        CHECK_EQ(text.GetSize(), 62);
    }

    SUBCASE("Character pointer constructor wchar_t")
    {
        String text = L"Olá, meu nome é Brian, eu sou de Uberlândia e gosto de maçãs.";

        CHECK_EQ(text.GetSize(), 62);
    }
}

//TEST_CASE("[String] Formatter")
//{
//    String formatted
//      = StringUtils::Format(ATEXT("Olá, meu nome é {}, eu sou de {}, vulgo {1:s4} e gosto de {}. Eu tenho {} anos e peso {:3.5f} kg."),
//                            ATEXT("Brian"),
//                            ATEXT("Uberlândia"),
//                            ATEXT("Udia"),
//                            ATEXT("maçã"),
//                            29,
//                            71.3542f);
//
//    CHECK_EQ(formatted, ATEXT("Olá, meu nome é Brian, eu sou de Uberlândia, vulgo Udia e gosto de maçã. Eu tenho 29 anos e peso 72.35"));
//}
TEST_SUITE_END();
