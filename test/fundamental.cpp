
// easylazy
//
// Copyright iorate 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>
#define EASYLAZY_ENABLE_INTEGER
#include "../easylazy.hpp"

using namespace easylazy;

int main() {
    BOOST_TEST(bool_(true).get() == true);
    BOOST_TEST(char_('C').get() == 'C');
    BOOST_TEST(int_(10).get() == 10);
    BOOST_TEST(integer(42).get() == 42);
    BOOST_TEST(float_(1.5f).get() == 1.5f);
    BOOST_TEST(double_(2.5).get() == 2.5);

    BOOST_TEST('C'_c .get() == 'C');
    BOOST_TEST(10_d .get() == 10);
    BOOST_TEST(42_n .get() == 42);
    BOOST_TEST(1.5_f .get() == 1.5f);
    BOOST_TEST(2.5_lf .get() == 2.5);

    BOOST_TEST(10.5_f .get_as<int>() == 10);

    return boost::report_errors();
}
