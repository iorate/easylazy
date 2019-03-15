# easylazy

## General
`easylazy` provides basic components for lazy evaluation.

## Header `<easylazy.hpp>` synopsis
```cpp
#include <initializer_list>

namespace easylazy {
    // ## thunk
    template <class T> class thunk;

    using bool_ = thunk<bool>;
    using char_ = thunk<char>;
    using int_ = thunk<int>;
#ifdef EASYLAZY_ENABLE_INTEGER
    using integer = thunk<
        boost::multiprecision::number<
	    boost::multiprecision::cpp_int_backend<>,
	    boost::multiprecision::et_off
	>
    >;
#endif
    using float_ = thunk<float>;
    using double_ = thunk<double>;

    // ### partial specialization for functions
    template <class R, class ...Args>
    class thunk<std::function<R (Args...)>>;

    template <class Sig> using function = thunk<std::function<Sig>>;

    // ### partial specialization for lists
    template <class T> using list_rep = unspecified;

    template <class T> class thunk<list_rep<T>>;

    template <class T> using list = thunk<list_rep<T>>;

    using string = list<char_>;

    // ### suffix for `thunk` literals
    inline namespace literals {
        char_   operator"" _c (char c);
        int_    operator"" _d (unsigned long long ull);
#ifdef EASYLAZY_ENABLE_INTEGER
        integer operator"" _n (unsigned long long ull);
#endif
        float_  operator"" _f (long double ld);
        double_ operator"" _lf(long double ld);
        string  operator"" _s (char const *s, std::size_t n);
    }

    // ### macro for `function` lambdas
#define EASYLAZY_FUNCTION(...) unspecified

    // ## operators
    template <class T> thunk<see-below> operator+(thunk<T> x);
    template <class T> thunk<see-below> operator-(thunk<T> x);
    template <class T> thunk<see-below> operator!(thunk<T> x);
    template <class T> thunk<see-below> operator~(thunk<T> x);

    template <class T, class U> thunk<see-below> operator* (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator/ (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator% (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator+ (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator- (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator<<(thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator>>(thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator< (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator> (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator<=(thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator>=(thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator==(thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator!=(thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator& (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator^ (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator| (thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator&&(thunk<T> x, thunk<T> y);
    template <class T, class U> thunk<see-below> operator||(thunk<T> x, thunk<T> y);

    // ## list functions
    template <class T> bool_ operator< (list<T> xs, list<T> ys);
    template <class T> bool_ operator> (list<T> xs, list<T> ys);
    template <class T> bool_ operator<=(list<T> xs, list<T> ys);
    template <class T> bool_ operator>=(list<T> xs, list<T> ys);
    template <class T> bool_ operator==(list<T> xs, list<T> ys);
    template <class T> bool_ operator!=(list<T> xs, list<T> ys);

    template <class T, class U> list<U> map(function<U (T)> f, list<T> xs);
    template <class T> list<T> append(list<T> xs, list<T> ys);
    template <class T> list<T> filter(function<bool_ (T)> p, list<T> xs);
    template <class T> T head(list<T> xs);
    template <class T> T last(list<T> xs);
    template <class T> list<T> tail(list<T> xs);
    template <class T> list<T> init(list<T> xs);
    template <class T> bool_ null(list<T> xs);
    template <class T> int_ length(list<T> xs);
    template <class T> list<T> reverse(list<T> xs);
}
```

## Class template `thunk`
```cpp
namespace easylazy {
    template <class T> class thunk {
        std::shared_ptr<std::variant<T, std::function<thunk ()>>> pimpl; // exposition only

    public:
        using type = T;

        // ### constructors
        template <class U> explicit thunk(U &&u);
        template <class F> explicit thunk(F &&f);
        template <class U> EXPLICIT thunk(thunk<U> x);

        // ### resolvers
        T get() const;
        template <class U> U get_as() const;

        // ### convertion to `bool`
        explicit operator bool() const;
    };
}
```

### Constructors

```cpp
template <class U> explicit thunk(U &&u);
```

Effects: Constructs a thunk that has an already evaluated value of type `T` initialized with the expression `std::forward<U>(u)`.

Remarks: This constructor does not participate in overload resolution unless `std::is_constructible_v<T, U>` is `true`.

```cpp
template <class F> explicit thunk(F &&f);
```

Effects: Constructs a thunk that has a computation of type `() -> thunk<T>` initialized with `std::forward<F>(f)` to be lazily evaluated.

Remarks: This constructor does not participate in overload resolution unless `std::decay_t<F>` is Lvalue-Callable for no arguments and return type `thunk<T>`.

```cpp
template <class U> EXPLICIT thunk(thunk<U> x);
```

Effects: Constructs a thunk that has a computaion `T(x.get())` to be lazily evaluated.

Remarks: This constructor does not participate in overload resolution unless `std::is_constructible_v<T, U>` is `true`. This constructor is `explicit` if and only if `std::is_convertible_v<U, T>` is `false`.

### Resolvers
```cpp
T get() const;
```

Effects: Resolve the thunk; that is, run a computaion yet to be evaluated and save an evaluated value.

Returns: An evaluated value.

Remarks: Although this function is marked as `const`, it may alter the internal state of the thunk.

```cpp
template <class U> get_as() const;
```

Effects: Resolve the thunk.

Returns: `U(get())`.

### Convertion to `bool`
```cpp
explicit operator bool() const;
```

Effects: Resolve the thunk.

Returns: `bool(get())`.

### Partial specialization for functions
```cpp
namespace easylazy {
    template <class R, class ...Args> class thunk<std::function<R (Args...)>> {
        std::shared_ptr<std::variant<std::function<R (Args...)>, std::function<thunk ()>>> pimpl;
            // exposition only

    public:
        using type = std::function<R (Args...)>;

        template <class U> explicit thunk(U &&u);
        template <class F> explicit thunk(F &&f);
        template <class U> EXPLICIT thunk(thunk<U> x);

        std::function<R (Args...)> get() const;
        template <class U> U get_as() const;

        explicit operator bool() const;

        R operator()(Args ...args) const;
    };
}
```

`thunk<std::function<R (Args...)>>`, a.k.a. `function<R (Args...)>`, represents a lazy function of type `(Args...) -> R`. It provides a lazy function-call operator.

```cpp
R operator()(Args ...args) const;
```

Returns: A thunk of type `R` initialized with a computation `get()(args...)` to be lazily evaluated.

### Partial specialization for lists
```cpp
namespace easylazy {
    template <class T> class thunk<list_rep<T>> {
        std::shared_ptr<std::variant<list_rep<T>, std::function<thunk ()>>> pimpl;
            // exposition only

    public:
        using type = list_rep<T>;

        template <class U> explicit thunk(U &&u);
        template <class F> explicit thunk(F &&f);
        template <class U> EXPLICIT thunk(thunk<U> x);

        template <class Range> explicit thunk(Range &&r);
        template <class U> explicit thunk(std::initializer_list<U> il);

        list_rep<T> get() const;

        template <class Container> Container get_as() const;

        explicit operator bool() const;

        T operator[](int_ n) const;
    };
}
```

`thunk<list_rep<T>>`, a.k.a. `list<T>`, represents a lazy list of type `[T]`. It provides constructors receiving a range, and a lazy subscript operator.

```cpp
template <class Range> explicit thunk(Range &&r);
```

Effects: Construct a list that contains sub-thunks initialized with the objects in the range [`std::begin(r)`, `std::end(r)`). 

Remarks: This constructor does not participate in overload resolution unless `T(*std::begin(r))` is a valid expression.

\[Example:
```cpp
std::vector<int> v{1, 2, 3};
list<int_> xs(v);
```

-- end example]

```cpp
template <class U> explicit thunk(std::initializer_list<U> il);
```

Effects: Construct a list that contains sub-thunks initialized with the objects in the range [`il.begin()`, `il.end()`). 

Remarks: This constructor does not participate in overload resolution unless `std::is_constructible_v<T, U const &>` is `true`.

\[Example:
```cpp
list<int_> xs{1, 2, 3};
```

-- end example]

```cpp
template <class Container> Container get_as() const;
```

Effects: Resolve the thunk and all its sub-thunks.

Returns: A container object that contains evaluated values.

\[Example:
```cpp
string hello = "Hello, world!"_s;
std::cout << hello.get_as<std::string>() << std::endl;
```

-- end example]

```cpp
T operator[](int_ n) const;
```

Returns: A thunk initialized with a computation returning the `n`th sub-thunk to be lazily evaluated.

Throws: `std::out_of_range` when a thunk is resolved if `n` is negative, or greater than or equal to the length of the thunk.

### Suffix for `thunk` literals
```cpp
char_ operator"" _c(char c);
```

Returns: `char_(c)`.

```cpp
int_ operator"" _d(unsigned long long ull);
```

Returns: `int_(ull)`.

```cpp
integer operator"" _n(unsigned long long ull);
```

Returns: `integer(ull)`.

Remarks: The type alias `integer` and the user-defined literal `_n` is defined if and only if the macro `EASYLAZY_ENABLE_INTEGER` is defined.

```cpp
float_ operator"" _f(long double ld);
```

Returns: `float_(ld)`.

```cpp
double_ operator"" _lf(long double ld);
```

Returns: `double_(ld)`.

```cpp
string operator"" _s (char const *s, std::size_t n);
```

Returns: `string(std::string_view(s, n))`.

\[Example:
```cpp
string hello = "Hello, world!"_s;
```

-- end example]

### Macro for `function` lambdas
```cpp
#define EASYLAZY_FUNCTION(...) unspecified
```

This macro constructs a thunk of type `function<R (Args...)>` from a lambda expression of type `(Args...) -> R`. Local variables are captured.

\[Example:
```cpp
list<int_> multiply(int_ x, list<int_> ys) {
    return list<int_>([=]() {
        return map(EASYLAZY_FUNCTION(int_ y) { return x * y; }, ys);
        // Equivalent to `map(function<int_ (int_)>([=](int_ y) { return x * y; }), ys)`
    });
}
```

-- end example]

## Operators
The operators are lazy.

```cpp
template <class T> thunk<see-below> operator+(thunk<T> x);
template <class T> thunk<see-below> operator-(thunk<T> x);
template <class T> thunk<see-below> operator!(thunk<T> x);
template <class T> thunk<see-below> operator~(thunk<T> x);
```

Returns: Let `op` be a unary operator. `op x` returns a thunk initialized with a computation `op x.get()` to be lazily evaluated, which type is `thunk<decltype(op x.get())>`.

```cpp
template <class T, class U> thunk<see-below> operator* (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator/ (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator% (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator+ (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator- (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator<<(thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator>>(thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator< (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator> (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator<=(thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator>=(thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator==(thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator!=(thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator& (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator^ (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator| (thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator&&(thunk<T> x, thunk<T> y);
template <class T, class U> thunk<see-below> operator||(thunk<T> x, thunk<T> y);
```

Returns: Let `op` be a binary operator. `x op y` returns a thunk initialized with a computation `x.get() op y.get()` to be lazily evaluated, which type is `thunk<decltype(x.get() op y.get())>`.

Remarks: For logical operators, short-circuit evaluation is used. \[Example: `(bool_(true) || bool_(1_d / 0_d)).get()` does not raise division by zero. -- end example]

## List functions
```cpp
template <class T> bool_ operator< (list<T> xs, list<T> ys);
template <class T> bool_ operator> (list<T> xs, list<T> ys);
template <class T> bool_ operator<=(list<T> xs, list<T> ys);
template <class T> bool_ operator>=(list<T> xs, list<T> ys);
template <class T> bool_ operator==(list<T> xs, list<T> ys);
template <class T> bool_ operator!=(list<T> xs, list<T> ys);
```

Returns: A thunk initialized with a computation comparing two lists to be lazily evaluated.

```cpp
template <class T, class U> list<U> map(function<U (T)> f, list<T> xs);
template <class T> list<T> append(list<T> xs, list<T> ys);
template <class T> list<T> filter(function<bool_ (T)> p, list<T> xs);
template <class T> T head(list<T> xs);
template <class T> T last(list<T> xs);
template <class T> list<T> tail(list<T> xs);
template <class T> list<T> init(list<T> xs);
template <class T> bool_ null(list<T> xs);
template <class T> int_ length(list<T> xs);
template <class T> list<T> reverse(list<T> xs);
```

Some rudimentary lazy list functions are provided. See also [Haskell Prelude](https://www.haskell.org/onlinereport/haskell2010/haskellch9.html#x16-1720009.1).
