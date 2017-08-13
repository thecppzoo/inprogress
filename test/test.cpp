
#ifdef TESTS
    #define CATCH_CONFIG_MAIN
#else
    #define CATCH_CONFIG_RUNNER
#endif
#include "catch.hpp"

#include "digits10/digits10.h"

TEST_CASE("ZeroAndOne", "[digits10]") {
    REQUIRE(1 == digits10(0));
    REQUIRE(1 == digits10(1));
}

TEST_CASE("JustBefore3digits", "[digits10]") {
    REQUIRE(2 == digits10(99));
}

TEST_CASE("Starting3digits", "[digits10]") {
    REQUIRE(3 == digits10(100));
}

TEST_CASE("IndifirentToPowersOfTwo", "[digits10]") {
    REQUIRE(5 == digits10((1 << 16) - 1));
    REQUIRE(5 == digits10(1 << 16));
}

TEST_CASE("Comprehensive", "[digits10]") {
    auto last = 1ul << 63;
    auto expected = 1;
    auto val = 1ul;
    for(; val < last; val *= 10) {
        REQUIRE(digits10(val) == expected);
        ++expected;
    }
    REQUIRE(digits10(val) == expected);
}
