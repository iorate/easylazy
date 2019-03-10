
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
    // Postfix Expressions
    BOOST_TEST((list<int_>{23, 42}[1_d].get()) == 42);
    BOOST_TEST((EASYLAZY_FUNCTION(int_ x, int_ y) { return x + y; })(3_d, 4_d).get() == 7);
    BOOST_TEST(double_(10_d).get() == 10.0);

    // Unary Expressions
    BOOST_TEST((+4_d).get() == 4);
    BOOST_TEST((-2.5_lf).get() == -2.5);
    BOOST_TEST((!bool_(false)).get() == true);
    BOOST_TEST((!12_d).get() == false);
    BOOST_TEST((~0_d).get() == -1);

    // Explicit Type Conversion
    BOOST_TEST((float_)1_d == 1.0_f);

    // Multiplicative Operators
    BOOST_TEST((1.5_lf * 4_d).get() == 6.0);
    BOOST_TEST((19_d / 4_d).get() == 4);
    BOOST_TEST((19_d % 4_d).get() == 3);

    // Additive Operators
    BOOST_TEST((3_d + 4.5_lf).get() == 7.5);
    BOOST_TEST(('9'_c - '3'_c).get() == 6);

    // Shift Operators
    BOOST_TEST((18_d << 2_d).get() == 72);
    BOOST_TEST((18_d >> 2_d).get() == 4);

    // Relational Operators
    BOOST_TEST((1.5_f < 3.5_f).get() == true);
    BOOST_TEST((1.5_f < 1.5_f).get() == false);
    BOOST_TEST((35_d > 15_d).get() == true);
    BOOST_TEST((35_d > 35_d).get() == false);
    BOOST_TEST((1.5_f <= 1.5_f).get() == true);
    BOOST_TEST((1.5_f <= 0.5_f).get() == false);
    BOOST_TEST((15_d >= 15_d).get() == true);
    BOOST_TEST((5_d >= 15_d).get() == false);

    // Equality Operators
    BOOST_TEST((10_d == 10_d).get() == true);
    BOOST_TEST((8.0_f == 1.5_f).get() == false);
    BOOST_TEST((80_d != 15_d).get() == true);
    BOOST_TEST((1.0_f != 1.0_f).get() == false);

    // Bitwise Operators
    BOOST_TEST((12_d & 10_d).get() == 8);
    BOOST_TEST((12_d ^ 10_d).get() == 6);
    BOOST_TEST((12_d | 10_d).get() == 14);

    // Logical Operators
    BOOST_TEST((bool_(true) && bool_(true)).get() == true);
    BOOST_TEST((bool_(true) && 0_d).get() == false);
    BOOST_TEST((bool_(false) || bool_(true)).get() == true);
    BOOST_TEST((bool_(false) || 0_d).get() == false);

    // Implicit Type Conversion
    int_ n = ' '_c;
    BOOST_TEST(n.get() == 0x20);
    if ('\0'_c) {
        BOOST_ERROR("not reached");
    }

    return boost::report_errors();
}
