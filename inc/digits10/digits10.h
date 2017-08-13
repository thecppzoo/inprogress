#include "meta/Indices.h"

/// pureconstexpr is meant for functions too slow to be called at runtime
namespace pureconstexpr {

    constexpr int log10floor(unsigned long arg) {
        return arg < 10 ? 0 : 1 + log10floor(arg / 10);
    }

    constexpr long unsigned exp10(unsigned exp) {
        return exp < 1 ? 1 : 10 * exp10(exp - 1);
    }

}

constexpr unsigned log2floor(unsigned long arg) {
    auto leadingZeroes = __builtin_clzll(arg);
    auto rv = sizeof(arg)*8 - 1 - leadingZeroes;
    return rv; 
}

namespace pureconstexpr {

    constexpr unsigned long cutoff(unsigned long arg) {
        auto l10 = log10floor(arg);
        return exp10(l10 + 1);
    }

}

namespace detail {

template<typename> struct Cutoff;
template<unsigned long... p2s>
struct Cutoff<meta::IndexPack<unsigned long, p2s...>> {
    static long unsigned cutoff(unsigned p2) {
        static constexpr unsigned long arr[] = {
            pureconstexpr::cutoff(1ul << p2s)...
        };
        return arr[p2];
    }

    static long unsigned ndigits(unsigned p2) {
        static constexpr unsigned long arr[] = {
            pureconstexpr::log10floor(1ul << p2s)...
        };
        return arr[p2];
    }
};

}

unsigned long digits10(unsigned long arg) {
    if(arg < 2) { return 1; }
    auto l2f = log2floor(arg);
    using Combination = detail::Cutoff<meta::Indices<sizeof(long unsigned)*8 - 1>>; 
    auto cut = arg < Combination::cutoff(l2f) ? 0 : 1;
    auto base = 1 + Combination::ndigits(l2f);
    return cut + base;
}
