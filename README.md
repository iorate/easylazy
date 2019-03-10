# easylazy
Provides basic components for lazy evaluation in C++. Requires C++17. Tested under GCC 8.3.0, Clang 7.0.1, and MSVC 15.9.8.

## Example
### [Tarai function](https://en.wikipedia.org/wiki/Tak_%28function%29)

```cpp
#include <iostream>
#include <easylazy.hpp>

namespace el = easylazy;
using namespace el::literals;

// Use a thunk type `int_` instead of `int`.
el::int_ tarai(el::int_ x, el::int_ y, el::int_ z) {
    // Wrap a computaion into a lambda expression for lazy evaluation.
    return el::int_([=]() {
        if (x <= y) {
            return y;
        } else {
       	    // Use a user-defined literal `1_d` instead of `1`.
            return tarai(tarai(x - 1_d, y, z), tarai(y - 1_d, z, x), tarai(z - 1_d, x, y));
        }
    });
}

int main() {
    // `n` is not evaluated here.
    el::int_ n = tarai(200_d, 100_d, 0_d);

    // `n.get()` fires evaluation of `n`. It takes not so long time because of lazy evaluation.
    std::cout << n.get() << std::endl;
}
```

### [Fibonacci numbers](https://en.wikipedia.org/wiki/Fibonacci_number) as a lazy list
You may need to increase the stack size to run this example. For example, `g++ -std=c++17 -Wl,--stack,10485760 fibs.cpp`.

```cpp
#include <iostream>
#define EASYLAZY_ENABLE_INTEGER
#include <easylazy.hpp>

namespace el = easylazy;
using namespace el::literals;

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

template <class T>
el::list<T> fibs() {
    // Use a local static variable to share a thunk.
    static el::list<T> inst([]() {
        return el::cons(T(0), el::cons(T(1),
            zip_with_s(EASYLAZY_FUNCTION(T x, T y) { return x + y; }, fibs<T>(), el::tail(fibs<T>()))
        ));
    });
    return inst;
}

int main() {
    el::integer n = fibs<el::integer>()[10000_d];

    std::cout << n.get() << std::endl;
}
```

## Author
[iorate](https://github.com/iorate) ([Twitter](https://twitter.com/iorate))

## License
[Boost Software License](LICENSE_1_0.txt)
