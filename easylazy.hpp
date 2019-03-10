
// easylazy
//
// Copyright iorate 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef EASYLAZY_HPP_INCLUDED
#define EASYLAZY_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#ifdef EASYLAZY_ENABLE_INTEGER
#include <boost/multiprecision/cpp_int.hpp>
#endif

namespace easylazy {

// Thunk
template <class T>
class thunk;

namespace detail {

template <class T>
class thunk_base {
    using impl = std::variant<T, std::function<thunk<T> ()>>;

    std::shared_ptr<impl> pimpl;

public:
    using type = T;

    template <
        class U,
        std::enable_if_t<
            std::conjunction_v<
                std::negation<std::is_same<std::decay_t<U>, thunk_base>>,
                std::negation<std::is_same<std::decay_t<U>, thunk<T>>>,
                std::is_constructible<T, U>
            >
        > * = nullptr
    >
    explicit thunk_base(U &&u) :
        pimpl(std::make_shared<impl>(std::in_place_index<0>, std::forward<U>(u))) {
    }

    template <
        class F,
        std::enable_if_t<
            std::conjunction_v<
                std::negation<std::is_same<std::decay_t<F>, thunk_base>>,
                std::negation<std::is_same<std::decay_t<F>, thunk<T>>>,
                std::negation<std::is_constructible<T, F>>,
                std::is_invocable_r<thunk<T>, std::decay_t<F> &>
            >
        > * = nullptr
    >
    explicit thunk_base(F &&f) :
        pimpl(std::make_shared<impl>(std::in_place_index<1>, std::forward<F>(f))) {
    }

    template <
        class U,
        std::enable_if_t<
            std::conjunction_v<
                std::negation<std::is_same<U, T>>,
                std::is_constructible<T, U>,
                std::is_convertible<U, T>
            >
        > * = nullptr
    >
    thunk_base(thunk<U> x) :
        thunk_base([=]() {
            return thunk<T>(T(x.get()));
        }) {
    }

    template <
        class U,
        std::enable_if_t<
            std::conjunction_v<
                std::negation<std::is_same<U, T>>,
                std::is_constructible<T, U>,
                std::negation<std::is_convertible<U, T>>
            >
        > * = nullptr
    >
    explicit thunk_base(thunk<U> x) :
        thunk_base([=]() {
            return thunk<T>(T(x.get()));
        }) {
    }

    T get() const {
        if (pimpl->index() == 0) {
            return std::get<0>(*pimpl);
        } else {
            return pimpl->template emplace<0>(std::get<1>(*pimpl)().get());
        }
    }

    explicit operator bool() const {
        return bool(get());
    }
};

} // namespace detail {

template <class T>
class thunk :
    public detail::thunk_base<T> {
public:
    using detail::thunk_base<T>::thunk_base;

    template <class U>
    U get_as() const {
        return U(this->get());
    }
};

// Types
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

namespace detail {

template <class Sig>
class function_rep :
    public std::function<Sig> {
public:
    using std::function<Sig>::function;
};

} // namespace detail {

template <class R, class ...Args>
class thunk<detail::function_rep<R (Args...)>> :
    public detail::thunk_base<detail::function_rep<R (Args...)>> {

public:
    using detail::thunk_base<detail::function_rep<R (Args...)>>::thunk_base;

    template <class U>
    U get_as() const {
        return U(this->get());
    }

    R operator()(Args ...args) const {
        // Visual C++ 15.9.8 does not implement [*this] properly.
        return R([=, self = *this]() {
            return self.get()(args...);
        });
    }
};

template <class Sig>
using function = thunk<detail::function_rep<Sig>>;

template <class T>
class list_rep :
    public std::variant<std::tuple<>, std::tuple<T, thunk<list_rep<T>>>> {
public:
    using std::variant<std::tuple<>, std::tuple<T, thunk<list_rep<T>>>>::variant;
};

template <class T>
class thunk<list_rep<T>> :
    public detail::thunk_base<list_rep<T>> {

    template <class Iterator, class Sentinel>
    static thunk from_range(Iterator it, Sentinel end) {
        if (it == end) {
            return thunk(list_rep<T>(std::in_place_index<0>));
        } else {
            auto x = T(*it);
            return thunk(list_rep<T>(std::in_place_index<1>, x, from_range(++it, end)));
        }
    }

    template <class U>
    std::vector<U> to_reversed_vector() const {
        if (auto rep = this->get(); rep.index() == 0) {
            return std::vector<U>();
        } else {
            auto [x, xs] = std::get<1>(rep);
            auto rv = xs.template to_reversed_vector<U>();
            rv.push_back(x.template get_as<U>());
            return rv;
        }
    }

    T at(int n) const {
        if (n < 0) {
            throw std::out_of_range("operator[]: negative index");
        } else if (auto rep = this->get(); rep.index() == 0) {
            throw std::out_of_range("operator[]: index too large");
        } else if (auto [x, xs] = std::get<1>(rep); n == 0) {
            return x;
        } else {
            return xs.at(n - 1);
        }
    }

public:
    using detail::thunk_base<list_rep<T>>::thunk_base;

    // In Clang, thunk_base(U &&) and thunk_base(F &&) are hidden by thunk(Range &&).
    template <
        class U,
        std::enable_if_t<
            std::conjunction_v<
                std::negation<std::is_same<std::decay_t<U>, thunk>>,
                std::is_constructible<list_rep<T>, U>
            >
        > * = nullptr
    >
    explicit thunk(U &&u) :
        detail::thunk_base<list_rep<T>>(std::forward<U>(u)) {
    }

    template <
        class F,
        std::enable_if_t<
            std::conjunction_v<
                std::negation<std::is_same<std::decay_t<F>, thunk>>,
                std::negation<std::is_constructible<list_rep<T>, F>>,
                std::is_invocable_r<thunk, std::decay_t<F> &>
            >
        > * = nullptr
    >
    explicit thunk(F &&f) :
        detail::thunk_base<list_rep<T>>(std::forward<F>(f)) {
    }

    template <
        class Range,
        std::enable_if_t<
            std::conjunction_v<
                std::negation<std::is_same<std::decay_t<Range>, thunk>>,
                std::negation<std::is_constructible<list_rep<T>, Range>>,
                std::negation<std::is_invocable_r<thunk, std::decay_t<Range> &>>,
                std::is_constructible<T, decltype(*std::begin(std::declval<Range &>()))>
            >
        > * = nullptr
    >
    explicit thunk(Range &&r) :
        thunk(from_range(std::begin(r), std::end(r))) {
    }

    template <
        class U,
        std::enable_if_t<
            std::is_constructible_v<T, U const &>
        > * = nullptr
    >
    explicit thunk(std::initializer_list<U> il) :
        thunk(from_range(il.begin(), il.end())) {
    }

    template <class Container>
    Container get_as() const {
        auto rv = to_reversed_vector<typename Container::value_type>();
        return Container(rv.rbegin(), rv.rend());
    }

    T operator[](int_ n) const {
        return at(n.get());
    }
};

template <class T>
using list = thunk<list_rep<T>>;

using string = list<char_>;

// Literals
inline namespace literals {

inline char_ operator"" _c(char c) {
    return char_(c);
}

inline int_ operator"" _d(unsigned long long ull) {
    return int_(int(ull));
}

#ifdef EASYLAZY_ENABLE_INTEGER
inline integer operator"" _n(unsigned long long ull) {
    return integer(ull);
}
#endif

inline float_ operator"" _f(long double ld) {
    return float_(float(ld));
}

inline double_ operator"" _lf(long double ld) {
    return double_(double(ld));
}

inline string operator"" _s(char const *s, std::size_t n) {
    return string(std::string_view(s, n));
}

} // inline namespace literals {

// Macros
namespace detail {

template <class Sig>
class function_helper {};

template <class ...Args, class F, class R = std::invoke_result_t<F &, Args...>>
inline function<R (Args...)> operator*(function_helper<void (Args...)>, F &&f) {
    return function<R (Args...)>(std::move(f));
}

} // namespace detail {

#define EASYLAZY_FUNCTION(...)                                                 \
::easylazy::detail::function_helper<void (__VA_ARGS__)>() * [=](__VA_ARGS__)   \
/**/

// Operators
#define EASYLAZY_UNARY_OPERATOR(op)                                            \
template <class T, class R = thunk<decltype(op std::declval<T>())>>            \
inline R operator op(thunk<T> x) {                                             \
    return R([=]() {                                                           \
        return R(op x.get());                                                  \
    });                                                                        \
}                                                                              \
/**/
EASYLAZY_UNARY_OPERATOR(+)
EASYLAZY_UNARY_OPERATOR(-)
EASYLAZY_UNARY_OPERATOR(!)
EASYLAZY_UNARY_OPERATOR(~)
#undef EASYLAZY_UNARY_OPERATOR

#define EASYLAZY_BINARY_OPERATOR(op)                                           \
template <class T, class U, class R = thunk<decltype(std::declval<T>() op std::declval<U>())>> \
inline R operator op(thunk<T> x, thunk<U> y) {                                 \
    return R([=]() {                                                           \
        return R(x.get() op y.get());                                          \
    });                                                                        \
}                                                                              \
/**/
EASYLAZY_BINARY_OPERATOR(*)
EASYLAZY_BINARY_OPERATOR(/)
EASYLAZY_BINARY_OPERATOR(%)
EASYLAZY_BINARY_OPERATOR(+)
EASYLAZY_BINARY_OPERATOR(-)
EASYLAZY_BINARY_OPERATOR(<<)
EASYLAZY_BINARY_OPERATOR(>>)
EASYLAZY_BINARY_OPERATOR(<)
EASYLAZY_BINARY_OPERATOR(>)
EASYLAZY_BINARY_OPERATOR(<=)
EASYLAZY_BINARY_OPERATOR(>=)
EASYLAZY_BINARY_OPERATOR(==)
EASYLAZY_BINARY_OPERATOR(!=)
EASYLAZY_BINARY_OPERATOR(&)
EASYLAZY_BINARY_OPERATOR(^)
EASYLAZY_BINARY_OPERATOR(|)
EASYLAZY_BINARY_OPERATOR(&&)
EASYLAZY_BINARY_OPERATOR(||)
#undef EASYLAZY_BINARY_OPERATOR

// Functions
template <class T>
inline list<T> nil() {
    static list<T> xs{list_rep<T>(std::in_place_index<0>)};
    return xs;
}

template <class T>
inline list<T> cons(T x, list<T> xs) {
    return list<T>(list_rep<T>(std::in_place_index<1>, x, xs));
}

template <class T>
inline T head(list<T> x_xs) {
    return T([=]() {
        if (auto rep = x_xs.get(); rep.index() == 0) {
            throw std::invalid_argument("head: empty list");
        } else {
            auto [x, xs] = std::get<1>(rep);
            return x;
        }
    });
}

template <class T>
inline list<T> tail(list<T> x_xs) {
    return list<T>([=]() {
        if (auto rep = x_xs.get(); rep.index() == 0) {
            throw std::invalid_argument("tail: empty list");
        } else {
            auto [x, xs] = std::get<1>(rep);
            return xs;
        }
    });
}

template <class T>
inline bool_ null(list<T> xs) {
    return bool_([=]() {
        auto rep = xs.get();
        return bool_(rep.index() == 0);
    });
}

template <class T>
inline bool_ operator<(list<T> x_xs, list<T> y_ys) {
    return bool_([=]() {
        if (null(y_ys)) {
            return bool_(false);
        } else if (null(x_xs)) {
            return !null(y_ys);
        } else {
            T x = head(x_xs);
            T y = head(y_ys);
            return x < y || (x == y && tail(x_xs) < tail(y_ys));
        }
    });
}

template <class T>
inline bool_ operator>(list<T> xs, list<T> ys) {
    return ys < xs;
}

template <class T>
inline bool_ operator<=(list<T> xs, list<T> ys) {
    return !(ys < xs);
}

template <class T>
inline bool_ operator>=(list<T> xs, list<T> ys) {
    return !(xs < ys);
}

template <class T>
inline bool_ operator==(list<T> xs, list<T> ys) {
    return bool_([=]() {
        if (null(xs)) {
            return null(ys);
        } else {
            return !null(ys) && bool_(head(xs) == head(ys)) && tail(xs) == tail(ys);
        }
    });
}

template <class T>
inline bool_ operator!=(list<T> xs, list<T> ys) {
    return !(xs == ys);
}

template <class T, class U>
inline list<U> map(function<U (T)> f, list<T> xs) {
    return list<U>([=]() {
        if (null(xs)) {
            return nil<U>();
        } else {
            return cons(f(head(xs)), map(f, tail(xs)));
        }
    });
}

template <class T>
inline list<T> append(list<T> xs, list<T> ys) {
    return list<T>([=]() {
        if (null(xs)) {
            return ys;
        } else {
            return cons(head(xs), append(tail(xs), ys));
        }
    });
}

template <class T>
inline list<T> filter(function<bool_ (T)> p, list<T> x_xs) {
    return list<T>([=]() {
        if (null(x_xs)) {
            return nil<T>();
        } else if (T x = head(x_xs); p(x)) {
            return cons(x, filter(p, tail(x_xs)));
        } else {
            return filter(p, tail(x_xs));
        }
    });
}

template <class T>
inline T last(list<T> x_xs) {
    return T([=]() {
        if (null(x_xs)) {
            throw std::invalid_argument("last: empty list");
        } else if (list<T> xs = tail(x_xs); null(xs)) {
            return head(x_xs);
        } else {
            return last(xs);
        }
    });
}

template <class T>
inline list<T> init(list<T> x_xs) {
    return list<T>([=]() {
        if (null(x_xs)) {
            throw std::invalid_argument("init: empty list");
        } else if (list<T> xs = tail(x_xs); null(xs)) {
            return nil<T>();
        } else {
            return cons(head(x_xs), init(xs));
        }
    });
}

template <class T>
inline int_ length(list<T> xs) {
    return int_([=]() {
        if (null(xs)) {
            return 0_d;
        } else {
            return 1_d + length(tail(xs));
        }
    });
}

namespace detail {

template <class T>
inline list<T> reverse_impl(list<T> xs, list<T> acc) {
    return list<T>([=]() {
        if (null(xs)) {
            return acc;
        } else {
            return reverse_impl(tail(xs), cons(head(xs), acc));
        }
    });
}

} // namespace detail {

template <class T>
inline list<T> reverse(list<T> xs) {
    return list<T>([=]() {
        return detail::reverse_impl(xs, nil<T>());
    });
}

} // namespace easylazy {

#endif // #ifndef EASYLAZY_HPP_INCLUDED
