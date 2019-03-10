
// easylazy
//
// Copyright iorate 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include "../easylazy.hpp"

namespace el = easylazy;
using namespace el::literals;

// nats :: [Int]
// nats = 0 : map (+1) nats
el::list<el::int_> nats() {
    static el::list<el::int_> inst([]() {
        return el::cons(0_d, el::map(EASYLAZY_FUNCTION(el::int_ x) { return x + 1_d; }, nats()));
    });
    return inst;
}

int main() {
    // n :: Int
    // n = nats !! 100
    el::int_ n = nats()[100_d];

    std::cout << n.get() << std::endl;
}
