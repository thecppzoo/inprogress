#include "digits10/digits10.h"

static_assert(10 == digits10(unsigned(-1)), "");

#include "bmark/benchmark.h"

#include <iostream>

unsigned digits10_naive(unsigned long arg) {
    auto rv = 1;
    while(10 <= arg) {
        ++rv;
        arg /= 10;
    }
    return rv;
}

unsigned digits10_alexandrescu(unsigned long arg) {
    auto rv = 1;
    for(;;) {
        if(arg < 10) return rv;
        if(arg < 100) return rv + 1;
        if(arg < 1000) return rv + 2;
        if(arg < 10000) return rv + 3;
        arg /= 10000;
        rv += 4;
    }
}

unsigned digits10_mul(unsigned long arg) {
    if(pureconstexpr::exp10(19) <= arg) { return 20; }
    auto rv = 1;
    auto m = 10l;
    while(m <= arg) {
        ++rv;
        m *= 10;
    }
    return rv;
}

unsigned digits10_mul10000(unsigned long arg) {
    constexpr auto tenTo16 = pureconstexpr::exp10(16);
    if(tenTo16 <= arg) { return 16 + digits10_mul10000(arg/tenTo16); }
    auto rv = 1;
    auto m = 10000ul;
    while(m <= arg) {
        rv += 4;
        m *= 10000;
    }
    if(m <= arg*10) { return rv + 3; }
    if(m <= arg*100) { return rv + 2; }
    if(m <= arg*1000) { return rv + 1; }
    return rv;
}

unsigned digits10_divFactored(unsigned long arg) {
    auto rv = 1;
    while(10000 <= arg) {
        rv += 4;
        arg /= 10000;
    }
    if(arg < 10) { return rv; }
    if(arg < 100) { return rv + 1; }
    if(arg < 1000) { return rv + 2; }
    return rv + 3;
}

struct Experiment {
    Experiment(long bottom, long top): bottom(bottom), top(top) {}
    long
        top,
        bottom,
        accumulator = 0;
};

template<typename Fun>
auto callSequentialBenchmark(
    std::string name,
    Fun &&f,
    double base,
    long bottom,
    long top
) {
    Experiment e(bottom, top);
    auto rv =
        bmark::benchmark(
            [&]() {
                unsigned long
                    bottom = e.bottom,
                    top = e.top;
                long accumulator = 0;
                while(bottom < top--) {
                    accumulator += f(top);
                }
                e.accumulator = accumulator;
            }
        );
    std::cout << e.accumulator << ' ' << rv << ' ' <<
        rv/base << ' ' << name << std::endl;
    return rv;
}

#define sequentialExperiment(fun, comparator, bottom, top)\
    callSequentialBenchmark(#fun, fun, comparator, bottom, top)

#include <random>

constexpr unsigned digits10branch(unsigned long arg) {
    if(arg < 10) { return 1; }
    auto l2f = log2floor(arg);
    using Combination = detail::P10<meta::Indices<sizeof(long unsigned)*8 - 1>>; 
    auto cut = arg < Combination::nextPowerOf10(l2f) ? 0 : 1;
    auto base = Combination::ndigits(l2f);
    return cut + base;
}

int main(int argc, const char *argv[]) {
    constexpr auto top = 200l * 1000 * 1000;// * 1000;
    constexpr auto bottom = 0;

    if(argc < 2) {
        auto base = sequentialExperiment(digits10, 0, bottom, top);
        sequentialExperiment(digits10_divFactored, base, bottom, top);
        sequentialExperiment(digits10_alexandrescu, base, bottom, top);
        sequentialExperiment(digits10_mul, base, bottom, top);
        sequentialExperiment(digits10_naive, base, bottom, top);
        sequentialExperiment(digits10, base, bottom, top);
        sequentialExperiment(digits10_mul10000, base, bottom, top);
        return 0;
    }
    std::random_device device;
    auto seed = device();
    constexpr auto count = 2l*1000*1000*1000;
    std::minstd_rand engine(seed);
    auto randomInputExperiment =
        [](auto name, auto d10, auto seed, long empty, double base) {
            std::minstd_rand engine(seed);
            auto acc = 0l;
            auto driver = [&]() { acc += d10(engine()); };
            auto rv = bmark::countedBenchmark(driver, count);
            std::cout << name << ' ' << rv << ' ' << (rv - empty)/base <<
                ' ' << acc << std::endl;
            return rv;
        };
    auto empty =
        randomInputExperiment(
            "JustRandom", [](auto v) { return v; }, seed, 0, 0
        );
    auto toTen = [](auto a) { return digits10(a % 16); };
    auto toTenF = [](auto a) { return digits10branch(a % 16); };
    auto d10 =
        randomInputExperiment("digits10", digits10, seed, empty, 0) - empty;
    randomInputExperiment("Alexandrescu", digits10_alexandrescu, seed, empty, d10);
    auto base2 =
        randomInputExperiment("digits10-20", toTen, seed, empty, d10) - empty;
    randomInputExperiment("digits10-20slow", toTenF, seed, empty, base2);
    randomInputExperiment(
        "Alexandrescu-20",
        [](auto a) { return digits10_alexandrescu(a % 16); },
        seed, empty, base2
    );
    randomInputExperiment(
        "mul",
        [](auto a) { return digits10_mul10000(a % 16); },
        seed, empty, base2
    );
    return 0;
}
