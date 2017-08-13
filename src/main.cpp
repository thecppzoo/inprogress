#include "digits10/digits10.h"

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
    auto rv = 1;
    auto m = 10l;
    while(m <= arg) {
        ++rv;
        m *= 10;
    }
    return rv;
}

struct Experiment {
    static constexpr auto count = 20l * 1000 * 1000 * 1000;
    long counter = count;
    long accumulator = 0;
};

template<typename Fun> auto caller(Fun &&f, double base) {
    Experiment e;
    auto rv =
        bmark::benchmark(
            [&]() {
                while(e.counter--) {
                    e.accumulator += f(e.counter);
                }
            }
        );
    std::cout << e.accumulator << ' ' << rv << ' ' << rv/base << std::endl;
    return rv;
}

#include <random>

int main(int argc, const char *argv[]) {
    if(argc < 1) {
        auto base = caller(digits10, 0);
        caller(digits10_alexandrescu, base);
        caller(digits10_mul, base);
        caller(digits10_naive, base);
        return 0;
    }
    std::random_device device;
    auto seed = device();
    auto count = 1000*1000*1000;
    std::minstd_rand engine(seed);
    auto acc = 0l;
    auto driver = [&](unsigned (*digits)(unsigned long)) {
        acc = 0;
        for(auto c = count; c--; ) {
            auto v = engine();
            acc += digits(v);
        }
    };
    auto nothing = bmark::benchmark(driver, [](unsigned long) -> unsigned { return 0; });
    std::cout << nothing << ' ' << engine() << std::endl;
    engine = std::minstd_rand(seed);
    auto base = double(bmark::benchmark(driver, digits10) - nothing);
    std::cout << base << ' ' << acc << ' ' << engine() << std::endl;
    acc = 0;
    engine = std::minstd_rand(seed);
    auto alexandrescu = bmark::benchmark(driver, digits10_alexandrescu);
    std::cout << alexandrescu << ' ' << acc << ' ' <<
        (alexandrescu - nothing)/base << ' ' << engine() << std::endl;
    acc = 0;
    engine = std::minstd_rand(seed);
    auto naive = bmark::benchmark(driver, digits10_mul);
    std::cout << naive << ' ' << acc << ' ' <<
        (naive - nothing)/base << ' ' << engine() << std::endl;
    acc = 0;
    engine = std::minstd_rand(seed);
    auto repeat = bmark::benchmark(driver, digits10);
    std::cout << repeat << ' ' << (repeat - nothing)/base << ' ' << acc << std::endl;
    return 0;
}
