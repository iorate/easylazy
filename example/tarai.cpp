
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

// tarai:: Int -> Int -> Int -> Int
// tarai x y z = if x <= y then y else tarai (tarai (x-1) y z) (tarai (y-1) z x) (tarai (z-1) x y)
el::int_ tarai(el::int_ x, el::int_ y, el::int_ z) {
    return el::int_([=]() {
        if (x <= y) {
            return y;
        } else {
            return tarai(tarai(x - 1_d, y, z), tarai(y - 1_d, z, x), tarai(z - 1_d, x, y));
        }
    });
}

int main() {
    // n :: Int
    // n = tarai 200 100 0
    el::int_ n = tarai(200_d, 100_d, 0_d);

    std::cout << n.get() << std::endl;
}
