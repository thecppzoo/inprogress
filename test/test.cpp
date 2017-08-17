
#ifdef TESTS
    #define CATCH_CONFIG_MAIN
#else
    #define CATCH_CONFIG_RUNNER
#endif
#include "catch.hpp"

#include "digits10/digits10.h"

auto digits10f = digits10;

TEST_CASE("ZeroAndOne", "[digits10]") {
    REQUIRE(1 == digits10f(0));
    REQUIRE(1 == digits10f(1));
}

TEST_CASE("JustBefore3digits", "[digits10]") {
    REQUIRE(2 == digits10f(99));
}

TEST_CASE("Starting3digits", "[digits10]") {
    REQUIRE(3 == digits10f(100));
}

TEST_CASE("IndifirentToPowersOfTwo", "[digits10]") {
    REQUIRE(5 == digits10f((1 << 16) - 1));
    REQUIRE(5 == digits10f(1 << 16));
}

TEST_CASE("Comprehensive", "[digits10]") {
    auto last = 1ul << 63;
    auto expected = 1;
    auto val = 1ul;
    while(val < last) {
        REQUIRE(digits10f(val) == expected);
        val *= 10;
        REQUIRE(digits10f(val - 1) == expected);
        ++expected;
    }
    REQUIRE(digits10f(val) == expected);
}
