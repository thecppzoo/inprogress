#include "meta/Indices.h"

/// pureconstexpr is meant for functions too slow to be called at runtime
namespace pureconstexpr {

    constexpr int log10floor(unsigned long arg) {
        return arg < 10 ? 0 : 1 + log10floor(arg / 10);
    }

    constexpr long unsigned exp10(unsigned exp) {
        return exp < 1 ? 1 : 10 * exp10_constexpr(exp - 1);
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

template<typename> struct Cutoff_helper;
template<unsigned long... p2s>
struct Cutoff<IndexPack<unsigned long, p2
    static long unsigned cutoff(unsigned a) {
        static constexpr unsigned long arr[] = {
            pureconstexpr::cutoff(1ul << p2s)...
        };
        return arr[a];
    }
};

long cutoff(unsigned p2) {
    using Ndxs = typename Indices<sizeof(long unsigned)*8 - 1>::type;
    return Cutoff<Ndxs>::cutoff(p2);
}

}

unsigned long digits10(unsigned long arg) {
    if(arg < 2) { return 1; }
    auto l2f = log2floor(arg);
    auto cut = detail::cutoff(l2f);
    auto adj = arg < cut ? 0 : 1;
    return l2f/3 + adj;
}

#include <iostream>

int main(int argc, const char *argv[]) {
    for(int i = 0; i < 64; ++i) {
        auto v = 1ul << i;
        std::cout << i << ' ' << v << ' ' << cutoff(1ul) << std::endl;
    }
    std::cout <<
        countDigits(9) << ' ' <<
        countDigits(10) << ' ' <<
        countDigits(99) << ' ' <<
        countDigits(100) << ' ' <<
        countDigits(65535) << ' ' <<
        countDigits(65536) << std::endl;
    std::cout << log2floor(8) << std::endl;
    return 0;
}
