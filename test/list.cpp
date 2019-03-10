
// easylazy
//
// Copyright iorate 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <vector>
#include <boost/core/lightweight_test.hpp>
#include "../easylazy.hpp"

using namespace easylazy;

int main() {
    std::vector<int> v{1, 2, 3};
    list<int_> xs1(v);
    BOOST_TEST(xs1.get_as<std::vector<int>>() == v);
    list<int_> xs2{1, 2, 3};
    BOOST_TEST(xs2.get_as<std::vector<int>>() == v);
    list<int_> xs3(cons(1_d, cons(2_d, cons(3_d, nil<int_>()))));
    BOOST_TEST(xs3.get_as<std::vector<int>>() == v);

    string s = "hello"_s;
    BOOST_TEST(s.get_as<std::string>() == "hello");

    BOOST_TEST((list<int_>{1, 2}[1_d].get()) == 2);
    BOOST_TEST_THROWS((list<int_>{1, 2}[2_d].get()), std::out_of_range);

    BOOST_TEST((nil<int_>() < nil<int_>()).get() == false);
    BOOST_TEST((nil<int_>() < list<int_>{1, 2}).get() == true);
    BOOST_TEST((list<int_>{1, 2} > list<int_>{1}).get() == true);
    BOOST_TEST((list<int_>{1, 2} > list<int_>{2}).get() == false);
    BOOST_TEST((list<int_>{1, 1} <= list<int_>{1, 2}).get() == true);
    BOOST_TEST((list<int_>{1, 2} <= list<int_>{1, 2}).get() == true);
    BOOST_TEST((list<int_>{1, 2} >= list<int_>{1, 3}).get() == false);
    BOOST_TEST((list<int_>{1, 2} >= list<int_>{1, 2, 3}).get() == false);

    BOOST_TEST((nil<int_>() == nil<int_>()).get() == true);
    BOOST_TEST((nil<int_>() == list<int_>{1, 2}).get() == false);
    BOOST_TEST((list<int_>{1} == list<int_>{1, 2}).get() == false);
    BOOST_TEST((list<int_>{1, 2} != list<int_>{1, 2}).get() == false);
    BOOST_TEST((list<int_>{1, 2} != list<int_>{1, 3}).get() == true);

    BOOST_TEST(
        map(EASYLAZY_FUNCTION(int_ x) { return x * 2_d; }, list<int_>{1, 2})
            == (list<int_>{2, 4})
    );

    BOOST_TEST(
        append(list<int_>{1, 2}, list<int_>{3, 4})
            == (list<int_>{1, 2, 3, 4})
    );

    BOOST_TEST(
        filter(EASYLAZY_FUNCTION(int_ x) { return x % 2_d == 0_d; }, list<int_>{1, 2, 3, 4})
            == (list<int_>{2, 4})
    );

    BOOST_TEST(head(list<int_>{1, 2}) == 1_d);
    BOOST_TEST_THROWS(head(nil<int_>()).get(), std::invalid_argument);

    BOOST_TEST(last(list<int_>{1, 2}) == 2_d);
    BOOST_TEST_THROWS(last(nil<int_>()).get(), std::invalid_argument);

    BOOST_TEST(tail(list<int_>{1, 2}) == list<int_>{2});
    BOOST_TEST_THROWS(tail(nil<int_>()).get(), std::invalid_argument);

    BOOST_TEST(init(list<int_>{1, 2}) == list<int_>{1});
    BOOST_TEST_THROWS(init(nil<int_>()).get(), std::invalid_argument);

    BOOST_TEST(!null(list<int_>{1, 2}));
    BOOST_TEST(null(nil<int_>()));

    BOOST_TEST(length(list<int_>{1, 2}) == 2_d);
    BOOST_TEST(length(nil<int_>()) == 0_d);

    BOOST_TEST(reverse(list<int_>{1, 2}) == (list<int_>{2, 1}));

    return boost::report_errors();
}
