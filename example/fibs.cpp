
// easylazy
//
// Copyright iorate 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#define EASYLAZY_ENABLE_INTEGER
#include "../easylazy.hpp"

namespace el = easylazy;
using namespace el::literals;

// zipWith' :: (a -> b -> c) -> [a] -> [b] -> [c]
// zipWith' f [] _ = []
// zipWith' f _ [] = []
// zipWith' f (x:xs) (y:ys) = let z = f x y in z `seq` z : zipWith' f xs ys
template <class T, class U, class V>
el::list<U> zip_with_s(el::function<V (T, U)> f, el::list<T> x_xs, el::list<U> y_ys) {
    return el::list<U>([=]() {
        if (el::null(x_xs) || el::null(y_ys)) {
            return el::nil<V>();
        } else {
            V z = f(el::head(x_xs), el::head(y_ys));
            z.get();
            return el::cons(z, zip_with_s(f, el::tail(x_xs), el::tail(y_ys)));
        }
    });
}

// fibs :: Integral a => [a]
// fibs = 0 : 1 : zipWith' (+) fibs (tail fibs)
template <class T>
el::list<T> fibs() {
    static el::list<T> inst([]() {
        return el::cons(T(0), el::cons(T(1),
            zip_with_s(EASYLAZY_FUNCTION(T x, T y) { return x + y; }, fibs<T>(), el::tail(fibs<T>()))
        ));
    });
    return inst;
}

int main() {
    // n :: Integer
    // n = fibs !! 10000
    el::integer n = fibs<el::integer>()[10000_d];

    std::cout << n.get() << std::endl;
}
