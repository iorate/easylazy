
// easylazy
//
// Copyright iorate 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>
#include "../easylazy.hpp"

using namespace easylazy;

int main() {
    BOOST_TEST(
        (function<int_ (int_, int_)>([](int_ x, int_ y) { return x + y; })(3_d, 4_d).get())
            == 7
    );
    BOOST_TEST(
        (EASYLAZY_FUNCTION(int_ x, int_ y) { return x * y; })(3_d, 4_d).get()
            == 12
    );

    return boost::report_errors();
}
