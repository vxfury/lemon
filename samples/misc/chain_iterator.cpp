#include <functional>

namespace boost
{
namespace hana
{

template <bool condition>
struct when;

namespace core_detail
{
template <typename...>
struct always_true {
    static constexpr bool value = true;
};
} // namespace core_detail

template <typename... Dummy>
using when_valid = when<core_detail::always_true<Dummy...>::value>;

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct accessors_impl : accessors_impl<S, when<true>> {};

template <typename S>
struct accessors_t;

template <typename S>
constexpr accessors_t<S> accessors{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S>
struct Struct;
}
} // namespace boost

namespace boost
{
namespace hana
{

struct default_ {};

template <typename T, typename = void>
struct is_default;

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename Method, typename>
struct is_default : std::false_type {};

template <typename Method>
struct is_default<Method, decltype((void)static_cast<default_>(*(Method *)0))> : std::true_type {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct tag_of;

template <typename T>
using tag_of_t = typename hana::tag_of<T>::type;
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename>
struct tag_of : tag_of<T, when<true>> {};

namespace core_detail
{
template <typename...>
struct is_valid {
    static constexpr bool value = true;
};
} // namespace core_detail

template <typename T, bool condition>
struct tag_of<T, when<condition>> {
    using type = T;
};

template <typename T>
struct tag_of<T, when<core_detail::is_valid<typename T::hana_tag>::value>> {
    using type = typename T::hana_tag;
};

template <typename T>
struct tag_of<T const> : tag_of<T> {};
template <typename T>
struct tag_of<T volatile> : tag_of<T> {};
template <typename T>
struct tag_of<T const volatile> : tag_of<T> {};
template <typename T>
struct tag_of<T &> : tag_of<T> {};
template <typename T>
struct tag_of<T &&> : tag_of<T> {};

namespace detail
{
template <typename T>
struct has_idempotent_tag : std::is_same<hana::tag_of_t<T>, std::remove_const_t<std::remove_reference_t<T>>> {};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
namespace operators
{

template <typename...>
struct adl {};
} // namespace operators
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct integral_constant_tag {
    using value_type = T;
};

namespace ic_detail
{
template <typename T, T v>
struct with_index_t {
    template <typename F>
    constexpr void operator()(F &&f) const;
};

template <typename T, T v>
struct times_t {
    static constexpr with_index_t<T, v> with_index{};

    template <typename F>
    constexpr void operator()(F &&f) const;
};
} // namespace ic_detail

template <typename T, T v>

struct integral_constant

    : std::integral_constant<T, v>,
      detail::operators::adl<integral_constant<T, v>> {
    using type = integral_constant;
    static constexpr ic_detail::times_t<T, v> times{};
    using hana_tag = integral_constant_tag<T>;
};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename S>
struct Struct : hana::integral_constant<bool, !is_default<accessors_impl<typename tag_of<S>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
struct deleted_implementation {
    template <typename... T>
    static constexpr auto apply(T &&...) = delete;
};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename S>
struct accessors_t {
    static_assert(hana::Struct<S>::value, "hana::accessors<S> requires 'S' to be a Struct");

    constexpr decltype(auto) operator()() const
    {
        using Accessors =
            ::std::conditional_t<(hana::Struct<S>::value), accessors_impl<S>, ::boost::hana::deleted_implementation>;

        return Accessors::apply();
    }
};

template <typename S, bool condition>
struct accessors_impl<S, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

namespace struct_detail
{
template <typename...>
struct is_valid {
    static constexpr bool value = true;
};
} // namespace struct_detail

template <typename S>
struct accessors_impl<S, when<struct_detail::is_valid<typename S::hana_accessors_impl>::value>>
    : S::hana_accessors_impl {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Tag, typename = void>
struct make_impl;

template <typename Tag>
struct make_t {
    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const
    {
        return make_impl<Tag>::apply(static_cast<X &&>(x)...);
    }
};

template <typename Tag>
constexpr make_t<Tag> make{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename First, typename Second>
struct pair;

struct pair_tag {};

constexpr auto make_pair = make<pair_tag>;
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename T, typename U = typename std::remove_reference<T>::type>
struct decay {
    using type = typename std::remove_cv<U>::type;
};

template <typename T, typename U>
struct decay<T, U[]> {
    using type = U *;
};
template <typename T, typename U, std::size_t N>
struct decay<T, U[N]> {
    using type = U *;
};

template <typename T, typename R, typename... A>
struct decay<T, R(A...)> {
    using type = R (*)(A...);
};
template <typename T, typename R, typename... A>
struct decay<T, R(A..., ...)> {
    using type = R (*)(A..., ...);
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace _hana
{

template <typename K, typename V, bool = __is_empty(V) && !__is_final(V)>
struct ebo;

template <typename K, typename V>
struct ebo<K, V, true> : V {
    constexpr ebo() {}

    template <typename T>
    explicit constexpr ebo(T &&t) : V(static_cast<T &&>(t))
    {
    }
};

template <typename K, typename V>
struct ebo<K, V, false> {
    constexpr ebo() : data_() {}

    template <typename T>
    explicit constexpr ebo(T &&t) : data_(static_cast<T &&>(t))
    {
    }

    V data_;
};

template <typename K, typename V>
constexpr V const &ebo_get(ebo<K, V, true> const &x)
{
    return x;
}

template <typename K, typename V>
constexpr V &ebo_get(ebo<K, V, true> &x)
{
    return x;
}

template <typename K, typename V>
constexpr V &&ebo_get(ebo<K, V, true> &&x)
{
    return static_cast<V &&>(x);
}

template <typename K, typename V>
constexpr V const &ebo_get(ebo<K, V, false> const &x)
{
    return x.data_;
}

template <typename K, typename V>
constexpr V &ebo_get(ebo<K, V, false> &x)
{
    return x.data_;
}

template <typename K, typename V>
constexpr V &&ebo_get(ebo<K, V, false> &&x)
{
    return static_cast<V &&>(x.data_);
}
} // namespace _hana

namespace boost
{
namespace hana
{
namespace detail
{
using ::_hana::ebo;
using ::_hana::ebo_get;
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Algorithm>
struct nested_to_t {
    template <typename X>
    constexpr decltype(auto) operator()(X &&x) const;
};

template <typename Algorithm>
struct nested_to {
    static constexpr nested_to_t<Algorithm> to{};
};

template <typename Algorithm>
constexpr nested_to_t<Algorithm> nested_to<Algorithm>::to;
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct equal_impl : equal_impl<T, U, when<true>> {};

struct equal_t : detail::nested_to<equal_t> {
    template <typename X, typename Y>
    constexpr auto operator()(X &&x, Y &&y) const;
};

constexpr equal_t equal{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct not_equal_impl : not_equal_impl<T, U, when<true>> {};

struct not_equal_t : detail::nested_to<not_equal_t> {
    template <typename X, typename Y>
    constexpr auto operator()(X &&x, Y &&y) const;
};

constexpr not_equal_t not_equal{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Tag>
struct comparable_operators {
    static constexpr bool value = false;
};

namespace operators
{
template <typename X, typename Y,
          typename = typename std::enable_if<
              !detail::has_idempotent_tag<X>::value && !detail::has_idempotent_tag<Y>::value
              && (detail::comparable_operators<typename hana::tag_of<X>::type>::value
                  || detail::comparable_operators<typename hana::tag_of<Y>::type>::value)>::type>
constexpr auto operator==(X &&x, Y &&y)
{
    return hana::equal(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename X, typename Y,
          typename = typename std::enable_if<
              !detail::has_idempotent_tag<X>::value && !detail::has_idempotent_tag<Y>::value
              && (detail::comparable_operators<typename hana::tag_of<X>::type>::value
                  || detail::comparable_operators<typename hana::tag_of<Y>::type>::value)>::type>
constexpr auto operator!=(X &&x, Y &&y)
{
    return hana::not_equal(static_cast<X &&>(x), static_cast<Y &&>(y));
}
} // namespace operators
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Algorithm>
struct nested_than_t {
    template <typename X>
    constexpr decltype(auto) operator()(X &&x) const;
};

template <typename Algorithm>
struct nested_than {
    static constexpr nested_than_t<Algorithm> than{};
};

template <typename Algorithm>
constexpr nested_than_t<Algorithm> nested_than<Algorithm>::than;
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct greater_impl : greater_impl<T, U, when<true>> {};

struct greater_t : detail::nested_than<greater_t> {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr greater_t greater{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct greater_equal_impl : greater_equal_impl<T, U, when<true>> {};

struct greater_equal_t : detail::nested_than<greater_equal_t> {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr greater_equal_t greater_equal{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct less_impl : less_impl<T, U, when<true>> {};

struct less_t : detail::nested_than<less_t> {
    template <typename X, typename Y>
    constexpr auto operator()(X &&x, Y &&y) const;
};

constexpr less_t less{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct less_equal_impl : less_equal_impl<T, U, when<true>> {};

struct less_equal_t : detail::nested_than<less_equal_t> {
    template <typename X, typename Y>
    constexpr auto operator()(X &&x, Y &&y) const;
};

constexpr less_equal_t less_equal{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Tag>
struct orderable_operators {
    static constexpr bool value = false;
};

namespace operators
{
template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::orderable_operators<typename hana::tag_of<X>::type>::value
                                       || detail::orderable_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator<(X &&x, Y &&y)
{
    return hana::less(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::orderable_operators<typename hana::tag_of<X>::type>::value
                                       || detail::orderable_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator>(X &&x, Y &&y)
{
    return hana::greater(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::orderable_operators<typename hana::tag_of<X>::type>::value
                                       || detail::orderable_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator<=(X &&x, Y &&y)
{
    return hana::less_equal(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::orderable_operators<typename hana::tag_of<X>::type>::value
                                       || detail::orderable_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator>=(X &&x, Y &&y)
{
    return hana::greater_equal(static_cast<X &&>(x), static_cast<Y &&>(y));
}
} // namespace operators
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename P, typename = void>
struct first_impl : first_impl<P, when<true>> {};

struct first_t {
    template <typename Pair>
    constexpr decltype(auto) operator()(Pair &&pair) const;
};

constexpr first_t first{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename P, typename = void>
struct second_impl : second_impl<P, when<true>> {};

struct second_t {
    template <typename Pair>
    constexpr decltype(auto) operator()(Pair &&pair) const;
};

constexpr second_t second{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <int>
struct pix;
}

template <typename First, typename Second>

struct pair : detail::operators::adl<pair<First, Second>>

    ,
              private detail::ebo<detail::pix<0>, First>,
              private detail::ebo<detail::pix<1>, Second> {
    template <typename... dummy, typename = typename std::enable_if<::std::is_constructible<
                                     First, dummy...>::value && ::std::is_constructible<Second, dummy...>::value>::type>
    constexpr pair() : detail::ebo<detail::pix<0>, First>(), detail::ebo<detail::pix<1>, Second>()
    {
    }

    template <typename... dummy, typename = typename std::enable_if<
                                     ::std::is_constructible<First, First const &, dummy...>::value
                                         && ::std::is_constructible<Second, Second const &, dummy...>::value>::type>
    constexpr pair(First const &fst, Second const &snd)
        : detail::ebo<detail::pix<0>, First>(fst), detail::ebo<detail::pix<1>, Second>(snd)
    {
    }

    template <typename T, typename U,
              typename = typename std::enable_if<
                  ::std::is_convertible<T &&, First>::value && ::std::is_convertible<U &&, Second>::value>::type>
    constexpr pair(T &&t, U &&u)
        : detail::ebo<detail::pix<0>, First>(static_cast<T &&>(t)),
          detail::ebo<detail::pix<1>, Second>(static_cast<U &&>(u))
    {
    }

    template <
        typename T, typename U,
        typename = typename std::enable_if<::std::is_constructible<First, T const &>::value && ::std::is_constructible<
            Second, U const &>::value && ::std::is_convertible<T const &, First>::value
                                               && ::std::is_convertible<U const &, Second>::value>::type>
    constexpr pair(pair<T, U> const &other)
        : detail::ebo<detail::pix<0>, First>(hana::first(other)),
          detail::ebo<detail::pix<1>, Second>(hana::second(other))
    {
    }

    template <typename T, typename U,
              typename = typename std::enable_if<
                  ::std::is_constructible<First, T &&>::value && ::std::is_constructible<Second, U &&>::value
                      && ::std::is_convertible<T &&, First>::value && ::std::is_convertible<U &&, Second>::value>::type>
    constexpr pair(pair<T, U> &&other)
        : detail::ebo<detail::pix<0>, First>(hana::first(static_cast<pair<T, U> &&>(other))),
          detail::ebo<detail::pix<1>, Second>(hana::second(static_cast<pair<T, U> &&>(other)))
    {
    }

    template <typename T, typename U,
              typename = typename std::enable_if<::std::is_assignable<First &, T const &>::value
                                                     && ::std::is_assignable<Second &, U const &>::value>::type>
    constexpr pair &operator=(pair<T, U> const &other)
    {
        hana::first(*this) = hana::first(other);
        hana::second(*this) = hana::second(other);
        return *this;
    }

    template <typename T, typename U,
              typename = typename std::enable_if<
                  ::std::is_assignable<First &, T &&>::value && ::std::is_assignable<Second &, U &&>::value>::type>
    constexpr pair &operator=(pair<T, U> &&other)
    {
        hana::first(*this) = hana::first(static_cast<pair<T, U> &&>(other));
        hana::second(*this) = hana::second(static_cast<pair<T, U> &&>(other));
        return *this;
    }

    ~pair() = default;

    friend struct first_impl<pair_tag>;
    friend struct second_impl<pair_tag>;
    template <typename F, typename S>
    friend struct pair;
};

template <typename First, typename Second>
struct tag_of<pair<First, Second>> {
    using type = pair_tag;
};

namespace detail
{
template <>
struct comparable_operators<pair_tag> {
    static constexpr bool value = true;
};
template <>
struct orderable_operators<pair_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <>
struct make_impl<pair_tag> {
    template <typename F, typename S>
    static constexpr pair<typename detail::decay<F>::type, typename detail::decay<S>::type> apply(F &&f, S &&s)
    {
        return {static_cast<F &&>(f), static_cast<S &&>(s)};
    }
};

template <>
struct first_impl<pair_tag> {
    template <typename First, typename Second>
    static constexpr decltype(auto) apply(hana::pair<First, Second> &p)
    {
        return detail::ebo_get<detail::pix<0>>(static_cast<detail::ebo<detail::pix<0>, First> &>(p));
    }
    template <typename First, typename Second>
    static constexpr decltype(auto) apply(hana::pair<First, Second> const &p)
    {
        return detail::ebo_get<detail::pix<0>>(static_cast<detail::ebo<detail::pix<0>, First> const &>(p));
    }
    template <typename First, typename Second>
    static constexpr decltype(auto) apply(hana::pair<First, Second> &&p)
    {
        return detail::ebo_get<detail::pix<0>>(static_cast<detail::ebo<detail::pix<0>, First> &&>(p));
    }
};

template <>
struct second_impl<pair_tag> {
    template <typename First, typename Second>
    static constexpr decltype(auto) apply(hana::pair<First, Second> &p)
    {
        return detail::ebo_get<detail::pix<1>>(static_cast<detail::ebo<detail::pix<1>, Second> &>(p));
    }
    template <typename First, typename Second>
    static constexpr decltype(auto) apply(hana::pair<First, Second> const &p)
    {
        return detail::ebo_get<detail::pix<1>>(static_cast<detail::ebo<detail::pix<1>, Second> const &>(p));
    }
    template <typename First, typename Second>
    static constexpr decltype(auto) apply(hana::pair<First, Second> &&p)
    {
        return detail::ebo_get<detail::pix<1>>(static_cast<detail::ebo<detail::pix<1>, Second> &&>(p));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename To, typename From, typename = void>
struct to_impl;

template <typename To>
struct to_t {
    template <typename X>
    constexpr decltype(auto) operator()(X &&x) const;
};

template <typename To>
constexpr to_t<To> to{};

template <typename From, typename To, typename = void>
struct is_convertible;

template <bool = true>
struct embedding {};

template <typename From, typename To, typename = void>
struct is_embedded;

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <char... s>
struct string;

struct string_tag {};

constexpr auto make_string = make<string_tag>;

constexpr auto to_string = to<string_tag>;

template <char... s>
constexpr string<s...> string_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, T v>
constexpr integral_constant<T, v> integral_c{};

template <bool b>
using bool_ = integral_constant<bool, b>;

template <bool b>
constexpr bool_<b> bool_c{};

using true_ = bool_<true>;

constexpr auto true_c = bool_c<true>;

using false_ = bool_<false>;

constexpr auto false_c = bool_c<false>;

template <char c>
using char_ = integral_constant<char, c>;

template <char c>
constexpr char_<c> char_c{};

template <short i>
using short_ = integral_constant<short, i>;

template <short i>
constexpr short_<i> short_c{};

template <unsigned short i>
using ushort_ = integral_constant<unsigned short, i>;

template <unsigned short i>
constexpr ushort_<i> ushort_c{};

template <int i>
using int_ = integral_constant<int, i>;

template <int i>
constexpr int_<i> int_c{};

template <unsigned int i>
using uint = integral_constant<unsigned int, i>;

template <unsigned int i>
constexpr uint<i> uint_c{};

template <long i>
using long_ = integral_constant<long, i>;

template <long i>
constexpr long_<i> long_c{};

template <unsigned long i>
using ulong = integral_constant<unsigned long, i>;

template <unsigned long i>
constexpr ulong<i> ulong_c{};

template <long long i>
using llong = integral_constant<long long, i>;

template <long long i>
constexpr llong<i> llong_c{};

template <unsigned long long i>
using ullong = integral_constant<unsigned long long, i>;

template <unsigned long long i>
constexpr ullong<i> ullong_c{};

template <std::size_t i>
using size_t = integral_constant<std::size_t, i>;

template <std::size_t i>
constexpr size_t<i> size_c{};

namespace literals
{

template <char... c>
constexpr auto operator"" _c();
}
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename C>
struct IntegralConstant;
}
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename C, typename Tag = typename tag_of<C>::type>
struct integral_constant_dispatch : hana::integral_constant<bool, hana::IntegralConstant<Tag>::value> {};

template <typename C>
struct integral_constant_dispatch<C, C> : hana::integral_constant<bool, false> {};
} // namespace detail

template <typename C>
struct IntegralConstant : detail::integral_constant_dispatch<C> {};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename C>
struct Constant;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename C, typename = void>
struct value_impl : value_impl<C, when<true>> {};

template <typename T>
constexpr decltype(auto) value();

template <typename T>
constexpr decltype(auto) value(T const &)
{
    return hana::value<T>();
}

struct value_of_t {
    template <typename T>
    constexpr decltype(auto) operator()(T const &) const
    {
        return hana::value<T>();
    }
};

constexpr value_of_t value_of{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename C, bool condition>
struct value_impl<C, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...args) = delete;
};

template <typename T>
constexpr decltype(auto) value()
{
    using RawT = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    using C = typename hana::tag_of<RawT>::type;
    using Value =
        ::std::conditional_t<(hana::Constant<C>::value), value_impl<C>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Constant<C>::value, "hana::value<T>() requires 'T' to be a Constant");

    return Value::template apply<RawT>();
}

template <typename I>
struct value_impl<I, when<hana::IntegralConstant<I>::value>> {
    template <typename C>
    static constexpr auto apply()
    {
        return C::value;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename C>
struct Constant : hana::integral_constant<bool, !is_default<value_impl<typename tag_of<C>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct Foldable;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct fold_left_impl : fold_left_impl<T, when<true>> {};

struct fold_left_t {
    template <typename Xs, typename State, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, State &&state, F &&f) const;

    template <typename Xs, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, F &&f) const;
};

constexpr fold_left_t fold_left{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
namespace variadic
{

template <unsigned int n, typename = when<true>>
struct foldl1_impl;

template <>
struct foldl1_impl<1> {
    template <typename F, typename X1>
    static constexpr X1 apply(F &&, X1 &&x1)
    {
        return static_cast<X1 &&>(x1);
    }
};

template <>
struct foldl1_impl<2> {
    template <typename F, typename X1, typename X2>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2)
    {
        return static_cast<F &&>(f)(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2));
    }
};

template <>
struct foldl1_impl<3> {
    template <typename F, typename X1, typename X2, typename X3>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3)
    {
        return f(f(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2)), static_cast<X3 &&>(x3));
    }
};

template <>
struct foldl1_impl<4> {
    template <typename F, typename X1, typename X2, typename X3, typename X4>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4)
    {
        return f(f(f(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2)), static_cast<X3 &&>(x3)), static_cast<X4 &&>(x4));
    }
};

template <>
struct foldl1_impl<5> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5)
    {
        return f(
            f(f(f(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2)), static_cast<X3 &&>(x3)), static_cast<X4 &&>(x4)),
            static_cast<X5 &&>(x5));
    }
};

template <>
struct foldl1_impl<6> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6)
    {
        return f(
            f(f(f(f(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2)), static_cast<X3 &&>(x3)), static_cast<X4 &&>(x4)),
              static_cast<X5 &&>(x5)),
            static_cast<X6 &&>(x6));
    }
};

template <unsigned int n>
struct foldl1_impl<n, when<(n >= 7) && (n < 14)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7,
                                          Xn &&...xn)
    {
        return foldl1_impl<sizeof...(xn) + 1>::apply(
            f,
            f(f(f(f(f(f(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2)), static_cast<X3 &&>(x3)),
                    static_cast<X4 &&>(x4)),
                  static_cast<X5 &&>(x5)),
                static_cast<X6 &&>(x6)),
              static_cast<X7 &&>(x7)),
            static_cast<Xn &&>(xn)...);
    }
};

template <unsigned int n>
struct foldl1_impl<n, when<(n >= 14) && (n < 28)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename X8, typename X9, typename X10, typename X11, typename X12, typename X13, typename X14,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7, X8 &&x8,
                                          X9 &&x9, X10 &&x10, X11 &&x11, X12 &&x12, X13 &&x13, X14 &&x14, Xn &&...xn)
    {
        return foldl1_impl<sizeof...(xn) + 1>::apply(
            f,
            f(f(f(f(f(f(f(f(f(f(f(f(f(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2)), static_cast<X3 &&>(x3)),
                                  static_cast<X4 &&>(x4)),
                                static_cast<X5 &&>(x5)),
                              static_cast<X6 &&>(x6)),
                            static_cast<X7 &&>(x7)),
                          static_cast<X8 &&>(x8)),
                        static_cast<X9 &&>(x9)),
                      static_cast<X10 &&>(x10)),
                    static_cast<X11 &&>(x11)),
                  static_cast<X12 &&>(x12)),
                static_cast<X13 &&>(x13)),
              static_cast<X14 &&>(x14)),
            static_cast<Xn &&>(xn)...);
    }
};

template <unsigned int n>
struct foldl1_impl<n, when<(n >= 28) && (n < 56)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename X8, typename X9, typename X10, typename X11, typename X12, typename X13, typename X14,
              typename X15, typename X16, typename X17, typename X18, typename X19, typename X20, typename X21,
              typename X22, typename X23, typename X24, typename X25, typename X26, typename X27, typename X28,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7, X8 &&x8,
                                          X9 &&x9, X10 &&x10, X11 &&x11, X12 &&x12, X13 &&x13, X14 &&x14, X15 &&x15,
                                          X16 &&x16, X17 &&x17, X18 &&x18, X19 &&x19, X20 &&x20, X21 &&x21, X22 &&x22,
                                          X23 &&x23, X24 &&x24, X25 &&x25, X26 &&x26, X27 &&x27, X28 &&x28, Xn &&...xn)
    {
        return foldl1_impl<sizeof...(xn) + 1>::apply(
            f,
            f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2)),
                                                                static_cast<X3 &&>(x3)),
                                                              static_cast<X4 &&>(x4)),
                                                            static_cast<X5 &&>(x5)),
                                                          static_cast<X6 &&>(x6)),
                                                        static_cast<X7 &&>(x7)),
                                                      static_cast<X8 &&>(x8)),
                                                    static_cast<X9 &&>(x9)),
                                                  static_cast<X10 &&>(x10)),
                                                static_cast<X11 &&>(x11)),
                                              static_cast<X12 &&>(x12)),
                                            static_cast<X13 &&>(x13)),
                                          static_cast<X14 &&>(x14)),
                                        static_cast<X15 &&>(x15)),
                                      static_cast<X16 &&>(x16)),
                                    static_cast<X17 &&>(x17)),
                                  static_cast<X18 &&>(x18)),
                                static_cast<X19 &&>(x19)),
                              static_cast<X20 &&>(x20)),
                            static_cast<X21 &&>(x21)),
                          static_cast<X22 &&>(x22)),
                        static_cast<X23 &&>(x23)),
                      static_cast<X24 &&>(x24)),
                    static_cast<X25 &&>(x25)),
                  static_cast<X26 &&>(x26)),
                static_cast<X27 &&>(x27)),
              static_cast<X28 &&>(x28)),
            static_cast<Xn &&>(xn)...);
    }
};

template <unsigned int n>
struct foldl1_impl<n, when<(n >= 56)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename X8, typename X9, typename X10, typename X11, typename X12, typename X13, typename X14,
              typename X15, typename X16, typename X17, typename X18, typename X19, typename X20, typename X21,
              typename X22, typename X23, typename X24, typename X25, typename X26, typename X27, typename X28,
              typename X29, typename X30, typename X31, typename X32, typename X33, typename X34, typename X35,
              typename X36, typename X37, typename X38, typename X39, typename X40, typename X41, typename X42,
              typename X43, typename X44, typename X45, typename X46, typename X47, typename X48, typename X49,
              typename X50, typename X51, typename X52, typename X53, typename X54, typename X55, typename X56,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7, X8 &&x8,
                                          X9 &&x9, X10 &&x10, X11 &&x11, X12 &&x12, X13 &&x13, X14 &&x14, X15 &&x15,
                                          X16 &&x16, X17 &&x17, X18 &&x18, X19 &&x19, X20 &&x20, X21 &&x21, X22 &&x22,
                                          X23 &&x23, X24 &&x24, X25 &&x25, X26 &&x26, X27 &&x27, X28 &&x28, X29 &&x29,
                                          X30 &&x30, X31 &&x31, X32 &&x32, X33 &&x33, X34 &&x34, X35 &&x35, X36 &&x36,
                                          X37 &&x37, X38 &&x38, X39 &&x39, X40 &&x40, X41 &&x41, X42 &&x42, X43 &&x43,
                                          X44 &&x44, X45 &&x45, X46 &&x46, X47 &&x47, X48 &&x48, X49 &&x49, X50 &&x50,
                                          X51 &&x51, X52 &&x52, X53 &&x53, X54 &&x54, X55 &&x55, X56 &&x56, Xn &&...xn)
    {
            return foldl1_impl<sizeof...(xn) + 1>::apply(
                f,
                f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(
                    static_cast<X1&&>(x1), static_cast<X2&&>(x2)), static_cast<X3&&>(x3)), static_cast<X4&&>(x4)), static_cast<X5&&>(x5)), static_cast<X6&&>(x6)), static_cast<X7&&>(x7)),
                    static_cast<X8&&>(x8)), static_cast<X9&&>(x9)), static_cast<X10&&>(x10)), static_cast<X11&&>(x11)), static_cast<X12&&>(x12)), static_cast<X13&&>(x13)), static_cast<X14&&>(x14)),
                    static_cast<X15&&>(x15)), static_cast<X16&&>(x16)), static_cast<X17&&>(x17)), static_cast<X18&&>(x18)), static_cast<X19&&>(x19)), static_cast<X20&&>(x20)), static_cast<X21&&>(x21)),
                    static_cast<X22&&>(x22)), static_cast<X23&&>(x23)), static_cast<X24&&>(x24)), static_cast<X25&&>(x25)), static_cast<X26&&>(x26)), static_cast<X27&&>(x27)), static_cast<X28&&>(x28)),
                    static_cast<X29&&>(x29)), static_cast<X30&&>(x30)), static_cast<X31&&>(x31)), static_cast<X32&&>(x32)), static_cast<X33&&>(x33)), static_cast<X34&&>(x34)), static_cast<X35&&>(x35)),
                    static_cast<X36&&>(x36)), static_cast<X37&&>(x37)), static_cast<X38&&>(x38)), static_cast<X39&&>(x39)), static_cast<X40&&>(x40)), static_cast<X41&&>(x41)), static_cast<X42&&>(x42)),
                    static_cast<X43&&>(x43)), static_cast<X44&&>(x44)), static_cast<X45&&>(x45)), static_cast<X46&&>(x46)), static_cast<X47&&>(x47)), static_cast<X48&&>(x48)), static_cast<X49&&>(x49)),
                    static_cast<X50&&>(x50)), static_cast<X51&&>(x51)), static_cast<X52&&>(x52)), static_cast<X53&&>(x53)), static_cast<X54&&>(x54)), static_cast<X55&&>(x55)), static_cast<X56&&>(x56))
                , static_cast<Xn&&>(xn)...);
    }
};

struct foldl1_t {
    template <typename F, typename X1, typename... Xn>
    constexpr decltype(auto) operator()(F &&f, X1 &&x1, Xn &&...xn) const
    {
            return foldl1_impl<sizeof...(xn) + 1>::apply(static_cast<F &&>(f), static_cast<X1 &&>(x1),
                                                         static_cast<Xn &&>(xn)...);
    }
};

constexpr foldl1_t foldl1{};
constexpr auto foldl = foldl1;
} // namespace variadic
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename... Xs>
struct basic_tuple;

struct basic_tuple_tag {};

constexpr auto make_basic_tuple = make<basic_tuple_tag>;
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It, typename = void>
struct at_impl : at_impl<It, when<true>> {};

struct at_t {
    template <typename Xs, typename N>
    constexpr decltype(auto) operator()(Xs &&xs, N const &n) const;
};

constexpr at_t at{};

template <std::size_t n, typename Xs>
constexpr decltype(auto) at_c(Xs &&xs);

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct Sequence : Sequence<S, when<true>> {};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It, typename = void>
struct drop_front_impl : drop_front_impl<It, when<true>> {};

struct drop_front_t {
    template <typename Xs, typename N>
    constexpr auto operator()(Xs &&xs, N const &n) const;

    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr drop_front_t drop_front{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It, typename = void>
struct is_empty_impl : is_empty_impl<It, when<true>> {};

struct is_empty_t {
    template <typename Xs>
    constexpr auto operator()(Xs const &xs) const;
};

constexpr is_empty_t is_empty{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct length_impl : length_impl<T, when<true>> {};

struct length_t {
    template <typename Xs>
    constexpr auto operator()(Xs const &xs) const;
};

constexpr length_t length{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename = void>
struct transform_impl : transform_impl<Xs, when<true>> {};

struct transform_t {
    template <typename Xs, typename F>
    constexpr auto operator()(Xs &&xs, F &&f) const;
};

constexpr transform_t transform{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct unpack_impl : unpack_impl<T, when<true>> {};

struct unpack_t {
    template <typename Xs, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, F &&f) const;
};

constexpr unpack_t unpack{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <std::size_t>
struct bti;

struct from_other {};

template <typename Indices, typename... Xn>

struct basic_tuple_impl;

template <std::size_t... n, typename... Xn>

struct basic_tuple_impl<std::index_sequence<n...>, Xn...>

    : detail::ebo<bti<n>, Xn>... {
    static constexpr std::size_t size_ = sizeof...(Xn);

    constexpr basic_tuple_impl() = default;

    template <typename Other>
    explicit constexpr basic_tuple_impl(detail::from_other, Other &&other)
        : detail::ebo<bti<n>, Xn>(detail::ebo_get<bti<n>>(static_cast<Other &&>(other)))...
    {
    }

    template <typename... Yn>
    explicit constexpr basic_tuple_impl(Yn &&...yn) : detail::ebo<bti<n>, Xn>(static_cast<Yn &&>(yn))...
    {
    }
};
} // namespace detail

template <typename... Xn>
struct basic_tuple final : detail::basic_tuple_impl<std::make_index_sequence<sizeof...(Xn)>, Xn...> {
    using Base = detail::basic_tuple_impl<std::make_index_sequence<sizeof...(Xn)>, Xn...>;

    constexpr basic_tuple() = default;

    template <typename Other, typename = typename std::enable_if<
                                  std::is_same<typename detail::decay<Other>::type, basic_tuple>::value>::type>
    constexpr basic_tuple(Other &&other) : Base(detail::from_other{}, static_cast<Other &&>(other))
    {
    }

    template <typename... Yn>
    explicit constexpr basic_tuple(Yn &&...yn) : Base(static_cast<Yn &&>(yn)...)
    {
    }
};

template <typename... Xn>
struct tag_of<basic_tuple<Xn...>> {
    using type = basic_tuple_tag;
};

template <>
struct unpack_impl<basic_tuple_tag> {
    template <std::size_t... i, typename... Xn, typename F>
    static constexpr decltype(auto) apply(detail::basic_tuple_impl<std::index_sequence<i...>, Xn...> const &xs, F &&f)
    {
            return static_cast<F &&>(f)(
                detail::ebo_get<detail::bti<i>>(static_cast<detail::ebo<detail::bti<i>, Xn> const &>(xs))...);
    }

    template <std::size_t... i, typename... Xn, typename F>
    static constexpr decltype(auto) apply(detail::basic_tuple_impl<std::index_sequence<i...>, Xn...> &xs, F &&f)
    {
            return static_cast<F &&>(f)(
                detail::ebo_get<detail::bti<i>>(static_cast<detail::ebo<detail::bti<i>, Xn> &>(xs))...);
    }

    template <std::size_t... i, typename... Xn, typename F>
    static constexpr decltype(auto) apply(detail::basic_tuple_impl<std::index_sequence<i...>, Xn...> &&xs, F &&f)
    {
            return static_cast<F &&>(f)(
                detail::ebo_get<detail::bti<i>>(static_cast<detail::ebo<detail::bti<i>, Xn> &&>(xs))...);
    }
};

template <>
struct transform_impl<basic_tuple_tag> {
    template <std::size_t... i, typename... Xn, typename F>
    static constexpr auto apply(detail::basic_tuple_impl<std::index_sequence<i...>, Xn...> const &xs, F const &f)
    {
            return hana::make_basic_tuple(
                f(detail::ebo_get<detail::bti<i>>(static_cast<detail::ebo<detail::bti<i>, Xn> const &>(xs)))...);
    }

    template <std::size_t... i, typename... Xn, typename F>
    static constexpr auto apply(detail::basic_tuple_impl<std::index_sequence<i...>, Xn...> &xs, F const &f)
    {
            return hana::make_basic_tuple(
                f(detail::ebo_get<detail::bti<i>>(static_cast<detail::ebo<detail::bti<i>, Xn> &>(xs)))...);
    }

    template <std::size_t... i, typename... Xn, typename F>
    static constexpr auto apply(detail::basic_tuple_impl<std::index_sequence<i...>, Xn...> &&xs, F const &f)
    {
            return hana::make_basic_tuple(
                f(detail::ebo_get<detail::bti<i>>(static_cast<detail::ebo<detail::bti<i>, Xn> &&>(xs)))...);
    }
};

template <>
struct at_impl<basic_tuple_tag> {
    template <typename Xs, typename N>
    static constexpr decltype(auto) apply(Xs &&xs, N const &)
    {
            constexpr std::size_t index = N::value;
            return detail::ebo_get<detail::bti<index>>(static_cast<Xs &&>(xs));
    }
};

template <>
struct drop_front_impl<basic_tuple_tag> {
    template <std::size_t N, typename Xs, std::size_t... i>
    static constexpr auto drop_front_helper(Xs &&xs, std::index_sequence<i...>)
    {
            return hana::make_basic_tuple(detail::ebo_get<detail::bti<i + N>>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &)
    {
            constexpr std::size_t len = detail::decay<Xs>::type::size_;
            return drop_front_helper<N::value>(static_cast<Xs &&>(xs),
                                               std::make_index_sequence < (N::value < len) ? len - N::value : 0 > {});
    }
};

template <>
struct is_empty_impl<basic_tuple_tag> {
    template <typename... Xs>
    static constexpr hana::bool_<sizeof...(Xs) == 0> apply(basic_tuple<Xs...> const &)
    {
            return {};
    }
};

template <std::size_t n, typename... Xs>
constexpr decltype(auto) at_c(basic_tuple<Xs...> const &xs)
{
    return detail::ebo_get<detail::bti<n>>(xs);
}

template <std::size_t n, typename... Xs>
constexpr decltype(auto) at_c(basic_tuple<Xs...> &xs)
{
    return detail::ebo_get<detail::bti<n>>(xs);
}

template <std::size_t n, typename... Xs>
constexpr decltype(auto) at_c(basic_tuple<Xs...> &&xs)
{
    return detail::ebo_get<detail::bti<n>>(static_cast<basic_tuple<Xs...> &&>(xs));
}

template <>
struct Sequence<basic_tuple_tag> {
    static constexpr bool value = true;
};

template <>
struct make_impl<basic_tuple_tag> {
    template <typename... Xn>
    static constexpr basic_tuple<typename detail::decay<Xn>::type...> apply(Xn &&...xn)
    {
            return basic_tuple<typename detail::decay<Xn>::type...>{static_cast<Xn &&>(xn)...};
    }
};

template <>
struct length_impl<basic_tuple_tag> {
    template <typename... Xn>
    static constexpr auto apply(basic_tuple<Xn...> const &)
    {
            return hana::size_t<sizeof...(Xn)>{};
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Indices, typename F, typename... X>
struct partial_t;

struct make_partial_t {
    struct secret {};
    template <typename F, typename... X>
    constexpr partial_t<std::make_index_sequence<sizeof...(X)>, typename detail::decay<F>::type,
                        typename detail::decay<X>::type...>
    operator()(F &&f, X &&...x) const
    {
            return {secret{}, static_cast<F &&>(f), static_cast<X &&>(x)...};
    }
};

template <std::size_t... n, typename F, typename... X>
struct partial_t<std::index_sequence<n...>, F, X...> {
    partial_t() = default;

    template <typename... T>
    constexpr partial_t(make_partial_t::secret, T &&...t) : storage_{static_cast<T &&>(t)...}
    {
    }

    basic_tuple<F, X...> storage_;

    template <typename... Y>
    constexpr decltype(auto) operator()(Y &&...y) const &
    {
            return hana::at_c<0>(storage_)(hana::at_c<n + 1>(storage_)..., static_cast<Y &&>(y)...);
    }

    template <typename... Y>
    constexpr decltype(auto) operator()(Y &&...y) &
    {
            return hana::at_c<0>(storage_)(hana::at_c<n + 1>(storage_)..., static_cast<Y &&>(y)...);
    }

    template <typename... Y>
    constexpr decltype(auto) operator()(Y &&...y) &&
    {
            return static_cast<F &&>(hana::at_c<0>(storage_))(static_cast<X &&>(hana::at_c<n + 1>(storage_))...,
                                                              static_cast<Y &&>(y)...);
    }
};

constexpr make_partial_t partial{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It>
struct Iterable;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr auto drop_front_t::operator()(Xs &&xs, N const &n) const
{
    using It = typename hana::tag_of<Xs>::type;
    using DropFront = ::std::conditional_t<(hana::Iterable<It>::value && hana::IntegralConstant<N>::value),
                                           drop_front_impl<It>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It>::value, "hana::drop_front(xs, n) requires 'xs' to be an Iterable");

    static_assert(hana::IntegralConstant<N>::value, "hana::drop_front(xs, n) requires 'n' to be an IntegralConstant");

    return DropFront::apply(static_cast<Xs &&>(xs), n);
}

template <typename Xs>
constexpr auto drop_front_t::operator()(Xs &&xs) const
{
    return (*this)(static_cast<Xs &&>(xs), hana::size_t<1>{});
}

template <typename It, bool condition>
struct drop_front_impl<It, when<condition>> : default_ {
    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&, N const &) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto is_empty_t::operator()(Xs const &xs) const
{
    using It = typename hana::tag_of<Xs>::type;
    using IsEmpty =
        ::std::conditional_t<(hana::Iterable<It>::value), is_empty_impl<It>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It>::value, "hana::is_empty(xs) requires 'xs' to be an Iterable");

    return IsEmpty::apply(xs);
}

template <typename It, bool condition>
struct is_empty_impl<It, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename It>
struct Iterable : hana::integral_constant<bool, !is_default<at_impl<typename tag_of<It>::type>>::value
                                                    && !is_default<drop_front_impl<typename tag_of<It>::type>>::value
                                                    && !is_default<is_empty_impl<typename tag_of<It>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr decltype(auto) at_t::operator()(Xs &&xs, N const &n) const
{
    using It = typename hana::tag_of<Xs>::type;
    using At = ::std::conditional_t<(hana::Iterable<It>::value), at_impl<It>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It>::value, "hana::at(xs, n) requires 'xs' to be an Iterable");

    static_assert(hana::IntegralConstant<N>::value, "hana::at(xs, n) requires 'n' to be an IntegralConstant");

    return At::apply(static_cast<Xs &&>(xs), n);
}

template <typename It, bool condition>
struct at_impl<It, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <std::size_t n, typename Xs>
constexpr decltype(auto) at_c(Xs &&xs)
{
    return hana::at(static_cast<Xs &&>(xs), hana::size_t<n>{});
}
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename P>
struct Product;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Pair>
constexpr decltype(auto) second_t::operator()(Pair &&pair) const
{
    using P = typename hana::tag_of<Pair>::type;
    using Second =
        ::std::conditional_t<(hana::Product<P>::value), second_impl<P>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Product<P>::value, "hana::second(pair) requires 'pair' to be a Product");

    return Second::apply(static_cast<Pair &&>(pair));
}

template <typename P, bool condition>
struct second_impl<P, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename P>
struct Product : hana::integral_constant<bool, !is_default<first_impl<typename tag_of<P>::type>>::value
                                                   && !is_default<second_impl<typename tag_of<P>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Pair>
constexpr decltype(auto) first_t::operator()(Pair &&pair) const
{
    using P = typename hana::tag_of<Pair>::type;
    using First = ::std::conditional_t<(hana::Product<P>::value), first_impl<P>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Product<P>::value, "hana::first(pair) requires 'pair' to be a Product");

    return First::apply(static_cast<Pair &&>(pair));
}

template <typename P, bool condition>
struct first_impl<P, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto length_t::operator()(Xs const &xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Length =
        ::std::conditional_t<(hana::Foldable<S>::value), length_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::length(xs) requires 'xs' to be Foldable");

    return Length::apply(xs);
}

namespace detail
{
struct argn {
    template <typename... Xs>
    constexpr hana::size_t<sizeof...(Xs)> operator()(Xs const &...) const
    {
            return {};
    }
};
} // namespace detail

template <typename T, bool condition>
struct length_impl<T, when<condition>> : default_ {
    template <typename Xs>
    static constexpr auto apply(Xs const &xs)
    {
            return hana::unpack(xs, detail::argn{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename F>
constexpr decltype(auto) unpack_t::operator()(Xs &&xs, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Unpack =
        ::std::conditional_t<(hana::Foldable<S>::value), unpack_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::unpack(xs, f) requires 'xs' to be Foldable");

    return Unpack::apply(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

template <typename T, bool condition>
struct unpack_impl<T, when<condition>> : default_ {
    template <typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), static_cast<F &&>(f), hana::partial)();
    }
};

template <typename It>
struct unpack_impl<It, when<hana::Iterable<It>::value && !is_default<length_impl<It>>::value>> {
    template <typename Xs, typename F, std::size_t... i>
    static constexpr decltype(auto) unpack_helper(Xs &&xs, F &&f, std::index_sequence<i...>)
    {
            return static_cast<F &&>(f)(hana::at_c<i>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            constexpr std::size_t N = decltype(hana::length(xs))::value;
            return unpack_helper(static_cast<Xs &&>(xs), static_cast<F &&>(f), std::make_index_sequence<N>{});
    }
};

template <typename T, std::size_t N>
struct unpack_impl<T[N]> {
    template <typename Xs, typename F, std::size_t... i>
    static constexpr decltype(auto) unpack_helper(Xs &&xs, F &&f, std::index_sequence<i...>)
    {
            return static_cast<F &&>(f)(static_cast<Xs &&>(xs)[i]...);
    }

    template <typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            return unpack_impl::unpack_helper(static_cast<Xs &&>(xs), static_cast<F &&>(f),
                                              std::make_index_sequence<N>{});
    }
};

template <typename T>
struct unpack_impl<T, when<hana::Product<T>::value>> {
    template <typename P, typename F>
    static constexpr decltype(auto) apply(P &&p, F &&f)
    {
            return static_cast<F &&>(f)(hana::first(static_cast<P &&>(p)), hana::second(static_cast<P &&>(p)));
    }
};

namespace struct_detail
{

struct almost_demux {
    template <typename F, typename Udt, typename... Members>
    constexpr decltype(auto) operator()(F &&f, Udt &&udt, Members &&...g) const
    {
            return static_cast<F &&>(f)(
                hana::make_pair(hana::first(static_cast<Members &&>(g)),
                                hana::second(static_cast<Members &&>(g))(static_cast<Udt &&>(udt)))...);
    }
};
} // namespace struct_detail

template <typename S>
struct unpack_impl<S, when<hana::Struct<S>::value>> {
    template <typename Udt, typename F>
    static constexpr decltype(auto) apply(Udt &&udt, F &&f)
    {
            return hana::unpack(hana::accessors<S>(), hana::partial(struct_detail::almost_demux{}, static_cast<F &&>(f),
                                                                    static_cast<Udt &&>(udt)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename State, typename F>
constexpr decltype(auto) fold_left_t::operator()(Xs &&xs, State &&state, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using FoldLeft =
        ::std::conditional_t<(hana::Foldable<S>::value), fold_left_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::fold_left(xs, state, f) requires 'xs' to be Foldable");

    return FoldLeft::apply(static_cast<Xs &&>(xs), static_cast<State &&>(state), static_cast<F &&>(f));
}

template <typename Xs, typename F>
constexpr decltype(auto) fold_left_t::operator()(Xs &&xs, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using FoldLeft =
        ::std::conditional_t<(hana::Foldable<S>::value), fold_left_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::fold_left(xs, f) requires 'xs' to be Foldable");

    return FoldLeft::apply(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

namespace detail
{
template <typename F, typename State>
struct variadic_foldl1 {
    F &f;
    State &state;
    template <typename... T>
    constexpr decltype(auto) operator()(T &&...t) const
    {
            return detail::variadic::foldl1(static_cast<F &&>(f), static_cast<State &&>(state),
                                            static_cast<T &&>(t)...);
    }
};
} // namespace detail

template <typename T, bool condition>
struct fold_left_impl<T, when<condition>> : default_ {
    template <typename Xs, typename S, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, S &&s, F &&f)
    {
            return hana::unpack(static_cast<Xs &&>(xs), detail::variadic_foldl1<F, S>{f, s});
    }

    template <typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            return hana::unpack(static_cast<Xs &&>(xs), hana::partial(detail::variadic::foldl1, static_cast<F &&>(f)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename T>
struct Foldable : hana::integral_constant<bool, !is_default<fold_left_impl<typename tag_of<T>::type>>::value
                                                    || !is_default<unpack_impl<typename tag_of<T>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename S, typename Tag = typename hana::tag_of<S>::type>
struct sequence_dispatch : hana::integral_constant<bool, hana::Sequence<Tag>::value> {};

template <typename S>
struct sequence_dispatch<S, S> : hana::integral_constant<bool, false> {};
} // namespace detail

template <typename S, bool condition>
struct Sequence<S, when<condition>> : detail::sequence_dispatch<S> {};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct common;

template <typename T, typename U, typename = void>
struct has_common;

template <typename T, typename U>
using common_t = typename common<T, U>::type;
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename T>
struct CanonicalConstant {
    using value_type = T;
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct value_impl<detail::CanonicalConstant<T>> {
    template <typename X>
    static constexpr decltype(auto) apply()
    {
            return X::value;
    }
};

namespace detail
{
template <typename T, typename X>
struct canonical_constant {
    static constexpr auto value = hana::to<T>(hana::value<X>());
    using hana_tag = detail::CanonicalConstant<T>;
};
} // namespace detail

template <typename T, typename C>
struct to_impl<detail::CanonicalConstant<T>, C,
               when<hana::Constant<C>::value && is_convertible<typename C::value_type, T>::value>>
    : embedding<is_embedded<typename C::value_type, T>::value> {
    template <typename X>
    static constexpr detail::canonical_constant<T, X> apply(X const &)
    {
            return {};
    }
};

template <typename T>
struct IntegralConstant<detail::CanonicalConstant<T>> {
    static constexpr bool value = std::is_integral<T>::value;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename T, typename U, typename = void>
struct std_common_type {};

template <typename T, typename U>
struct std_common_type<T, U, decltype((void)(true ? std::declval<T>() : std::declval<U>()))> {
    using type = typename detail::decay<decltype(true ? std::declval<T>() : std::declval<U>())>::type;
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename...>
using void_t = void;
}
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename>
struct common : common<T, U, when<true>> {};

template <typename T, typename U, bool condition>
struct common<T, U, when<condition>> : detail::std_common_type<T, U> {};

template <typename T>
struct common<T, T> {
    using type = T;
};

template <typename T, typename U, typename>
struct has_common : std::false_type {};

template <typename T, typename U>
struct has_common<T, U, detail::void_t<typename common<T, U>::type>> : std::true_type {};

namespace constant_detail
{

template <typename A, typename B, typename C>
struct which {
    using type = detail::CanonicalConstant<C>;
};

template <template <typename...> class A, typename T, typename U, typename C>
struct which<A<T>, A<U>, C> {
    using type = A<C>;
};
} // namespace constant_detail

template <typename A, typename B>
struct common<A, B,
              when<hana::Constant<A>::value && hana::Constant<B>::value
                   && has_common<typename A::value_type, typename B::value_type>::value>> {
    using type =
        typename constant_detail::which<A, B,
                                        typename common<typename A::value_type, typename B::value_type>::type>::type;
};

template <typename A, typename B>
struct common<
    A, B, when<hana::Constant<A>::value && !hana::Constant<B>::value && has_common<typename A::value_type, B>::value>> {
    using type = typename common<typename A::value_type, B>::type;
};

template <typename A, typename B>
struct common<
    A, B, when<!hana::Constant<A>::value && hana::Constant<B>::value && has_common<A, typename B::value_type>::value>> {
    using type = typename common<A, typename B::value_type>::type;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Datatype, typename>
struct make_impl : make_impl<Datatype, when<true>> {};

template <typename Datatype, bool condition>
struct make_impl<Datatype, when<condition>> : default_ {
    template <typename... X>
    static constexpr auto make_helper(int, X &&...x) -> decltype(Datatype(static_cast<X &&>(x)...))
    {
            return Datatype(static_cast<X &&>(x)...);
    }

    template <typename... X>
    static constexpr auto make_helper(long, X &&...)
    {
            static_assert((sizeof...(X), false), "there exists no constructor for the given data type");
    }

    template <typename... X>
    static constexpr decltype(auto) apply(X &&...x)
    {
            return make_helper(int{}, static_cast<X &&>(x)...);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename...>
struct wrong : std::false_type {};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename To, typename From, typename>
struct to_impl : to_impl<To, From, when<true>> {};

namespace convert_detail
{
struct no_conversion {};

template <typename To, typename From, typename = void>
struct maybe_static_cast : no_conversion {
    template <typename X>
    static constexpr auto apply(X const &)
    {
            static_assert(detail::wrong<to_impl<To, From>, X>{},
                          "no conversion is available between the provided types");
    }
};

template <typename To, typename From>
struct maybe_static_cast<To, From, decltype((void)static_cast<To>(std::declval<From>()))> {
    template <typename X>
    static constexpr To apply(X &&x)
    {
            return static_cast<To>(static_cast<X &&>(x));
    }
};
} // namespace convert_detail

template <typename To, typename From, bool condition>
struct to_impl<To, From, when<condition>> : convert_detail::maybe_static_cast<To, From> {};

template <typename To>
struct to_impl<To, To> : embedding<> {
    template <typename X>
    static constexpr X apply(X &&x)
    {
            return static_cast<X &&>(x);
    }
};

template <typename To>
template <typename X>
constexpr decltype(auto) to_t<To>::operator()(X &&x) const
{
    using From = typename hana::tag_of<X>::type;
    return to_impl<To, From>::apply(static_cast<X &&>(x));
}

template <>
struct to_impl<long double, double> : embedding<> {
    static constexpr long double apply(double x)
    {
            return x;
    }
};
template <>
struct to_impl<long double, float> : embedding<> {
    static constexpr long double apply(float x)
    {
            return x;
    }
};
template <>
struct to_impl<double, float> : embedding<> {
    static constexpr double apply(float x)
    {
            return x;
    }
};

template <>
struct to_impl<signed long long, signed long> : embedding<> {
    static constexpr signed long long apply(signed long x)
    {
            return x;
    }
};
template <>
struct to_impl<signed long long, signed int> : embedding<> {
    static constexpr signed long long apply(signed int x)
    {
            return x;
    }
};
template <>
struct to_impl<signed long long, signed short> : embedding<> {
    static constexpr signed long long apply(signed short x)
    {
            return x;
    }
};
template <>
struct to_impl<signed long long, signed char> : embedding<> {
    static constexpr signed long long apply(signed char x)
    {
            return x;
    }
};
template <>
struct to_impl<signed long, signed int> : embedding<> {
    static constexpr signed long apply(signed int x)
    {
            return x;
    }
};
template <>
struct to_impl<signed long, signed short> : embedding<> {
    static constexpr signed long apply(signed short x)
    {
            return x;
    }
};
template <>
struct to_impl<signed long, signed char> : embedding<> {
    static constexpr signed long apply(signed char x)
    {
            return x;
    }
};
template <>
struct to_impl<signed int, signed short> : embedding<> {
    static constexpr signed int apply(signed short x)
    {
            return x;
    }
};
template <>
struct to_impl<signed int, signed char> : embedding<> {
    static constexpr signed int apply(signed char x)
    {
            return x;
    }
};
template <>
struct to_impl<signed short, signed char> : embedding<> {
    static constexpr signed short apply(signed char x)
    {
            return x;
    }
};

template <>
struct to_impl<unsigned long long, unsigned long> : embedding<> {
    static constexpr unsigned long long apply(unsigned long x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned long long, unsigned int> : embedding<> {
    static constexpr unsigned long long apply(unsigned int x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned long long, unsigned short> : embedding<> {
    static constexpr unsigned long long apply(unsigned short x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned long long, unsigned char> : embedding<> {
    static constexpr unsigned long long apply(unsigned char x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned long, unsigned int> : embedding<> {
    static constexpr unsigned long apply(unsigned int x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned long, unsigned short> : embedding<> {
    static constexpr unsigned long apply(unsigned short x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned long, unsigned char> : embedding<> {
    static constexpr unsigned long apply(unsigned char x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned int, unsigned short> : embedding<> {
    static constexpr unsigned int apply(unsigned short x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned int, unsigned char> : embedding<> {
    static constexpr unsigned int apply(unsigned char x)
    {
            return x;
    }
};
template <>
struct to_impl<unsigned short, unsigned char> : embedding<> {
    static constexpr unsigned short apply(unsigned char x)
    {
            return x;
    }
};

namespace detail
{
template <typename T>
struct copy_char_signedness {
    using type =
        typename std::conditional<std::is_signed<char>::value, std::make_signed<T>, std::make_unsigned<T>>::type::type;
};
} // namespace detail

template <>
struct to_impl<detail::copy_char_signedness<long long>::type, char> : embedding<> {
    static constexpr detail::copy_char_signedness<long long>::type apply(char x)
    {
            return x;
    }
};
template <>
struct to_impl<detail::copy_char_signedness<long>::type, char> : embedding<> {
    static constexpr detail::copy_char_signedness<long>::type apply(char x)
    {
            return x;
    }
};
template <>
struct to_impl<detail::copy_char_signedness<int>::type, char> : embedding<> {
    static constexpr detail::copy_char_signedness<int>::type apply(char x)
    {
            return x;
    }
};
template <>
struct to_impl<detail::copy_char_signedness<short>::type, char> : embedding<> {
    static constexpr detail::copy_char_signedness<short>::type apply(char x)
    {
            return x;
    }
};

template <typename T>
struct to_impl<T *, decltype(nullptr)> : embedding<> {
    static constexpr T *apply(decltype(nullptr))
    {
            return nullptr;
    }
};

template <typename From, typename To, typename>
struct is_convertible : std::true_type {};

template <typename From, typename To>
struct is_convertible<From, To, decltype((void)static_cast<convert_detail::no_conversion>(*(to_impl<To, From> *)0))>
    : std::false_type {};

template <typename From, typename To, typename>
struct is_embedded : std::false_type {};

template <typename From, typename To>
struct is_embedded<From, To, decltype((void)static_cast<embedding<true>>(*(to_impl<To, From> *)0))> : std::true_type {};

template <typename To, typename From>
struct to_impl<To, From, when<hana::Constant<From>::value && is_convertible<typename From::value_type, To>::value>>
    : embedding<is_embedded<typename From::value_type, To>::value> {
    template <typename X>
    static constexpr decltype(auto) apply(X const &)
    {
            return hana::to<To>(hana::value<X>());
    }
};

template <typename S, typename F>
struct to_impl<S, F, when<hana::Sequence<S>::value && hana::Foldable<F>::value>> : embedding<Sequence<F>::value> {
    template <typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::unpack(static_cast<Xs &&>(xs), hana::make<S>);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct div_impl : div_impl<T, U, when<true>> {};

struct div_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr div_t div{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct minus_impl : minus_impl<T, U, when<true>> {};

struct minus_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr minus_t minus{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct mod_impl : mod_impl<T, U, when<true>> {};

struct mod_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr mod_t mod{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct mult_impl : mult_impl<T, U, when<true>> {};

struct mult_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr mult_t mult{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename G, typename = void>
struct negate_impl : negate_impl<G, when<true>> {};

struct negate_t {
    template <typename X>
    constexpr decltype(auto) operator()(X &&x) const;
};

constexpr negate_t negate{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct plus_impl : plus_impl<T, U, when<true>> {};

struct plus_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr plus_t plus{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Tag>
struct arithmetic_operators {
    static constexpr bool value = false;
};

namespace operators
{
template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::arithmetic_operators<typename hana::tag_of<X>::type>::value
                                       || detail::arithmetic_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator+(X &&x, Y &&y)
{
    return hana::plus(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::arithmetic_operators<typename hana::tag_of<X>::type>::value
                                       || detail::arithmetic_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator-(X &&x, Y &&y)
{
    return hana::minus(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename X,
          typename = typename std::enable_if<detail::arithmetic_operators<typename hana::tag_of<X>::type>::value>::type>
constexpr auto operator-(X &&x)
{
    return hana::negate(static_cast<X &&>(x));
}

template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::arithmetic_operators<typename hana::tag_of<X>::type>::value
                                       || detail::arithmetic_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator*(X &&x, Y &&y)
{
    return hana::mult(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::arithmetic_operators<typename hana::tag_of<X>::type>::value
                                       || detail::arithmetic_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator/(X &&x, Y &&y)
{
    return hana::div(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <
    typename X, typename Y,
    typename = typename std::enable_if<detail::arithmetic_operators<typename hana::tag_of<X>::type>::value
                                       || detail::arithmetic_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator%(X &&x, Y &&y)
{
    return hana::mod(static_cast<X &&>(x), static_cast<Y &&>(y));
}
} // namespace operators
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename L, typename = void>
struct and_impl : and_impl<L, when<true>> {};

struct and_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;

    template <typename X, typename... Y>
    constexpr decltype(auto) operator()(X &&x, Y &&...y) const;
};

constexpr and_t and_{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename L, typename = void>
struct not_impl : not_impl<L, when<true>> {};

struct not_t {
    template <typename X>
    constexpr decltype(auto) operator()(X &&x) const;
};

constexpr not_t not_{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename L, typename = void>
struct or_impl : or_impl<L, when<true>> {};

struct or_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;

    template <typename X, typename... Y>
    constexpr decltype(auto) operator()(X &&x, Y &&...y) const;
};

constexpr or_t or_{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Tag>
struct logical_operators {
    static constexpr bool value = false;
};

namespace operators
{
template <typename X, typename Y,
          typename = typename std::enable_if<detail::logical_operators<typename hana::tag_of<X>::type>::value
                                             || detail::logical_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator||(X &&x, Y &&y)
{
    return hana::or_(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename X, typename Y,
          typename = typename std::enable_if<detail::logical_operators<typename hana::tag_of<X>::type>::value
                                             || detail::logical_operators<typename hana::tag_of<Y>::type>::value>::type>
constexpr auto operator&&(X &&x, Y &&y)
{
    return hana::and_(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename X,
          typename = typename std::enable_if<detail::logical_operators<typename hana::tag_of<X>::type>::value>::type>
constexpr auto operator!(X &&x)
{
    return hana::not_(static_cast<X &&>(x));
}
} // namespace operators
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct eval_impl : eval_impl<T, when<true>> {};

struct eval_t {
    template <typename Expr>
    constexpr decltype(auto) operator()(Expr &&expr) const;
};

constexpr eval_t eval{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct id_t {
    template <typename T>
    constexpr T operator()(T &&t) const
    {
            return static_cast<T &&>(t);
    }
};

constexpr id_t id{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Expr>
constexpr decltype(auto) eval_t::operator()(Expr &&expr) const
{
    return eval_impl<typename hana::tag_of<Expr>::type>::apply(static_cast<Expr &&>(expr));
}

template <typename T, bool condition>
struct eval_impl<T, when<condition>> : default_ {
    template <typename Expr>
    static constexpr auto eval_helper(Expr &&expr, int) -> decltype(static_cast<Expr &&>(expr)())
    {
            return static_cast<Expr &&>(expr)();
    }

    template <typename Expr>
    static constexpr auto eval_helper(Expr &&expr, long) -> decltype(static_cast<Expr &&>(expr)(hana::id))
    {
            return static_cast<Expr &&>(expr)(hana::id);
    }

    template <typename Expr>
    static constexpr auto eval_helper(Expr &&, ...)
    {
            static_assert(detail::wrong<Expr>{}, "hana::eval(expr) requires the expression to be a hana::lazy, "
                                                 "a nullary Callable or a unary Callable that may be "
                                                 "called with hana::id");
    }

    template <typename Expr>
    static constexpr decltype(auto) apply(Expr &&expr)
    {
            return eval_helper(static_cast<Expr &&>(expr), int{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename L, typename = void>
struct eval_if_impl : eval_if_impl<L, when<true>> {};

struct eval_if_t {
    template <typename Cond, typename Then, typename Else>
    constexpr decltype(auto) operator()(Cond &&cond, Then &&then, Else &&else_) const;
};

constexpr eval_if_t eval_if{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename L, typename = void>
struct if_impl : if_impl<L, when<true>> {};

struct if_t {
    template <typename Cond, typename Then, typename Else>
    constexpr decltype(auto) operator()(Cond &&cond, Then &&then, Else &&else_) const;
};

constexpr if_t if_{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

namespace ic_detail
{
template <typename T, T N, typename = std::make_integer_sequence<T, N>>
struct go;

template <typename T, T N, T... i>
struct go<T, N, std::integer_sequence<T, i...>> {
    using swallow = T[];

    template <typename F>
    static constexpr void with_index(F &&f)
    {
            (void)swallow{T{}, ((void)f(integral_constant<T, i>{}), i)...};
    }

    template <typename F>
    static constexpr void without_index(F &&f)
    {
            (void)swallow{T{}, ((void)f(), i)...};
    }
};

template <typename T, T v>
template <typename F>
constexpr void with_index_t<T, v>::operator()(F &&f) const
{
    go<T, ((void)sizeof(&f), v)>::with_index(static_cast<F &&>(f));
}

template <typename T, T v>
template <typename F>
constexpr void times_t<T, v>::operator()(F &&f) const
{
    go<T, ((void)sizeof(&f), v)>::without_index(static_cast<F &&>(f));
}

template <typename T, T v>
constexpr with_index_t<T, v> times_t<T, v>::with_index;
} // namespace ic_detail

template <typename T, T v>
constexpr ic_detail::times_t<T, v> integral_constant<T, v>::times;

template <typename T, T v>
struct tag_of<integral_constant<T, v>> {
    using type = integral_constant_tag<T>;
};

namespace detail
{
template <typename T>
struct comparable_operators<integral_constant_tag<T>> {
    static constexpr bool value = true;
};
template <typename T>
struct orderable_operators<integral_constant_tag<T>> {
    static constexpr bool value = true;
};
template <typename T>
struct arithmetic_operators<integral_constant_tag<T>> {
    static constexpr bool value = true;
};
template <typename T>
struct logical_operators<integral_constant_tag<T>> {
    static constexpr bool value = true;
};
} // namespace detail

template <typename U, U u>
constexpr integral_constant<decltype(+u), (+u)> operator+(integral_constant<U, u>)
{
    return {};
}

template <typename U, U u>
constexpr integral_constant<decltype(~u), (~u)> operator~(integral_constant<U, u>)
{
    return {};
}
template <typename U, U u, typename V, V v>
constexpr integral_constant<decltype(u & v), (u & v)> operator&(integral_constant<U, u>, integral_constant<V, v>)
{
    return {};
}
template <typename U, U u, typename V, V v>
constexpr integral_constant<decltype(u | v), (u | v)> operator|(integral_constant<U, u>, integral_constant<V, v>)
{
    return {};
}
template <typename U, U u, typename V, V v>
constexpr integral_constant<decltype(u ^ v), (u ^ v)> operator^(integral_constant<U, u>, integral_constant<V, v>)
{
    return {};
}
template <typename U, U u, typename V, V v>
constexpr integral_constant<decltype(u << v), (u << v)> operator<<(integral_constant<U, u>, integral_constant<V, v>)
{
    return {};
}
template <typename U, U u, typename V, V v>
constexpr integral_constant<decltype(u >> v), (u >> v)> operator>>(integral_constant<U, u>, integral_constant<V, v>)
{
    return {};
}

namespace ic_detail
{

constexpr int to_int(char c)
{
    int result = 0;

    if (c >= 'A' && c <= 'F') {
            result = static_cast<int>(c) - static_cast<int>('A') + 10;
    } else if (c >= 'a' && c <= 'f') {
            result = static_cast<int>(c) - static_cast<int>('a') + 10;
    } else {
            result = static_cast<int>(c) - static_cast<int>('0');
    }

    return result;
}

template <std::size_t N>
constexpr long long parse(const char (&arr)[N])
{
    long long base = 10;
    std::size_t offset = 0;

    if (N > 2) {
            bool starts_with_zero = arr[0] == '0';
            bool is_hex = starts_with_zero && arr[1] == 'x';
            bool is_binary = starts_with_zero && arr[1] == 'b';

            if (is_hex) {
                base = 16;
                offset = 2;
            } else if (is_binary) {
                base = 2;
                offset = 2;
            } else if (starts_with_zero) {
                base = 8;
                offset = 1;
            }
    }

    long long number = 0;
    long long multiplier = 1;

    for (std::size_t i = 0; i < N - offset; ++i) {
            char c = arr[N - 1 - i];
            if (c != '\'') {
                number += to_int(c) * multiplier;
                multiplier *= base;
            }
    }

    return number;
}
} // namespace ic_detail

namespace literals
{
template <char... c>
constexpr auto operator"" _c()
{
    return hana::llong<ic_detail::parse<sizeof...(c)>({c...})>{};
}
} // namespace literals

template <typename T>
struct IntegralConstant<integral_constant_tag<T>> {
    static constexpr bool value = true;
};

template <typename T, typename C>
struct to_impl<integral_constant_tag<T>, C, when<hana::IntegralConstant<C>::value>>
    : embedding<is_embedded<typename C::value_type, T>::value> {
    template <typename N>
    static constexpr auto apply(N const &)
    {
            return integral_constant<T, N::value>{};
    }
};

template <typename T>
struct eval_if_impl<integral_constant_tag<T>> {
    template <typename Cond, typename Then, typename Else>
    static constexpr decltype(auto) apply(Cond const &, Then &&t, Else &&e)
    {
            constexpr bool cond = static_cast<bool>(Cond::value);
            return eval_if_impl::apply(hana::bool_<cond>{}, static_cast<Then &&>(t), static_cast<Else &&>(e));
    }

    template <typename Then, typename Else>
    static constexpr decltype(auto) apply(hana::true_ const &, Then &&t, Else &&)
    {
            return hana::eval(static_cast<Then &&>(t));
    }

    template <typename Then, typename Else>
    static constexpr decltype(auto) apply(hana::false_ const &, Then &&, Else &&e)
    {
            return hana::eval(static_cast<Else &&>(e));
    }
};

template <typename T>
struct if_impl<integral_constant_tag<T>> {
    template <typename Cond, typename Then, typename Else>
    static constexpr decltype(auto) apply(Cond const &, Then &&t, Else &&e)
    {
            constexpr bool cond = static_cast<bool>(Cond::value);
            return if_impl::apply(hana::bool_<cond>{}, static_cast<Then &&>(t), static_cast<Else &&>(e));
    }

    template <typename Then, typename Else>
    static constexpr auto apply(hana::true_ const &, Then &&t, Else &&)
    {
            return static_cast<Then &&>(t);
    }

    template <typename Then, typename Else>
    static constexpr auto apply(hana::false_ const &, Then &&, Else &&e)
    {
            return static_cast<Else &&>(e);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <template <typename...> class T>
struct create {
    template <typename... X>
    constexpr T<typename detail::decay<X>::type...> operator()(X &&...x) const
    {
            return T<typename detail::decay<X>::type...>{static_cast<X &&>(x)...};
    }
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

namespace placeholder_detail
{
template <typename I>
struct subscript {
    I i;

    template <typename Xs, typename... Z>
    constexpr auto operator()(Xs &&xs, Z const &...) const & -> decltype(static_cast<Xs &&>(xs)[i])
    {
            return static_cast<Xs &&>(xs)[i];
    }

    template <typename Xs, typename... Z>
    constexpr auto operator()(Xs &&xs, Z const &...) & -> decltype(static_cast<Xs &&>(xs)[i])
    {
            return static_cast<Xs &&>(xs)[i];
    }

    template <typename Xs, typename... Z>
    constexpr auto operator()(Xs &&xs, Z const &...) && -> decltype(static_cast<Xs &&>(xs)[std::declval<I>()])
    {
            return static_cast<Xs &&>(xs)[std::move(i)];
    }
};

template <typename F, typename Xs, std::size_t... i>
constexpr decltype(auto) invoke_impl(F &&f, Xs &&xs, std::index_sequence<i...>)
{
    return static_cast<F &&>(f)(hana::at_c<i>(static_cast<Xs &&>(xs).storage_)...);
}

template <typename... X>
struct invoke;

struct placeholder {
    struct secret {};

    template <typename X>
    constexpr decltype(auto) operator[](X &&x) const
    {
            return detail::create<subscript>{}(static_cast<X &&>(x));
    }

    template <typename... X>
    constexpr invoke<typename detail::decay<X>::type...> operator()(X &&...x) const
    {
            return {secret{}, static_cast<X &&>(x)...};
    }
};

template <typename... X>
struct invoke {
    template <typename... Y>
    constexpr invoke(placeholder::secret, Y &&...y) : storage_{static_cast<Y &&>(y)...}
    {
    }

    basic_tuple<X...> storage_;

    template <typename F, typename... Z>
    constexpr auto operator()(F &&f,
                              Z const &...) const & -> decltype(static_cast<F &&>(f)(std::declval<X const &>()...))
    {
            return invoke_impl(static_cast<F &&>(f), *this, std::make_index_sequence<sizeof...(X)>{});
    }

    template <typename F, typename... Z>
    constexpr auto operator()(F &&f, Z const &...) & -> decltype(static_cast<F &&>(f)(std::declval<X &>()...))
    {
            return invoke_impl(static_cast<F &&>(f), *this, std::make_index_sequence<sizeof...(X)>{});
    }

    template <typename F, typename... Z>
    constexpr auto operator()(F &&f, Z const &...) && -> decltype(static_cast<F &&>(f)(std::declval<X &&>()...))
    {
            return invoke_impl(static_cast<F &&>(f), static_cast<invoke &&>(*this),
                               std::make_index_sequence<sizeof...(X)>{});
    }
};

struct unary_plus {
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const -> decltype(+static_cast<X &&>(x))
    {
            return +static_cast<X &&>(x);
    }
};
inline constexpr decltype(auto) operator+(placeholder)
{
    return unary_plus{};
}
struct unary_minus {
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const -> decltype(-static_cast<X &&>(x))
    {
            return -static_cast<X &&>(x);
    }
};
inline constexpr decltype(auto) operator-(placeholder)
{
    return unary_minus{};
}
template <typename X>
struct plus_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() + static_cast<Y &&>(y))
    {
            return x + static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() + static_cast<Y &&>(y))
    {
            return x + static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() + static_cast<Y &&>(y))
    {
            return std::move(x) + static_cast<Y &&>(y);
    }
};
template <typename Y>
struct plus_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) + std::declval<Y const &>())
    {
            return static_cast<X &&>(x) + y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) + std::declval<Y &>())
    {
            return static_cast<X &&>(x) + y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) + std::declval<Y>())
    {
            return static_cast<X &&>(x) + std::move(y);
    }
};
struct plus {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) + static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) + static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator+(X &&x, placeholder)
{
    return detail::create<plus_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator+(placeholder, Y &&y)
{
    return detail::create<plus_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator+(placeholder, placeholder)
{
    return plus{};
}
template <typename X>
struct minus_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() - static_cast<Y &&>(y))
    {
            return x - static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() - static_cast<Y &&>(y))
    {
            return x - static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() - static_cast<Y &&>(y))
    {
            return std::move(x) - static_cast<Y &&>(y);
    }
};
template <typename Y>
struct minus_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) - std::declval<Y const &>())
    {
            return static_cast<X &&>(x) - y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) - std::declval<Y &>())
    {
            return static_cast<X &&>(x) - y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) - std::declval<Y>())
    {
            return static_cast<X &&>(x) - std::move(y);
    }
};
struct minus {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) - static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) - static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator-(X &&x, placeholder)
{
    return detail::create<minus_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator-(placeholder, Y &&y)
{
    return detail::create<minus_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator-(placeholder, placeholder)
{
    return minus{};
}
template <typename X>
struct times_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() * static_cast<Y &&>(y))
    {
            return x * static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() * static_cast<Y &&>(y))
    {
            return x * static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() * static_cast<Y &&>(y))
    {
            return std::move(x) * static_cast<Y &&>(y);
    }
};
template <typename Y>
struct times_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) * std::declval<Y const &>())
    {
            return static_cast<X &&>(x) * y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) * std::declval<Y &>())
    {
            return static_cast<X &&>(x) * y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) * std::declval<Y>())
    {
            return static_cast<X &&>(x) * std::move(y);
    }
};
struct times {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) * static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) * static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator*(X &&x, placeholder)
{
    return detail::create<times_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator*(placeholder, Y &&y)
{
    return detail::create<times_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator*(placeholder, placeholder)
{
    return times{};
}
template <typename X>
struct divide_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() / static_cast<Y &&>(y))
    {
            return x / static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() / static_cast<Y &&>(y))
    {
            return x / static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() / static_cast<Y &&>(y))
    {
            return std::move(x) / static_cast<Y &&>(y);
    }
};
template <typename Y>
struct divide_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) / std::declval<Y const &>())
    {
            return static_cast<X &&>(x) / y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) / std::declval<Y &>())
    {
            return static_cast<X &&>(x) / y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) / std::declval<Y>())
    {
            return static_cast<X &&>(x) / std::move(y);
    }
};
struct divide {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) / static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) / static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator/(X &&x, placeholder)
{
    return detail::create<divide_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator/(placeholder, Y &&y)
{
    return detail::create<divide_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator/(placeholder, placeholder)
{
    return divide{};
}
template <typename X>
struct modulo_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() % static_cast<Y &&>(y))
    {
            return x % static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() % static_cast<Y &&>(y))
    {
            return x % static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() % static_cast<Y &&>(y))
    {
            return std::move(x) % static_cast<Y &&>(y);
    }
};
template <typename Y>
struct modulo_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) % std::declval<Y const &>())
    {
            return static_cast<X &&>(x) % y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) % std::declval<Y &>())
    {
            return static_cast<X &&>(x) % y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) % std::declval<Y>())
    {
            return static_cast<X &&>(x) % std::move(y);
    }
};
struct modulo {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) % static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) % static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator%(X &&x, placeholder)
{
    return detail::create<modulo_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator%(placeholder, Y &&y)
{
    return detail::create<modulo_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator%(placeholder, placeholder)
{
    return modulo{};
}

struct bitwise_not {
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const -> decltype(~static_cast<X &&>(x))
    {
            return ~static_cast<X &&>(x);
    }
};
inline constexpr decltype(auto) operator~(placeholder)
{
    return bitwise_not{};
}
template <typename X>
struct bitwise_and_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() & static_cast<Y &&>(y))
    {
            return x & static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() & static_cast<Y &&>(y))
    {
            return x & static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() & static_cast<Y &&>(y))
    {
            return std::move(x) & static_cast<Y &&>(y);
    }
};
template <typename Y>
struct bitwise_and_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) & std::declval<Y const &>())
    {
            return static_cast<X &&>(x) & y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) & std::declval<Y &>())
    {
            return static_cast<X &&>(x) & y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) & std::declval<Y>())
    {
            return static_cast<X &&>(x) & std::move(y);
    }
};
struct bitwise_and {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) & static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) & static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator&(X &&x, placeholder)
{
    return detail::create<bitwise_and_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator&(placeholder, Y &&y)
{
    return detail::create<bitwise_and_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator&(placeholder, placeholder)
{
    return bitwise_and{};
}
template <typename X>
struct bitwise_or_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() | static_cast<Y &&>(y))
    {
            return x | static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() | static_cast<Y &&>(y))
    {
            return x | static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() | static_cast<Y &&>(y))
    {
            return std::move(x) | static_cast<Y &&>(y);
    }
};
template <typename Y>
struct bitwise_or_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) | std::declval<Y const &>())
    {
            return static_cast<X &&>(x) | y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) | std::declval<Y &>())
    {
            return static_cast<X &&>(x) | y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) | std::declval<Y>())
    {
            return static_cast<X &&>(x) | std::move(y);
    }
};
struct bitwise_or {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) | static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) | static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator|(X &&x, placeholder)
{
    return detail::create<bitwise_or_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator|(placeholder, Y &&y)
{
    return detail::create<bitwise_or_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator|(placeholder, placeholder)
{
    return bitwise_or{};
}
template <typename X>
struct bitwise_xor_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() ^ static_cast<Y &&>(y))
    {
            return x ^ static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() ^ static_cast<Y &&>(y))
    {
            return x ^ static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() ^ static_cast<Y &&>(y))
    {
            return std::move(x) ^ static_cast<Y &&>(y);
    }
};
template <typename Y>
struct bitwise_xor_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) ^ std::declval<Y const &>())
    {
            return static_cast<X &&>(x) ^ y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) ^ std::declval<Y &>())
    {
            return static_cast<X &&>(x) ^ y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) ^ std::declval<Y>())
    {
            return static_cast<X &&>(x) ^ std::move(y);
    }
};
struct bitwise_xor {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) ^ static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) ^ static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator^(X &&x, placeholder)
{
    return detail::create<bitwise_xor_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator^(placeholder, Y &&y)
{
    return detail::create<bitwise_xor_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator^(placeholder, placeholder)
{
    return bitwise_xor{};
}
template <typename X>
struct left_shift_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>()
                                                                       << static_cast<Y &&>(y))
    {
            return x << static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() << static_cast<Y &&>(y))
    {
            return x << static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() << static_cast<Y &&>(y))
    {
            return std::move(x) << static_cast<Y &&>(y);
    }
};
template <typename Y>
struct left_shift_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x)
                                                                       << std::declval<Y const &>())
    {
            return static_cast<X &&>(x) << y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) << std::declval<Y &>())
    {
            return static_cast<X &&>(x) << y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) << std::declval<Y>())
    {
            return static_cast<X &&>(x) << std::move(y);
    }
};
struct left_shift {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) << static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) << static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator<<(X &&x, placeholder)
{
    return detail::create<left_shift_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator<<(placeholder, Y &&y)
{
    return detail::create<left_shift_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator<<(placeholder, placeholder)
{
    return left_shift{};
}
template <typename X>
struct right_shift_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y,
                              Z const &...) const & -> decltype(std::declval<X const &>() >> static_cast<Y &&>(y))
    {
            return x >> static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() >> static_cast<Y &&>(y))
    {
            return x >> static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() >> static_cast<Y &&>(y))
    {
            return std::move(x) >> static_cast<Y &&>(y);
    }
};
template <typename Y>
struct right_shift_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x,
                              Z const &...) const & -> decltype(static_cast<X &&>(x) >> std::declval<Y const &>())
    {
            return static_cast<X &&>(x) >> y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) >> std::declval<Y &>())
    {
            return static_cast<X &&>(x) >> y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) >> std::declval<Y>())
    {
            return static_cast<X &&>(x) >> std::move(y);
    }
};
struct right_shift {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) >> static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) >> static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator>>(X &&x, placeholder)
{
    return detail::create<right_shift_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator>>(placeholder, Y &&y)
{
    return detail::create<right_shift_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator>>(placeholder, placeholder)
{
    return right_shift{};
}

template <typename X>
struct equal_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y,
                              Z const &...) const & -> decltype(std::declval<X const &>() == static_cast<Y &&>(y))
    {
            return x == static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() == static_cast<Y &&>(y))
    {
            return x == static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() == static_cast<Y &&>(y))
    {
            return std::move(x) == static_cast<Y &&>(y);
    }
};
template <typename Y>
struct equal_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x,
                              Z const &...) const & -> decltype(static_cast<X &&>(x) == std::declval<Y const &>())
    {
            return static_cast<X &&>(x) == y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) == std::declval<Y &>())
    {
            return static_cast<X &&>(x) == y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) == std::declval<Y>())
    {
            return static_cast<X &&>(x) == std::move(y);
    }
};
struct equal {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) == static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) == static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator==(X &&x, placeholder)
{
    return detail::create<equal_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator==(placeholder, Y &&y)
{
    return detail::create<equal_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator==(placeholder, placeholder)
{
    return equal{};
}
template <typename X>
struct not_equal_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y,
                              Z const &...) const & -> decltype(std::declval<X const &>() != static_cast<Y &&>(y))
    {
            return x != static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() != static_cast<Y &&>(y))
    {
            return x != static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() != static_cast<Y &&>(y))
    {
            return std::move(x) != static_cast<Y &&>(y);
    }
};
template <typename Y>
struct not_equal_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x,
                              Z const &...) const & -> decltype(static_cast<X &&>(x) != std::declval<Y const &>())
    {
            return static_cast<X &&>(x) != y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) != std::declval<Y &>())
    {
            return static_cast<X &&>(x) != y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) != std::declval<Y>())
    {
            return static_cast<X &&>(x) != std::move(y);
    }
};
struct not_equal {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) != static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) != static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator!=(X &&x, placeholder)
{
    return detail::create<not_equal_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator!=(placeholder, Y &&y)
{
    return detail::create<not_equal_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator!=(placeholder, placeholder)
{
    return not_equal{};
}
template <typename X>
struct less_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() < static_cast<Y &&>(y))
    {
            return x < static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() < static_cast<Y &&>(y))
    {
            return x < static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() < static_cast<Y &&>(y))
    {
            return std::move(x) < static_cast<Y &&>(y);
    }
};
template <typename Y>
struct less_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) < std::declval<Y const &>())
    {
            return static_cast<X &&>(x) < y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) < std::declval<Y &>())
    {
            return static_cast<X &&>(x) < y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) < std::declval<Y>())
    {
            return static_cast<X &&>(x) < std::move(y);
    }
};
struct less {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) < static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) < static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator<(X &&x, placeholder)
{
    return detail::create<less_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator<(placeholder, Y &&y)
{
    return detail::create<less_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator<(placeholder, placeholder)
{
    return less{};
}
template <typename X>
struct less_equal_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y,
                              Z const &...) const & -> decltype(std::declval<X const &>() <= static_cast<Y &&>(y))
    {
            return x <= static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() <= static_cast<Y &&>(y))
    {
            return x <= static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() <= static_cast<Y &&>(y))
    {
            return std::move(x) <= static_cast<Y &&>(y);
    }
};
template <typename Y>
struct less_equal_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x,
                              Z const &...) const & -> decltype(static_cast<X &&>(x) <= std::declval<Y const &>())
    {
            return static_cast<X &&>(x) <= y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) <= std::declval<Y &>())
    {
            return static_cast<X &&>(x) <= y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) <= std::declval<Y>())
    {
            return static_cast<X &&>(x) <= std::move(y);
    }
};
struct less_equal {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) <= static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) <= static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator<=(X &&x, placeholder)
{
    return detail::create<less_equal_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator<=(placeholder, Y &&y)
{
    return detail::create<less_equal_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator<=(placeholder, placeholder)
{
    return less_equal{};
}
template <typename X>
struct greater_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) const & -> decltype(std::declval<X const &>() > static_cast<Y &&>(y))
    {
            return x > static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() > static_cast<Y &&>(y))
    {
            return x > static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() > static_cast<Y &&>(y))
    {
            return std::move(x) > static_cast<Y &&>(y);
    }
};
template <typename Y>
struct greater_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const & -> decltype(static_cast<X &&>(x) > std::declval<Y const &>())
    {
            return static_cast<X &&>(x) > y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) > std::declval<Y &>())
    {
            return static_cast<X &&>(x) > y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) > std::declval<Y>())
    {
            return static_cast<X &&>(x) > std::move(y);
    }
};
struct greater {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const -> decltype(static_cast<X &&>(x) > static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) > static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator>(X &&x, placeholder)
{
    return detail::create<greater_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator>(placeholder, Y &&y)
{
    return detail::create<greater_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator>(placeholder, placeholder)
{
    return greater{};
}
template <typename X>
struct greater_equal_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y,
                              Z const &...) const & -> decltype(std::declval<X const &>() >= static_cast<Y &&>(y))
    {
            return x >= static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() >= static_cast<Y &&>(y))
    {
            return x >= static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() >= static_cast<Y &&>(y))
    {
            return std::move(x) >= static_cast<Y &&>(y);
    }
};
template <typename Y>
struct greater_equal_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x,
                              Z const &...) const & -> decltype(static_cast<X &&>(x) >= std::declval<Y const &>())
    {
            return static_cast<X &&>(x) >= y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) >= std::declval<Y &>())
    {
            return static_cast<X &&>(x) >= y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) >= std::declval<Y>())
    {
            return static_cast<X &&>(x) >= std::move(y);
    }
};
struct greater_equal {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) >= static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) >= static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator>=(X &&x, placeholder)
{
    return detail::create<greater_equal_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator>=(placeholder, Y &&y)
{
    return detail::create<greater_equal_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator>=(placeholder, placeholder)
{
    return greater_equal{};
}

template <typename X>
struct logical_or_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y,
                              Z const &...) const & -> decltype(std::declval<X const &>() || static_cast<Y &&>(y))
    {
            return x || static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() || static_cast<Y &&>(y))
    {
            return x || static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() || static_cast<Y &&>(y))
    {
            return std::move(x) || static_cast<Y &&>(y);
    }
};
template <typename Y>
struct logical_or_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x,
                              Z const &...) const & -> decltype(static_cast<X &&>(x) || std::declval<Y const &>())
    {
            return static_cast<X &&>(x) || y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) || std::declval<Y &>())
    {
            return static_cast<X &&>(x) || y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) || std::declval<Y>())
    {
            return static_cast<X &&>(x) || std::move(y);
    }
};
struct logical_or {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) || static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) || static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator||(X &&x, placeholder)
{
    return detail::create<logical_or_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator||(placeholder, Y &&y)
{
    return detail::create<logical_or_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator||(placeholder, placeholder)
{
    return logical_or{};
}
template <typename X>
struct logical_and_left {
    X x;
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y,
                              Z const &...) const & -> decltype(std::declval<X const &>() && static_cast<Y &&>(y))
    {
            return x && static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) & -> decltype(std::declval<X &>() && static_cast<Y &&>(y))
    {
            return x && static_cast<Y &&>(y);
    }
    template <typename Y, typename... Z>
    constexpr auto operator()(Y &&y, Z const &...) && -> decltype(std::declval<X>() && static_cast<Y &&>(y))
    {
            return std::move(x) && static_cast<Y &&>(y);
    }
};
template <typename Y>
struct logical_and_right {
    Y y;
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x,
                              Z const &...) const & -> decltype(static_cast<X &&>(x) && std::declval<Y const &>())
    {
            return static_cast<X &&>(x) && y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) & -> decltype(static_cast<X &&>(x) && std::declval<Y &>())
    {
            return static_cast<X &&>(x) && y;
    }
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) && -> decltype(static_cast<X &&>(x) && std::declval<Y>())
    {
            return static_cast<X &&>(x) && std::move(y);
    }
};
struct logical_and {
    template <typename X, typename Y, typename... Z>
    constexpr auto operator()(X &&x, Y &&y, Z const &...) const
        -> decltype(static_cast<X &&>(x) && static_cast<Y &&>(y))
    {
            return static_cast<X &&>(x) && static_cast<Y &&>(y);
    }
};
template <typename X>
constexpr decltype(auto) operator&&(X &&x, placeholder)
{
    return detail::create<logical_and_left>{}(static_cast<X &&>(x));
}
template <typename Y>
constexpr decltype(auto) operator&&(placeholder, Y &&y)
{
    return detail::create<logical_and_right>{}(static_cast<Y &&>(y));
}
inline constexpr decltype(auto) operator&&(placeholder, placeholder)
{
    return logical_and{};
}
struct logical_not {
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const -> decltype(!static_cast<X &&>(x))
    {
            return !static_cast<X &&>(x);
    }
};
inline constexpr decltype(auto) operator!(placeholder)
{
    return logical_not{};
}

struct dereference {
    template <typename X, typename... Z>
    constexpr auto operator()(X &&x, Z const &...) const -> decltype(*static_cast<X &&>(x))
    {
            return *static_cast<X &&>(x);
    }
};
inline constexpr decltype(auto) operator*(placeholder)
{
    return dereference{};
}

} // namespace placeholder_detail

constexpr placeholder_detail::placeholder _{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename T>
constexpr void constexpr_swap(T &x, T &y)
{
    auto tmp = x;
    x = y;
    y = std::move(tmp);
}

template <typename BidirIter>
constexpr void reverse(BidirIter first, BidirIter last)
{
    while (first != last) {
            if (first == --last) break;
            detail::constexpr_swap(*first, *last);
            ++first;
    }
}

template <typename BidirIter, typename BinaryPred>
constexpr bool next_permutation(BidirIter first, BidirIter last, BinaryPred pred)
{
    BidirIter i = last;
    if (first == last || first == --i) return false;
    while (true) {
            BidirIter ip1 = i;
            if (pred(*--i, *ip1)) {
                BidirIter j = last;
                while (!pred(*i, *--j))
                    ;
                detail::constexpr_swap(*i, *j);
                detail::reverse(ip1, last);
                return true;
            }
            if (i == first) {
                detail::reverse(first, last);
                return false;
            }
    }
}

template <typename BidirIter>
constexpr bool next_permutation(BidirIter first, BidirIter last)
{
    return detail::next_permutation(first, last, hana::_ < hana::_);
}

template <typename InputIter1, typename InputIter2, typename BinaryPred>
constexpr bool lexicographical_compare(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2,
                                       BinaryPred pred)
{
    for (; first2 != last2; ++first1, ++first2) {
            if (first1 == last1 || pred(*first1, *first2))
                return true;
            else if (pred(*first2, *first1))
                return false;
    }
    return false;
}

template <typename InputIter1, typename InputIter2>
constexpr bool lexicographical_compare(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2)
{
    return detail::lexicographical_compare(first1, last1, first2, last2, hana::_ < hana::_);
}

template <typename InputIter1, typename InputIter2, typename BinaryPred>
constexpr bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, BinaryPred pred)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
            if (!pred(*first1, *first2)) return false;
    return first1 == last1 && first2 == last2;
}

template <typename InputIter1, typename InputIter2>
constexpr bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2)
{
    return detail::equal(first1, last1, first2, last2, hana::_ == hana::_);
}

template <typename BidirIter, typename BinaryPred>
constexpr void sort(BidirIter first, BidirIter last, BinaryPred pred)
{
    if (first == last) return;

    BidirIter i = first;
    for (++i; i != last; ++i) {
            BidirIter j = i;
            auto t = *j;
            for (BidirIter k = i; k != first && pred(t, *--k); --j) *j = *k;
            *j = t;
    }
}

template <typename BidirIter>
constexpr void sort(BidirIter first, BidirIter last)
{
    detail::sort(first, last, hana::_ < hana::_);
}

template <typename InputIter, typename T>
constexpr InputIter find(InputIter first, InputIter last, T const &value)
{
    for (; first != last; ++first)
            if (*first == value) return first;
    return last;
}

template <typename InputIter, typename UnaryPred>
constexpr InputIter find_if(InputIter first, InputIter last, UnaryPred pred)
{
    for (; first != last; ++first)
            if (pred(*first)) return first;
    return last;
}

template <typename ForwardIter, typename T>
constexpr void iota(ForwardIter first, ForwardIter last, T value)
{
    while (first != last) {
            *first++ = value;
            ++value;
    }
}

template <typename InputIt, typename T>
constexpr std::size_t count(InputIt first, InputIt last, T const &value)
{
    std::size_t n = 0;
    for (; first != last; ++first)
            if (*first == value) ++n;
    return n;
}

template <typename InputIt, typename T, typename F>
constexpr T accumulate(InputIt first, InputIt last, T init, F f)
{
    for (; first != last; ++first) init = f(init, *first);
    return init;
}

template <typename InputIt, typename T>
constexpr T accumulate(InputIt first, InputIt last, T init)
{
    return detail::accumulate(first, last, init, hana::_ + hana::_);
}

template <typename ForwardIt>
constexpr ForwardIt min_element(ForwardIt first, ForwardIt last)
{
    if (first == last) return last;

    ForwardIt smallest = first;
    ++first;
    for (; first != last; ++first)
            if (*first < *smallest) smallest = first;
    return smallest;
}
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Derived>
struct iterable_operators {
    template <typename N>
    constexpr decltype(auto) operator[](N &&n) &
    {
            return hana::at(static_cast<Derived &>(*this), static_cast<N &&>(n));
    }

    template <typename N>
    constexpr decltype(auto) operator[](N &&n) const &
    {
            return hana::at(static_cast<Derived const &>(*this), static_cast<N &&>(n));
    }

    template <typename N>
    constexpr decltype(auto) operator[](N &&n) &&
    {
            return hana::at(static_cast<Derived &&>(*this), static_cast<N &&>(n));
    }
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F>
struct flip_t {
    F f;

    template <typename X, typename Y, typename... Z>
    constexpr decltype(auto) operator()(X &&x, Y &&y, Z &&...z) const &
    {
            return f(static_cast<Y &&>(y), static_cast<X &&>(x), static_cast<Z &&>(z)...);
    }

    template <typename X, typename Y, typename... Z>
    constexpr decltype(auto) operator()(X &&x, Y &&y, Z &&...z) &
    {
            return f(static_cast<Y &&>(y), static_cast<X &&>(x), static_cast<Z &&>(z)...);
    }

    template <typename X, typename Y, typename... Z>
    constexpr decltype(auto) operator()(X &&x, Y &&y, Z &&...z) &&
    {
            return std::move(f)(static_cast<Y &&>(y), static_cast<X &&>(x), static_cast<Z &&>(z)...);
    }
};

constexpr detail::create<flip_t> flip{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Indices, typename F, typename... X>
struct reverse_partial_t;

struct make_reverse_partial_t {
    struct secret {};
    template <typename F, typename... X>
    constexpr reverse_partial_t<std::make_index_sequence<sizeof...(X)>, typename detail::decay<F>::type,
                                typename detail::decay<X>::type...>
    operator()(F &&f, X &&...x) const
    {
            return {secret{}, static_cast<F &&>(f), static_cast<X &&>(x)...};
    }
};

template <std::size_t... n, typename F, typename... X>
struct reverse_partial_t<std::index_sequence<n...>, F, X...> {
    reverse_partial_t() = default;

    template <typename... T>
    constexpr reverse_partial_t(make_reverse_partial_t::secret, T &&...t) : storage_{static_cast<T &&>(t)...}
    {
    }

    basic_tuple<F, X...> storage_;

    template <typename... Y>
    constexpr decltype(auto) operator()(Y &&...y) const &
    {
            return hana::at_c<0>(storage_)(static_cast<Y &&>(y)..., hana::at_c<n + 1>(storage_)...);
    }

    template <typename... Y>
    constexpr decltype(auto) operator()(Y &&...y) &
    {
            return hana::at_c<0>(storage_)(static_cast<Y &&>(y)..., hana::at_c<n + 1>(storage_)...);
    }

    template <typename... Y>
    constexpr decltype(auto) operator()(Y &&...y) &&
    {
            return static_cast<F &&>(hana::at_c<0>(storage_))(static_cast<Y &&>(y)...,
                                                              static_cast<X &&>(hana::at_c<n + 1>(storage_))...);
    }
};

constexpr make_reverse_partial_t reverse_partial{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

namespace infix_detail
{

template <bool left, bool right, typename F>
struct infix_t {
    F f;

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const &
    {
            return f(static_cast<X &&>(x)...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &
    {
            return f(static_cast<X &&>(x)...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &&
    {
            return std::move(f)(static_cast<X &&>(x)...);
    }
};

template <bool left, bool right>
struct make_infix {
    template <typename F>
    constexpr infix_t<left, right, typename detail::decay<F>::type> operator()(F &&f) const
    {
            return {static_cast<F &&>(f)};
    }
};

template <bool left, bool right>
struct Infix;
struct Object;

template <typename T>
struct dispatch {
    using type = Object;
};

template <bool left, bool right, typename F>
struct dispatch<infix_t<left, right, F>> {
    using type = Infix<left, right>;
};

template <typename, typename>
struct bind_infix;

template <>
struct bind_infix<Infix<false, false>, Object> {
    template <typename F, typename Y>
    static constexpr decltype(auto) apply(F &&f, Y &&y)
    {
            return make_infix<false, true>{}(hana::reverse_partial(static_cast<F &&>(f), static_cast<Y &&>(y)));
    }
};

template <>
struct bind_infix<Infix<true, false>, Object> {
    template <typename F, typename Y>
    static constexpr decltype(auto) apply(F &&f, Y &&y)
    {
            return static_cast<F &&>(f)(static_cast<Y &&>(y));
    }
};

template <>
struct bind_infix<Object, Infix<false, false>> {
    template <typename X, typename F>
    static constexpr decltype(auto) apply(X &&x, F &&f)
    {
            return make_infix<true, false>{}(hana::partial(static_cast<F &&>(f), static_cast<X &&>(x)));
    }
};

template <>
struct bind_infix<Object, Infix<false, true>> {
    template <typename X, typename F>
    static constexpr decltype(auto) apply(X &&x, F &&f)
    {
            return static_cast<F &&>(f)(static_cast<X &&>(x));
    }
};

template <typename T>
using strip = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template <typename X, typename Y>
constexpr decltype(auto) operator^(X &&x, Y &&y)
{
    return bind_infix<typename dispatch<strip<X>>::type, typename dispatch<strip<Y>>::type>::apply(
        static_cast<X &&>(x), static_cast<Y &&>(y));
}
} // namespace infix_detail

constexpr infix_detail::make_infix<false, false> infix{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct contains_impl : contains_impl<S, when<true>> {};

struct contains_t {
    template <typename Xs, typename Key>
    constexpr auto operator()(Xs &&xs, Key &&key) const;
};

constexpr auto contains = hana::infix(contains_t{});

constexpr auto in = hana::infix(hana::flip(hana::contains));
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct find_impl : find_impl<S, when<true>> {};

struct find_t {
    template <typename Xs, typename Key>
    constexpr auto operator()(Xs &&xs, Key const &key) const;
};

constexpr find_t find{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It, typename = void>
struct front_impl : front_impl<It, when<true>> {};

struct front_t {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const;
};

constexpr front_t front{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct hash_impl : hash_impl<T, when<true>> {};

struct hash_t {
    template <typename X>
    constexpr auto operator()(X const &x) const;
};

constexpr hash_t hash{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct zero_impl : zero_impl<M, when<true>> {};

template <typename M>
struct zero_t {
    constexpr decltype(auto) operator()() const;
};

template <typename M>
constexpr zero_t<M> zero{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename L>
struct Logical;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Cond, typename Then, typename Else>
constexpr decltype(auto) eval_if_t::operator()(Cond &&cond, Then &&then_, Else &&else_) const
{
    using Bool = typename hana::tag_of<Cond>::type;
    using EvalIf =
        ::std::conditional_t<(hana::Logical<Bool>::value), eval_if_impl<Bool>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Logical<Bool>::value, "hana::eval_if(cond, then, else) requires 'cond' to be a Logical");

    return EvalIf::apply(static_cast<Cond &&>(cond), static_cast<Then &&>(then_), static_cast<Else &&>(else_));
}

template <typename L, bool condition>
struct eval_if_impl<L, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename L>
struct eval_if_impl<L, when<std::is_arithmetic<L>::value>> {
    template <typename Cond, typename T, typename E>
    static constexpr auto apply(Cond const &cond, T &&t, E &&e)
    {
            return cond ? hana::eval(static_cast<T &&>(t)) : hana::eval(static_cast<E &&>(e));
    }
};

template <typename C>
struct eval_if_impl<C, when<hana::Constant<C>::value && Logical<typename C::value_type>::value>> {
    template <typename Then, typename Else>
    static constexpr decltype(auto) eval_if_helper(hana::true_, Then &&t, Else &&)
    {
            return hana::eval(static_cast<Then &&>(t));
    }

    template <typename Then, typename Else>
    static constexpr decltype(auto) eval_if_helper(hana::false_, Then &&, Else &&e)
    {
            return hana::eval(static_cast<Else &&>(e));
    }

    template <typename Cond, typename Then, typename Else>
    static constexpr decltype(auto) apply(Cond const &, Then &&t, Else &&e)
    {
            constexpr auto cond = hana::value<Cond>();
            constexpr bool truth_value = hana::if_(cond, true, false);
            return eval_if_helper(hana::bool_<truth_value>{}, static_cast<Then &&>(t), static_cast<Else &&>(e));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X>
constexpr decltype(auto) not_t::operator()(X &&x) const
{
    using Bool = typename hana::tag_of<X>::type;
    using Not =
        ::std::conditional_t<(hana::Logical<Bool>::value), hana::not_impl<Bool>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Logical<Bool>::value, "hana::not_(cond) requires 'cond' to be a Logical");

    return Not::apply(static_cast<X &&>(x));
}

template <typename L, bool condition>
struct not_impl<L, when<condition>> : hana::default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename L>
struct not_impl<L, hana::when<std::is_arithmetic<L>::value>> {
    template <typename Cond>
    static constexpr Cond apply(Cond const &cond)
    {
            return static_cast<Cond>(cond ? false : true);
    }
};

namespace detail
{
template <typename C, typename X>
struct constant_from_not {
    static constexpr auto value = hana::not_(hana::value<X>());
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct not_impl<C, hana::when<hana::Constant<C>::value && hana::Logical<typename C::value_type>::value>> {
    template <typename Cond>
    static constexpr auto apply(Cond const &)
    {
            return hana::to<C>(detail::constant_from_not<C, Cond>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename L, typename = void>
struct while_impl : while_impl<L, when<true>> {};

struct while_t {
    template <typename Pred, typename State, typename F>
    constexpr decltype(auto) operator()(Pred &&pred, State &&state, F &&f) const;
};

constexpr while_t while_{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Pred, typename State, typename F>
constexpr decltype(auto) while_t::operator()(Pred &&pred, State &&state, F &&f) const
{
    using Cond = decltype(pred(state));
    using Bool = typename hana::tag_of<Cond>::type;
    using While =
        ::std::conditional_t<(hana::Logical<Bool>::value), while_impl<Bool>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Logical<Bool>::value, "hana::while_(pred, state, f) requires 'pred(state)' to be a Logical");

    return While::apply(static_cast<Pred &&>(pred), static_cast<State &&>(state), static_cast<F &&>(f));
}

template <typename L, bool condition>
struct while_impl<L, hana::when<condition>> : hana::default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename L>
struct while_impl<L, hana::when<std::is_arithmetic<L>::value>> {
    template <typename Pred, typename State, typename F>
    static auto apply(Pred &&pred, State &&state, F &&f)
        -> decltype(true ? f(static_cast<State &&>(state)) : static_cast<State &&>(state))
    {
            if (pred(state)) {
                decltype(auto) r = f(static_cast<State &&>(state));
                return hana::while_(static_cast<Pred &&>(pred), static_cast<decltype(r) &&>(r), static_cast<F &&>(f));
            } else {
                return static_cast<State &&>(state);
            }
    }
};

template <typename C>
struct while_impl<C, hana::when<hana::Constant<C>::value && hana::Logical<typename C::value_type>::value>> {
    template <typename Pred, typename State, typename F>
    static constexpr State while_helper(hana::false_, Pred &&, State &&state, F &&)
    {
            return static_cast<State &&>(state);
    }

    template <typename Pred, typename State, typename F>
    static constexpr decltype(auto) while_helper(hana::true_, Pred &&pred, State &&state, F &&f)
    {
            decltype(auto) r = f(static_cast<State &&>(state));
            return hana::while_(static_cast<Pred &&>(pred), static_cast<decltype(r) &&>(r), static_cast<F &&>(f));
    }

    template <typename Pred, typename State, typename F>
    static constexpr decltype(auto) apply(Pred &&pred, State &&state, F &&f)
    {
            auto cond_ = pred(state);
            constexpr auto cond = hana::value(cond_);
            constexpr bool truth_value = hana::if_(cond, true, false);
            return while_helper(hana::bool_c<truth_value>, static_cast<Pred &&>(pred), static_cast<State &&>(state),
                                static_cast<F &&>(f));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename L>
struct Logical : hana::integral_constant<bool, !is_default<eval_if_impl<typename tag_of<L>::type>>::value
                                                   && !is_default<not_impl<typename tag_of<L>::type>>::value
                                                   && !is_default<while_impl<typename tag_of<L>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Cond, typename Then, typename Else>
constexpr decltype(auto) if_t::operator()(Cond &&cond, Then &&then_, Else &&else_) const
{
    using Bool = typename hana::tag_of<Cond>::type;
    using If = ::std::conditional_t<(hana::Logical<Bool>::value), if_impl<Bool>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Logical<Bool>::value, "hana::if_(cond, then, else) requires 'cond' to be a Logical");

    return If::apply(static_cast<Cond &&>(cond), static_cast<Then &&>(then_), static_cast<Else &&>(else_));
}

namespace detail
{
template <typename T>
struct hold {
    T value;
    constexpr T &&operator()() &&
    {
            return static_cast<T &&>(value);
    }
};
} // namespace detail

template <typename L, bool condition>
struct if_impl<L, when<condition>> : default_ {
    template <typename C, typename T, typename E>
    static constexpr auto apply(C &&c, T &&t, E &&e)
    {
            return hana::eval_if(static_cast<C &&>(c), detail::hold<T &&>{static_cast<T &&>(t)},
                                 detail::hold<E &&>{static_cast<E &&>(e)});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename... T>
struct optional;

struct optional_tag {};

constexpr auto make_optional = make<optional_tag>;

struct make_just_t {
    template <typename T>
    constexpr auto operator()(T &&) const;
};

constexpr make_just_t just{};

template <>
struct optional<> : detail::operators::adl<optional<>> {
    constexpr optional() = default;
    constexpr optional(optional const &) = default;
    constexpr optional(optional &&) = default;

    constexpr optional &operator=(optional const &) = default;
    constexpr optional &operator=(optional &&) = default;

    constexpr decltype(nullptr) operator->() const
    {
            return nullptr;
    }

    template <typename... dummy>
    constexpr auto value() const;

    template <typename... dummy>
    constexpr auto operator*() const;

    template <typename U>
    constexpr U &&value_or(U &&u) const;
};

constexpr optional<> nothing{};

struct maybe_t {
    template <typename Def, typename F, typename T>
    constexpr decltype(auto) operator()(Def &&, F &&f, optional<T> const &m) const
    {
            return static_cast<F &&>(f)(m.value_);
    }

    template <typename Def, typename F, typename T>
    constexpr decltype(auto) operator()(Def &&, F &&f, optional<T> &m) const
    {
            return static_cast<F &&>(f)(m.value_);
    }

    template <typename Def, typename F, typename T>
    constexpr decltype(auto) operator()(Def &&, F &&f, optional<T> &&m) const
    {
            return static_cast<F &&>(f)(static_cast<optional<T> &&>(m).value_);
    }

    template <typename Def, typename F>
    constexpr Def operator()(Def &&def, F &&, optional<> const &) const
    {
            return static_cast<Def &&>(def);
    }
};

constexpr maybe_t maybe{};

struct sfinae_t {
    template <typename F>
    constexpr decltype(auto) operator()(F &&f) const;
};

constexpr sfinae_t sfinae{};

struct is_just_t {
    template <typename... T>
    constexpr auto operator()(optional<T...> const &) const;
};

constexpr is_just_t is_just{};

struct is_nothing_t {
    template <typename... T>
    constexpr auto operator()(optional<T...> const &) const;
};

constexpr is_nothing_t is_nothing{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct chain_impl : chain_impl<M, when<true>> {};

struct chain_t {
    template <typename Xs, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, F &&f) const;
};

constexpr chain_t chain{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Tag>
struct monad_operators {
    static constexpr bool value = false;
};

namespace operators
{
template <typename Xs, typename F,
          typename = typename std::enable_if<detail::monad_operators<typename hana::tag_of<Xs>::type>::value>::type>
constexpr auto operator|(Xs &&xs, F &&f)
{
    return hana::chain(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}
} // namespace operators
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct any_of_impl : any_of_impl<S, when<true>> {};

struct any_of_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr any_of_t any_of{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename A, typename = void>
struct ap_impl : ap_impl<A, when<true>> {};

struct ap_t {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const;

    template <typename F, typename... Xs>
    constexpr decltype(auto) operator()(F &&f, Xs &&...xs) const;
};

constexpr ap_t ap{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct concat_impl : concat_impl<M, when<true>> {};

struct concat_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs &&xs, Ys &&ys) const;
};

constexpr concat_t concat{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct empty_impl : empty_impl<M, when<true>> {};

template <typename M>
struct empty_t {
    constexpr auto operator()() const;
};

template <typename M>
constexpr empty_t<M> empty{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct find_if_impl : find_if_impl<S, when<true>> {};

struct find_if_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr find_if_t find_if{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct flatten_impl : flatten_impl<M, when<true>> {};

struct flatten_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr flatten_t flatten{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename A, typename = void>
struct lift_impl : lift_impl<A, when<true>> {};

template <typename A>
struct lift_t {
    template <typename X>
    constexpr auto operator()(X &&x) const;
};

template <typename A>
constexpr lift_t<A> lift{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct basic_type;

template <typename T>
struct type_impl;

template <typename T>
using type = typename type_impl<T>::_;

struct type_tag {};

template <typename T>
constexpr type<T> type_c{};

struct decltype_t {
    template <typename T>
    constexpr auto operator()(T &&) const;
};

constexpr decltype_t decltype_{};

struct typeid_t {
    template <typename T>
    constexpr auto operator()(T &&) const;
};

constexpr typeid_t typeid_{};

constexpr auto make_type = hana::make<type_tag>;

struct sizeof_t {
    template <typename T>
    constexpr auto operator()(T &&) const;
};

constexpr sizeof_t sizeof_{};

struct alignof_t {
    template <typename T>
    constexpr auto operator()(T &&) const;
};

constexpr alignof_t alignof_{};

struct is_valid_t {
    template <typename F>
    constexpr auto operator()(F &&) const;

    template <typename F, typename... Args>
    constexpr auto operator()(F &&, Args &&...) const;
};

constexpr is_valid_t is_valid{};

template <template <typename...> class F>
struct template_t;

template <template <typename...> class F>
constexpr template_t<F> template_{};

template <template <typename...> class f>
struct metafunction_t;

template <template <typename...> class f>
constexpr metafunction_t<f> metafunction{};

template <typename F>
struct metafunction_class_t;

template <typename F>
constexpr metafunction_class_t<F> metafunction_class{};

template <typename F>
struct integral_t;

struct make_integral_t {
    template <typename F>
    constexpr integral_t<F> operator()(F const &) const
    {
            return {};
    }
};

constexpr make_integral_t integral{};

template <template <typename...> class F>
constexpr auto trait = hana::integral(hana::metafunction<F>);
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

namespace detail
{
template <typename T, typename = typename hana::tag_of<T>::type>
struct nested_type {};

template <typename T>
struct nested_type<T, type_tag> {
    using type = typename T::type;
};
} // namespace detail

template <typename T>
struct optional<T> : detail::operators::adl<>, detail::nested_type<T> {
    constexpr optional() = default;
    constexpr optional(optional const &) = default;
    constexpr optional(optional &&) = default;

    constexpr optional(T const &t) : value_(t) {}

    constexpr optional(T &&t) : value_(static_cast<T &&>(t)) {}

    constexpr optional &operator=(optional const &) = default;
    constexpr optional &operator=(optional &&) = default;

    constexpr T const *operator->() const
    {
            return &value_;
    }
    constexpr T *operator->()
    {
            return &value_;
    }

    constexpr T &value() &
    {
            return value_;
    }
    constexpr T const &value() const &
    {
            return value_;
    }
    constexpr T &&value() &&
    {
            return static_cast<T &&>(value_);
    }
    constexpr T const &&value() const &&
    {
            return static_cast<T const &&>(value_);
    }

    constexpr T &operator*() &
    {
            return value_;
    }
    constexpr T const &operator*() const &
    {
            return value_;
    }
    constexpr T &&operator*() &&
    {
            return static_cast<T &&>(value_);
    }
    constexpr T const &&operator*() const &&
    {
            return static_cast<T const &&>(value_);
    }

    template <typename U>
    constexpr T &value_or(U &&) &
    {
            return value_;
    }
    template <typename U>
    constexpr T const &value_or(U &&) const &
    {
            return value_;
    }
    template <typename U>
    constexpr T &&value_or(U &&) &&
    {
            return static_cast<T &&>(value_);
    }
    template <typename U>
    constexpr T const &&value_or(U &&) const &&
    {
            return static_cast<T const &&>(value_);
    }

    T value_;
};

template <typename... dummy>
constexpr auto optional<>::value() const
{
    static_assert(detail::wrong<dummy...>{}, "hana::optional::value() requires a non-empty optional");
}

template <typename... dummy>
constexpr auto optional<>::operator*() const
{
    static_assert(detail::wrong<dummy...>{}, "hana::optional::operator* requires a non-empty optional");
}

template <typename U>
constexpr U &&optional<>::value_or(U &&u) const
{
    return static_cast<U &&>(u);
}

template <typename T>
constexpr auto make_just_t::operator()(T &&t) const
{
    return hana::optional<typename detail::decay<T>::type>(static_cast<T &&>(t));
}

template <typename... T>
struct tag_of<optional<T...>> {
    using type = optional_tag;
};

template <>
struct make_impl<optional_tag> {
    template <typename X>
    static constexpr auto apply(X &&x)
    {
            return hana::just(static_cast<X &&>(x));
    }

    static constexpr auto apply()
    {
            return hana::nothing;
    }
};

namespace detail
{
template <>
struct comparable_operators<optional_tag> {
    static constexpr bool value = true;
};
template <>
struct orderable_operators<optional_tag> {
    static constexpr bool value = true;
};
template <>
struct monad_operators<optional_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <typename... T>
constexpr auto is_just_t::operator()(optional<T...> const &) const
{
    return hana::bool_c<sizeof...(T) != 0>;
}

template <typename... T>
constexpr auto is_nothing_t::operator()(optional<T...> const &) const
{
    return hana::bool_c<sizeof...(T) == 0>;
}

namespace detail
{
struct sfinae_impl {
    template <typename F, typename... X, typename = decltype(std::declval<F>()(std::declval<X>()...))>
    constexpr decltype(auto) operator()(int, F &&f, X &&...x) const
    {
            using Return = decltype(static_cast<F &&>(f)(static_cast<X &&>(x)...));
            static_assert(!std::is_same<Return, void>::value,
                          "hana::sfinae(f)(args...) requires f(args...) to be non-void");

            return hana::just(static_cast<F &&>(f)(static_cast<X &&>(x)...));
    }

    template <typename F, typename... X>
    constexpr auto operator()(long, F &&, X &&...) const
    {
            return hana::nothing;
    }
};
} // namespace detail

template <typename F>
constexpr decltype(auto) sfinae_t::operator()(F &&f) const
{
    return hana::partial(detail::sfinae_impl{}, int{}, static_cast<F &&>(f));
}

template <>
struct equal_impl<optional_tag, optional_tag> {
    template <typename T, typename U>
    static constexpr auto apply(hana::optional<T> const &t, hana::optional<U> const &u)
    {
            return hana::equal(t.value_, u.value_);
    }

    static constexpr hana::true_ apply(hana::optional<> const &, hana::optional<> const &)
    {
            return {};
    }

    template <typename T, typename U>
    static constexpr hana::false_ apply(T const &, U const &)
    {
            return {};
    }
};

template <>
struct less_impl<optional_tag, optional_tag> {
    template <typename T>
    static constexpr hana::true_ apply(hana::optional<> const &, hana::optional<T> const &)
    {
            return {};
    }

    static constexpr hana::false_ apply(hana::optional<> const &, hana::optional<> const &)
    {
            return {};
    }

    template <typename T>
    static constexpr hana::false_ apply(hana::optional<T> const &, hana::optional<> const &)
    {
            return {};
    }

    template <typename T, typename U>
    static constexpr auto apply(hana::optional<T> const &x, hana::optional<U> const &y)
    {
            return hana::less(x.value_, y.value_);
    }
};

template <>
struct transform_impl<optional_tag> {
    template <typename F>
    static constexpr auto apply(optional<> const &, F &&)
    {
            return hana::nothing;
    }

    template <typename T, typename F>
    static constexpr auto apply(optional<T> const &opt, F &&f)
    {
            return hana::just(static_cast<F &&>(f)(opt.value_));
    }

    template <typename T, typename F>
    static constexpr auto apply(optional<T> &opt, F &&f)
    {
            return hana::just(static_cast<F &&>(f)(opt.value_));
    }

    template <typename T, typename F>
    static constexpr auto apply(optional<T> &&opt, F &&f)
    {
            return hana::just(static_cast<F &&>(f)(static_cast<T &&>(opt.value_)));
    }
};

template <>
struct lift_impl<optional_tag> {
    template <typename X>
    static constexpr auto apply(X &&x)
    {
            return hana::just(static_cast<X &&>(x));
    }
};

template <>
struct ap_impl<optional_tag> {
    template <typename F, typename X>
    static constexpr auto ap_helper(F &&, X &&, ...)
    {
            return hana::nothing;
    }

    template <typename F, typename X>
    static constexpr auto ap_helper(F &&f, X &&x, hana::true_, hana::true_)
    {
            return hana::just(static_cast<F &&>(f).value_(static_cast<X &&>(x).value_));
    }

    template <typename F, typename X>
    static constexpr auto apply(F &&f, X &&x)
    {
            return ap_impl::ap_helper(static_cast<F &&>(f), static_cast<X &&>(x), hana::is_just(f), hana::is_just(x));
    }
};

template <>
struct flatten_impl<optional_tag> {
    static constexpr auto apply(optional<> const &)
    {
            return hana::nothing;
    }

    static constexpr auto apply(optional<optional<>> const &)
    {
            return hana::nothing;
    }

    template <typename T>
    static constexpr auto apply(optional<optional<T>> const &opt)
    {
            return hana::just(opt.value_.value_);
    }

    template <typename T>
    static constexpr auto apply(optional<optional<T>> &&opt)
    {
            return hana::just(static_cast<T &&>(opt.value_.value_));
    }
};

template <>
struct concat_impl<optional_tag> {
    template <typename Y>
    static constexpr auto apply(hana::optional<> &, Y &&y)
    {
            return static_cast<Y &&>(y);
    }

    template <typename Y>
    static constexpr auto apply(hana::optional<> &&, Y &&y)
    {
            return static_cast<Y &&>(y);
    }

    template <typename Y>
    static constexpr auto apply(hana::optional<> const &, Y &&y)
    {
            return static_cast<Y &&>(y);
    }

    template <typename X, typename Y>
    static constexpr auto apply(X &&x, Y &&)
    {
            return static_cast<X &&>(x);
    }
};

template <>
struct empty_impl<optional_tag> {
    static constexpr auto apply()
    {
            return hana::nothing;
    }
};

template <>
struct unpack_impl<optional_tag> {
    template <typename T, typename F>
    static constexpr decltype(auto) apply(optional<T> &&opt, F &&f)
    {
            return static_cast<F &&>(f)(static_cast<T &&>(opt.value_));
    }

    template <typename T, typename F>
    static constexpr decltype(auto) apply(optional<T> const &opt, F &&f)
    {
            return static_cast<F &&>(f)(opt.value_);
    }

    template <typename T, typename F>
    static constexpr decltype(auto) apply(optional<T> &opt, F &&f)
    {
            return static_cast<F &&>(f)(opt.value_);
    }

    template <typename F>
    static constexpr decltype(auto) apply(optional<> const &, F &&f)
    {
            return static_cast<F &&>(f)();
    }
};

namespace detail
{
template <bool>
struct optional_find_if {
    template <typename T>
    static constexpr auto apply(T const &)
    {
            return hana::nothing;
    }
};

template <>
struct optional_find_if<true> {
    template <typename T>
    static constexpr auto apply(T &&t)
    {
            return hana::just(static_cast<T &&>(t));
    }
};
} // namespace detail

template <>
struct find_if_impl<optional_tag> {
    template <typename T, typename Pred>
    static constexpr auto apply(hana::optional<T> const &opt, Pred &&pred)
    {
            constexpr bool found = decltype(static_cast<Pred &&>(pred)(opt.value_))::value;
            return detail::optional_find_if<found>::apply(opt.value_);
    }

    template <typename T, typename Pred>
    static constexpr auto apply(hana::optional<T> &opt, Pred &&pred)
    {
            constexpr bool found = decltype(static_cast<Pred &&>(pred)(opt.value_))::value;
            return detail::optional_find_if<found>::apply(opt.value_);
    }

    template <typename T, typename Pred>
    static constexpr auto apply(hana::optional<T> &&opt, Pred &&pred)
    {
            constexpr bool found = decltype(static_cast<Pred &&>(pred)(static_cast<T &&>(opt.value_)))::value;
            return detail::optional_find_if<found>::apply(static_cast<T &&>(opt.value_));
    }

    template <typename Pred>
    static constexpr auto apply(hana::optional<> const &, Pred &&)
    {
            return hana::nothing;
    }
};

template <>
struct any_of_impl<optional_tag> {
    template <typename T, typename Pred>
    static constexpr auto apply(hana::optional<T> const &opt, Pred &&pred)
    {
            return static_cast<Pred &&>(pred)(opt.value_);
    }

    template <typename Pred>
    static constexpr hana::false_ apply(hana::optional<> const &, Pred &&)
    {
            return {};
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F>
struct Metafunction;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct basic_type : detail::operators::adl<basic_type<T>> {
    using hana_tag = type_tag;

    using type = T;
    constexpr auto operator+() const
    {
            return *this;
    }
};

template <typename T>
struct type_impl {
    struct _ : basic_type<T> {};
};

namespace detail
{
template <typename T, typename = type_tag>
struct decltype_t {
    using type = typename std::remove_reference<T>::type;
};

template <typename T>
struct decltype_t<T, typename hana::tag_of<T>::type> {
    using type = typename std::remove_reference<T>::type::type;
};
} // namespace detail

template <typename T>
constexpr auto decltype_t::operator()(T &&) const
{
    return hana::type_c<typename detail::decltype_t<T>::type>;
}

namespace detail
{
template <typename T, typename = type_tag>
struct typeid_t {
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template <typename T>
struct typeid_t<T, typename hana::tag_of<T>::type> {
    using type = typename std::remove_reference<T>::type::type;
};
} // namespace detail

template <typename T>
constexpr auto typeid_t::operator()(T &&) const
{
    return hana::type_c<typename detail::typeid_t<T>::type>;
}

template <>
struct make_impl<type_tag> {
    template <typename T>
    static constexpr auto apply(T &&t)
    {
            return hana::typeid_(static_cast<T &&>(t));
    }
};

template <typename T>
constexpr auto sizeof_t::operator()(T &&) const
{
    return hana::size_c<sizeof(typename detail::decltype_t<T>::type)>;
}

template <typename T>
constexpr auto alignof_t::operator()(T &&) const
{
    return hana::size_c<alignof(typename detail::decltype_t<T>::type)>;
}

namespace type_detail
{
template <typename F, typename... Args, typename = decltype(std::declval<F &&>()(std::declval<Args &&>()...))>
constexpr auto is_valid_impl(int)
{
    return hana::true_c;
}

template <typename F, typename... Args>
constexpr auto is_valid_impl(...)
{
    return hana::false_c;
}

template <typename F>
struct is_valid_fun {
    template <typename... Args>
    constexpr auto operator()(Args &&...) const
    {
            return is_valid_impl<F, Args &&...>(int{});
    }
};
} // namespace type_detail

template <typename F>
constexpr auto is_valid_t::operator()(F &&) const
{
    return type_detail::is_valid_fun<F &&>{};
}

template <typename F, typename... Args>
constexpr auto is_valid_t::operator()(F &&, Args &&...) const
{
    return type_detail::is_valid_impl<F &&, Args &&...>(int{});
}

namespace template_detail
{
template <typename... T>
struct args;
template <typename...>
using always_void = void;

template <template <typename...> class F, typename Args, typename = void>
struct specialization_is_valid : std::false_type {};

template <template <typename...> class F, typename... T>
struct specialization_is_valid<F, args<T...>, always_void<F<T...>>> : std::true_type {};
} // namespace template_detail

template <template <typename...> class F>
struct template_t {
    template <typename... T>
    struct apply {
            using type = F<T...>;
    };

    template <typename... T, typename = std::enable_if_t<template_detail::specialization_is_valid<
                                 F, template_detail::args<typename T::type...>>::value>>
    constexpr auto operator()(T const &...) const
    {
            return hana::type<F<typename T::type...>>{};
    }
};

template <template <typename...> class F>
struct metafunction_t {
    template <typename... T>
    using apply = F<T...>;

    template <typename... T>
    constexpr hana::type<typename F<typename T::type...>::type> operator()(T const &...) const
    {
            return {};
    }
};

namespace detail
{
template <typename F, typename... T>
struct always_first {
    using type = F;
};
} // namespace detail
template <typename F>
struct metafunction_class_t {
    template <typename... T>
    using apply = typename detail::always_first<F, T...>::type::template apply<T...>;

    template <typename... T>
    constexpr hana::type<typename detail::always_first<F, T...>::type::template apply<typename T::type...>::type>
    operator()(T const &...) const
    {
            return {};
    }
};

template <template <typename...> class F>
struct Metafunction<template_t<F>> {
    static constexpr bool value = true;
};

template <template <typename...> class F>
struct Metafunction<metafunction_t<F>> {
    static constexpr bool value = true;
};

template <typename F>
struct Metafunction<metafunction_class_t<F>> {
    static constexpr bool value = true;
};

template <typename F>
struct integral_t {
    template <typename... T,
              typename Result = typename detail::always_first<F, T...>::type::template apply<typename T::type...>::type>
    constexpr Result operator()(T const &...) const
    {
            return Result{};
    }
};

namespace detail
{
template <>
struct comparable_operators<type_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <>
struct equal_impl<type_tag, type_tag> {
    template <typename T, typename U>
    static constexpr auto apply(basic_type<T> const &, basic_type<U> const &)
    {
            return hana::false_c;
    }

    template <typename T>
    static constexpr auto apply(basic_type<T> const &, basic_type<T> const &)
    {
            return hana::true_c;
    }
};

template <>
struct hash_impl<hana::type_tag> {
    template <typename T>
    static constexpr T apply(T const &t)
    {
            return t;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

namespace detail
{
template <char... s>
constexpr char const string_storage[sizeof...(s) + 1] = {s..., '\0'};
}

template <char... s>
struct string : detail::operators::adl<string<s...>>, detail::iterable_operators<string<s...>> {
    static constexpr char const *c_str()
    {
            return &detail::string_storage<s...>[0];
    }
};

template <char... s>
struct tag_of<string<s...>> {
    using type = string_tag;
};

template <>
struct make_impl<string_tag> {
    template <typename... Chars>
    static constexpr auto apply(Chars const &...)
    {
            return hana::string<hana::value<Chars>()...>{};
    }
};

namespace string_detail
{
template <typename S, std::size_t... N>
constexpr string<S::get()[N]...> prepare_impl(S, std::index_sequence<N...>)
{
    return {};
}

template <typename S>
constexpr decltype(auto) prepare(S s)
{
    return prepare_impl(s, std::make_index_sequence<sizeof(S::get()) - 1>{});
}
} // namespace string_detail

namespace detail
{
template <>
struct comparable_operators<string_tag> {
    static constexpr bool value = true;
};
template <>
struct orderable_operators<string_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <>
struct to_impl<char const *, string_tag> {
    template <char... c>
    static constexpr char const *apply(string<c...> const &)
    {
            return string<c...>::c_str();
    }
};

namespace detail
{
constexpr std::size_t cx_strlen(char const *s)
{
    std::size_t n = 0u;
    while (*s != '\0') ++s, ++n;
    return n;
}

template <typename S, std::size_t... I>
constexpr hana::string<hana::value<S>()[I]...> expand(std::index_sequence<I...>)
{
    return {};
}
} // namespace detail

template <typename IC>
struct to_impl<
    hana::string_tag, IC,
    hana::when<hana::Constant<IC>::value && std::is_convertible<typename IC::value_type, char const *>::value>> {
    template <typename S>
    static constexpr auto apply(S const &)
    {
            constexpr char const *s = hana::value<S>();
            constexpr std::size_t len = detail::cx_strlen(s);
            return detail::expand<S>(std::make_index_sequence<len>{});
    }
};

template <>
struct equal_impl<string_tag, string_tag> {
    template <typename S>
    static constexpr auto apply(S const &, S const &)
    {
            return hana::true_c;
    }

    template <typename S1, typename S2>
    static constexpr auto apply(S1 const &, S2 const &)
    {
            return hana::false_c;
    }
};

template <>
struct less_impl<string_tag, string_tag> {
    template <char... s1, char... s2>
    static constexpr auto apply(string<s1...> const &, string<s2...> const &)
    {
            constexpr char const c_str1[] = {s1..., '\0'};
            constexpr char const c_str2[] = {s2..., '\0'};
            return hana::bool_c<detail::lexicographical_compare(c_str1, c_str1 + sizeof...(s1), c_str2,
                                                                c_str2 + sizeof...(s2))>;
    }
};

template <>
struct plus_impl<string_tag, string_tag> {
    template <char... s1, char... s2>
    static constexpr auto apply(string<s1...> const &, string<s2...> const &)
    {
            return string<s1..., s2...>{};
    }
};

template <>
struct zero_impl<string_tag> {
    static constexpr auto apply()
    {
            return string<>{};
    }
};

template <char... s1, char... s2>
constexpr auto operator+(string<s1...> const &, string<s2...> const &)
{
    return hana::string<s1..., s2...>{};
}

template <>
struct unpack_impl<string_tag> {
    template <char... s, typename F>
    static constexpr decltype(auto) apply(string<s...> const &, F &&f)
    {
            return static_cast<F &&>(f)(char_<s>{}...);
    }
};

template <>
struct length_impl<string_tag> {
    template <char... s>
    static constexpr auto apply(string<s...> const &)
    {
            return hana::size_c<sizeof...(s)>;
    }
};

template <>
struct front_impl<string_tag> {
    template <char x, char... xs>
    static constexpr auto apply(string<x, xs...> const &)
    {
            return hana::char_c<x>;
    }
};

template <>
struct drop_front_impl<string_tag> {
    template <std::size_t N, char... xs, std::size_t... i>
    static constexpr auto helper(string<xs...> const &, std::index_sequence<i...>)
    {
            constexpr char s[] = {xs...};
            return hana::string_c<s[i + N]...>;
    }

    template <char... xs, typename N>
    static constexpr auto apply(string<xs...> const &s, N const &)
    {
            return helper<N::value>(
                s, std::make_index_sequence < (N::value < sizeof...(xs)) ? sizeof...(xs) - N::value : 0 > {});
    }

    template <typename N>
    static constexpr auto apply(string<> const &s, N const &)
    {
            return s;
    }
};

template <>
struct is_empty_impl<string_tag> {
    template <char... s>
    static constexpr auto apply(string<s...> const &)
    {
            return hana::bool_c<sizeof...(s) == 0>;
    }
};

template <>
struct at_impl<string_tag> {
    template <char... s, typename N>
    static constexpr auto apply(string<s...> const &, N const &)
    {
            constexpr char characters[] = {s..., '\0'};
            constexpr auto n = N::value;
            return hana::char_c<characters[n]>;
    }
};

template <>
struct contains_impl<string_tag> {
    template <char... s, typename C>
    static constexpr auto helper(string<s...> const &, C const &, hana::true_)
    {
            constexpr char const characters[] = {s..., '\0'};
            constexpr char c = hana::value<C>();
            return hana::bool_c<detail::find(characters, characters + sizeof...(s), c) != characters + sizeof...(s)>;
    }

    template <typename S, typename C>
    static constexpr auto helper(S const &, C const &, hana::false_)
    {
            return hana::false_c;
    }

    template <typename S, typename C>
    static constexpr auto apply(S const &s, C const &c)
    {
            return helper(s, c, hana::bool_c<hana::Constant<C>::value>);
    }
};

template <>
struct find_impl<string_tag> {
    template <char... s, typename Char>
    static constexpr auto apply(string<s...> const &str, Char const &c)
    {
            return hana::if_(contains_impl<string_tag>::apply(str, c), hana::just(c), hana::nothing);
    }
};

template <>
struct hash_impl<string_tag> {
    template <typename String>
    static constexpr auto apply(String const &)
    {
            return hana::type_c<String>;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename... Xn>
struct tuple;

struct tuple_tag {};

constexpr auto make_tuple = make<tuple_tag>;

constexpr auto to_tuple = to<tuple_tag>;

template <typename... T>
constexpr hana::tuple<hana::type<T>...> tuple_t{};

template <typename T, T... v>
constexpr hana::tuple<hana::integral_constant<T, v>...> tuple_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <bool... b>
struct fast_and : std::is_same<fast_and<b...>, fast_and<(b, true)...>> {};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <std::size_t i, std::size_t N, bool Done>
struct index_if_helper;

template <std::size_t i, std::size_t N>
struct index_if_helper<i, N, false> {
    template <typename Pred, typename X1, typename... Xs>
    using f =
        typename index_if_helper<i + 1, N,
                                 static_cast<bool>(detail::decay<decltype(std::declval<Pred>()(
                                                       std::declval<X1>()))>::type::value)>::template f<Pred, Xs...>;
};

template <std::size_t N>
struct index_if_helper<N, N, false> {
    template <typename...>
    using f = hana::optional<>;
};

template <std::size_t i, std::size_t N>
struct index_if_helper<i, N, true> {
    template <typename...>
    using f = hana::optional<hana::size_t<i - 1>>;
};

template <typename Pred, typename... Xs>
struct index_if {
    using type = typename index_if_helper<0, sizeof...(Xs), false>::template f<Pred, Xs...>;
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct index_if_impl : index_if_impl<S, when<true>> {};

struct index_if_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr index_if_t index_if{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Xs, typename Ys, std::size_t... n>
constexpr void assign(Xs &xs, Ys &&ys, std::index_sequence<n...>)
{
    int sequence[] = {int{}, ((void)(hana::at_c<n>(xs) = hana::at_c<n>(static_cast<Ys &&>(ys))), int{})...};
    (void)sequence;
}

struct from_index_sequence_t {};

template <typename Tuple, typename... Yn>
struct is_same_tuple : std::false_type {};

template <typename Tuple>
struct is_same_tuple<typename detail::decay<Tuple>::type, Tuple> : std::true_type {};

template <bool SameTuple, bool SameNumberOfElements, typename Tuple, typename... Yn>
struct enable_tuple_variadic_ctor;

template <typename... Xn, typename... Yn>
struct enable_tuple_variadic_ctor<false, true, hana::tuple<Xn...>, Yn...>
    : std::enable_if<detail::fast_and<::std::is_constructible<Xn, Yn &&>::value...>::value> {};
} // namespace detail

template <>

struct tuple<> final

    : detail::operators::adl<tuple<>>,
      detail::iterable_operators<tuple<>> {
    constexpr tuple() {}
    using hana_tag = tuple_tag;
};

template <typename... Xn>

struct tuple final

    : detail::operators::adl<tuple<Xn...>>,
      detail::iterable_operators<tuple<Xn...>> {
    basic_tuple<Xn...> storage_;
    using hana_tag = tuple_tag;

  private:
    template <typename Other, std::size_t... n>
    explicit constexpr tuple(detail::from_index_sequence_t, std::index_sequence<n...>, Other &&other)
        : storage_(hana::at_c<n>(static_cast<Other &&>(other))...)
    {
    }

  public:
    template <typename... dummy, typename = typename std::enable_if<
                                     detail::fast_and<::std::is_constructible<Xn, dummy...>::value...>::value>::type>
    constexpr tuple() : storage_()
    {
    }

    template <typename... dummy, typename = typename std::enable_if<detail::fast_and<
                                     ::std::is_constructible<Xn, Xn const &, dummy...>::value...>::value>::type>
    constexpr tuple(Xn const &...xn) : storage_(xn...)
    {
    }

    template <typename... Yn,
              typename = typename detail::enable_tuple_variadic_ctor<
                  detail::is_same_tuple<tuple, Yn...>::value, sizeof...(Xn) == sizeof...(Yn), tuple, Yn...>::type>
    constexpr tuple(Yn &&...yn) : storage_(static_cast<Yn &&>(yn)...)
    {
    }

    template <typename... Yn, typename = typename std::enable_if<
                                  detail::fast_and<::std::is_constructible<Xn, Yn const &>::value...>::value>::type>
    constexpr tuple(tuple<Yn...> const &other)
        : tuple(detail::from_index_sequence_t{}, std::make_index_sequence<sizeof...(Xn)>{}, other.storage_)
    {
    }

    template <typename... Yn, typename = typename std::enable_if<
                                  detail::fast_and<::std::is_constructible<Xn, Yn &&>::value...>::value>::type>
    constexpr tuple(tuple<Yn...> &&other)
        : tuple(detail::from_index_sequence_t{}, std::make_index_sequence<sizeof...(Xn)>{},
                static_cast<tuple<Yn...> &&>(other).storage_)
    {
    }

    template <typename... dummy, typename = typename std::enable_if<detail::fast_and<
                                     ::std::is_constructible<Xn, Xn const &, dummy...>::value...>::value>::type>
    constexpr tuple(tuple const &other)
        : tuple(detail::from_index_sequence_t{}, std::make_index_sequence<sizeof...(Xn)>{}, other.storage_)
    {
    }

    template <typename... dummy, typename = typename std::enable_if<detail::fast_and<
                                     ::std::is_constructible<Xn, Xn const &, dummy...>::value...>::value>::type>
    constexpr tuple(tuple &other) : tuple(const_cast<tuple const &>(other))
    {
    }

    template <typename... dummy, typename = typename std::enable_if<detail::fast_and<
                                     ::std::is_constructible<Xn, Xn &&, dummy...>::value...>::value>::type>
    constexpr tuple(tuple &&other)
        : tuple(detail::from_index_sequence_t{}, std::make_index_sequence<sizeof...(Xn)>{},
                static_cast<tuple &&>(other).storage_)
    {
    }

    template <typename... Yn, typename = typename std::enable_if<
                                  detail::fast_and<::std::is_assignable<Xn &, Yn const &>::value...>::value>::type>
    constexpr tuple &operator=(tuple<Yn...> const &other)
    {
            detail::assign(this->storage_, other.storage_, std::make_index_sequence<sizeof...(Xn)>{});
            return *this;
    }

    template <typename... Yn, typename = typename std::enable_if<
                                  detail::fast_and<::std::is_assignable<Xn &, Yn &&>::value...>::value>::type>
    constexpr tuple &operator=(tuple<Yn...> &&other)
    {
            detail::assign(this->storage_, static_cast<tuple<Yn...> &&>(other).storage_,
                           std::make_index_sequence<sizeof...(Xn)>{});
            return *this;
    }
};

namespace detail
{
template <>
struct comparable_operators<tuple_tag> {
    static constexpr bool value = true;
};
template <>
struct orderable_operators<tuple_tag> {
    static constexpr bool value = true;
};
template <>
struct monad_operators<tuple_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <>
struct unpack_impl<tuple_tag> {
    template <typename F>
    static constexpr decltype(auto) apply(tuple<> &&, F &&f)
    {
            return static_cast<F &&>(f)();
    }
    template <typename F>
    static constexpr decltype(auto) apply(tuple<> &, F &&f)
    {
            return static_cast<F &&>(f)();
    }
    template <typename F>
    static constexpr decltype(auto) apply(tuple<> const &, F &&f)
    {
            return static_cast<F &&>(f)();
    }

    template <typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            return hana::unpack(static_cast<Xs &&>(xs).storage_, static_cast<F &&>(f));
    }
};

template <>
struct length_impl<tuple_tag> {
    template <typename... Xs>
    static constexpr auto apply(tuple<Xs...> const &)
    {
            return hana::size_c<sizeof...(Xs)>;
    }
};

template <>
struct at_impl<tuple_tag> {
    template <typename Xs, typename N>
    static constexpr decltype(auto) apply(Xs &&xs, N const &)
    {
            constexpr std::size_t index = N::value;
            return hana::at_c<index>(static_cast<Xs &&>(xs).storage_);
    }
};

template <>
struct drop_front_impl<tuple_tag> {
    template <std::size_t N, typename Xs, std::size_t... i>
    static constexpr auto helper(Xs &&xs, std::index_sequence<i...>)
    {
            return hana::make<tuple_tag>(hana::at_c<i + N>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &)
    {
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            return helper<N::value>(static_cast<Xs &&>(xs),
                                    std::make_index_sequence < (N::value < len) ? len - N::value : 0 > {});
    }
};

template <>
struct is_empty_impl<tuple_tag> {
    template <typename... Xs>
    static constexpr auto apply(tuple<Xs...> const &)
    {
            return hana::bool_c<sizeof...(Xs) == 0>;
    }
};

template <std::size_t n, typename... Xs>
constexpr decltype(auto) at_c(tuple<Xs...> const &xs)
{
    return hana::at_c<n>(xs.storage_);
}

template <std::size_t n, typename... Xs>
constexpr decltype(auto) at_c(tuple<Xs...> &xs)
{
    return hana::at_c<n>(xs.storage_);
}

template <std::size_t n, typename... Xs>
constexpr decltype(auto) at_c(tuple<Xs...> &&xs)
{
    return hana::at_c<n>(static_cast<tuple<Xs...> &&>(xs).storage_);
}

template <>
struct index_if_impl<tuple_tag> {
    template <typename... Xs, typename Pred>
    static constexpr auto apply(tuple<Xs...> const &, Pred const &) -> typename detail::index_if<Pred, Xs...>::type
    {
            return {};
    }
};

template <>
struct Sequence<tuple_tag> {
    static constexpr bool value = true;
};

template <>
struct make_impl<tuple_tag> {
    template <typename... Xs>
    static constexpr tuple<typename detail::decay<Xs>::type...> apply(Xs &&...xs)
    {
            return {static_cast<Xs &&>(xs)...};
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace struct_detail
{
template <typename Memptr, Memptr ptr>
struct member_ptr {
    template <typename T>
    constexpr decltype(auto) operator()(T &&t) const
    {
            return static_cast<T &&>(t).*ptr;
    }
};

constexpr std::size_t strlen(char const *s)
{
    std::size_t n = 0;
    while (*s++ != '\0') ++n;
    return n;
}

template <std::size_t n, typename Names, std::size_t... i>
constexpr auto prepare_member_name_impl(std::index_sequence<i...>)
{
    return hana::string_c<hana::at_c<n>(Names::get())[i]...>;
}

template <std::size_t n, typename Names>
constexpr auto prepare_member_name()
{
    constexpr std::size_t len = strlen(hana::at_c<n>(Names::get()));
    return prepare_member_name_impl<n, Names>(std::make_index_sequence<len>{});
}
} // namespace struct_detail
} // namespace hana
} // namespace boost

template <typename...>
struct BOOST_HANA_ADAPT_STRUCT_must_be_called_in_the_global_namespace;

template <typename...>
struct BOOST_HANA_ADAPT_ADT_must_be_called_in_the_global_namespace;

namespace boost
{
namespace hana
{

}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename = void>
struct adjust_impl : adjust_impl<Xs, when<true>> {};

struct adjust_t {
    template <typename Xs, typename Value, typename F>
    constexpr auto operator()(Xs &&xs, Value &&value, F &&f) const;
};

constexpr adjust_t adjust{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename = void>
struct adjust_if_impl : adjust_if_impl<Xs, when<true>> {};

struct adjust_if_t {
    template <typename Xs, typename Pred, typename F>
    constexpr auto operator()(Xs &&xs, Pred const &pred, F const &f) const;
};

constexpr adjust_if_t adjust_if{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F>
struct Functor;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct _always {
    T val_;

    template <typename... Args>
    constexpr T const &operator()(Args const &...) const &
    {
            return val_;
    }

    template <typename... Args>
    constexpr T &operator()(Args const &...) &
    {
            return val_;
    }

    template <typename... Args>
    constexpr T operator()(Args const &...) &&
    {
            return std::move(val_);
    }
};

constexpr detail::create<_always> always{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename F>
constexpr auto transform_t::operator()(Xs &&xs, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Transform =
        ::std::conditional_t<(hana::Functor<S>::value), transform_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Functor<S>::value, "hana::transform(xs, f) requires 'xs' to be a Functor");

    return Transform::apply(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

template <typename Fun, bool condition>
struct transform_impl<Fun, when<condition>> : default_ {
    template <typename Xs, typename F>
    static constexpr auto apply(Xs &&xs, F &&f)
    {
            return hana::adjust_if(static_cast<Xs &&>(xs), hana::always(hana::true_c), static_cast<F &&>(f));
    }
};

template <typename S>
struct transform_impl<S, when<Sequence<S>::value>> {
    template <typename F>
    struct transformer {
            F f;
            template <typename... Xs>
            constexpr auto operator()(Xs &&...xs) const
            {
                return hana::make<S>((*f)(static_cast<Xs &&>(xs))...);
            }
    };

    template <typename Xs, typename F>
    static constexpr auto apply(Xs &&xs, F &&f)
    {
            return hana::unpack(static_cast<Xs &&>(xs), transformer<decltype(&f)>{&f});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename F>
struct Functor : hana::integral_constant<bool, !is_default<transform_impl<typename tag_of<F>::type>>::value
                                                   || !is_default<adjust_if_impl<typename tag_of<F>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred, typename F>
constexpr auto adjust_if_t::operator()(Xs &&xs, Pred const &pred, F const &f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using AdjustIf =
        ::std::conditional_t<(hana::Functor<S>::value), adjust_if_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Functor<S>::value, "hana::adjust_if(xs, pred, f) requires 'xs' to be a Functor");

    return AdjustIf::apply(static_cast<Xs &&>(xs), pred, f);
}

namespace detail
{
template <typename Pred, typename F>
struct apply_if {
    Pred const &pred;
    F const &f;

    template <typename X>
    constexpr decltype(auto) helper(bool cond, X &&x) const
    {
            return cond ? f(static_cast<X &&>(x)) : static_cast<X &&>(x);
    }

    template <typename X>
    constexpr decltype(auto) helper(hana::true_, X &&x) const
    {
            return f(static_cast<X &&>(x));
    }

    template <typename X>
    constexpr decltype(auto) helper(hana::false_, X &&x) const
    {
            return static_cast<X &&>(x);
    }

    template <typename X>
    constexpr decltype(auto) operator()(X &&x) const
    {
            auto cond = hana::if_(pred(x), hana::true_c, hana::false_c);
            return this->helper(cond, static_cast<X &&>(x));
    }
};
} // namespace detail

template <typename Fun, bool condition>
struct adjust_if_impl<Fun, when<condition>> : default_ {
    template <typename Xs, typename Pred, typename F>
    static constexpr auto apply(Xs &&xs, Pred const &pred, F const &f)
    {
            return hana::transform(static_cast<Xs &&>(xs), detail::apply_if<Pred, F>{pred, f});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct all_of_impl : all_of_impl<S, when<true>> {};

struct all_of_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr all_of_t all_of{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S>
struct Searchable;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F, typename G>
struct _compose {
    F f;
    G g;

    template <typename X, typename... Xs>
    constexpr decltype(auto) operator()(X &&x, Xs &&...xs) const &
    {
            return f(g(static_cast<X &&>(x)), static_cast<Xs &&>(xs)...);
    }

    template <typename X, typename... Xs>
    constexpr decltype(auto) operator()(X &&x, Xs &&...xs) &
    {
            return f(g(static_cast<X &&>(x)), static_cast<Xs &&>(xs)...);
    }

    template <typename X, typename... Xs>
    constexpr decltype(auto) operator()(X &&x, Xs &&...xs) &&
    {
            return std::move(f)(std::move(g)(static_cast<X &&>(x)), static_cast<Xs &&>(xs)...);
    }
};

struct _make_compose {
    template <typename F, typename G, typename... H>
    constexpr decltype(auto) operator()(F &&f, G &&g, H &&...h) const
    {
            return detail::variadic::foldl1(detail::create<_compose>{}, static_cast<F &&>(f), static_cast<G &&>(g),
                                            static_cast<H &&>(h)...);
    }
};

constexpr _make_compose compose{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto index_if_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using IndexIf =
        ::std::conditional_t<(hana::Iterable<S>::value), index_if_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<S>::value, "hana::index_if(xs, pred) requires 'xs' to be a Iterable");

    return IndexIf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

namespace detail
{
template <std::size_t i, std::size_t N, bool Done>
struct iterate_while;

template <std::size_t i, std::size_t N>
struct iterate_while<i, N, false> {
    template <typename Xs, typename Pred>
    using f =
        typename iterate_while<i + 1, N,
                               static_cast<bool>(
                                   detail::decay<decltype(std::declval<Pred>()(hana::at(
                                       std::declval<Xs>(), hana::size_c<i>)))>::type::value)>::template f<Xs, Pred>;
};

template <std::size_t N>
struct iterate_while<N, N, false> {
    template <typename Xs, typename Pred>
    using f = hana::optional<>;
};

template <std::size_t i, std::size_t N>
struct iterate_while<i, N, true> {
    template <typename Xs, typename Pred>
    using f = hana::optional<hana::size_t<i - 1>>;
};
} // namespace detail

template <typename Tag>
struct index_if_impl<Tag, when<Foldable<Tag>::value>> {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs const &xs, Pred const &) ->
        typename detail::iterate_while<0, decltype(hana::length(xs))::value, false>::template f<Xs, Pred>
    {
            return {};
    }
};

template <typename It>
struct index_if_impl<It, when<!Foldable<It>::value>> {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs const &, Pred const &) ->
        typename detail::iterate_while<0, static_cast<std::size_t>(-1), false>::template f<Xs, Pred>
    {
            return {};
    }
};

template <>
struct index_if_impl<basic_tuple_tag> {
    template <typename... Xs, typename Pred>
    static constexpr auto apply(basic_tuple<Xs...> const &, Pred const &) ->
        typename detail::index_if<Pred, Xs...>::type
    {
            return {};
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto find_if_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using FindIf =
        ::std::conditional_t<(hana::Searchable<S>::value), find_if_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::find_if(xs, pred) requires 'xs' to be a Searchable");

    return FindIf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

template <typename S, bool condition>
struct find_if_impl<S, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

namespace detail
{
template <typename Xs>
struct partial_at {
    Xs const &xs;

    template <typename I>
    constexpr decltype(auto) operator()(I i) const
    {
            return hana::at(xs, i);
    }
};
} // namespace detail

template <typename Tag>
struct find_if_impl<Tag, when<Iterable<Tag>::value>> {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            using Result = decltype(hana::index_if(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred)));

            return hana::transform(Result{}, detail::partial_at<std::decay_t<Xs>>{static_cast<Xs &&>(xs)});
    }
};

template <typename T, std::size_t N>
struct find_if_impl<T[N]> {
    template <typename Xs>
    static constexpr auto find_if_helper(Xs &&, hana::false_)
    {
            return hana::nothing;
    }

    template <typename Xs>
    static constexpr auto find_if_helper(Xs &&xs, hana::true_)
    {
            return hana::just(static_cast<Xs &&>(xs)[0]);
    }

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            return find_if_helper(static_cast<Xs &&>(xs),
                                  hana::bool_c<decltype(static_cast<Pred &&>(pred)(static_cast<Xs &&>(xs)[0]))::value>);
    }
};

namespace struct_detail
{
template <typename X>
struct get_member {
    X x;
    template <typename Member>
    constexpr decltype(auto) operator()(Member &&member) &&
    {
            return hana::second(static_cast<Member &&>(member))(static_cast<X &&>(x));
    }
};
} // namespace struct_detail

template <typename S>
struct find_if_impl<S, when<hana::Struct<S>::value>> {
    template <typename X, typename Pred>
    static constexpr decltype(auto) apply(X &&x, Pred &&pred)
    {
            return hana::transform(
                hana::find_if(hana::accessors<S>(), hana::compose(static_cast<Pred &&>(pred), hana::first)),
                struct_detail::get_member<X>{static_cast<X &&>(x)});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename S>
struct Searchable : hana::integral_constant<bool, !is_default<any_of_impl<typename tag_of<S>::type>>::value
                                                      && !is_default<find_if_impl<typename tag_of<S>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr decltype(auto) front_t::operator()(Xs &&xs) const
{
    using It = typename hana::tag_of<Xs>::type;
    using Front =
        ::std::conditional_t<(hana::Iterable<It>::value), front_impl<It>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It>::value, "hana::front(xs) requires 'xs' to be an Iterable");

    return Front::apply(static_cast<Xs &&>(xs));
}

template <typename It, bool condition>
struct front_impl<It, when<condition>> : default_ {
    template <typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::at_c<0>(static_cast<Xs &&>(xs));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto any_of_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using AnyOf =
        ::std::conditional_t<(hana::Searchable<S>::value), any_of_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::any_of(xs, pred) requires 'xs' to be a Searchable");

    return AnyOf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

template <typename S, bool condition>
struct any_of_impl<S, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename S>
struct any_of_impl<S, when<Sequence<S>::value>> {
    template <std::size_t k, std::size_t Len>
    struct any_of_helper {
            template <typename Xs, typename Pred>
            static constexpr auto apply(bool prev_cond, Xs &&xs, Pred &&pred)
            {
                return prev_cond ? hana::true_c
                                 : any_of_impl::any_of_helper<k + 1, Len>::apply(
                                     hana::if_(pred(hana::at_c<k>(xs)), hana::true_c, hana::false_c),
                                     static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
            }

            template <typename Xs, typename Pred>
            static constexpr auto apply(hana::true_, Xs &&, Pred &&)
            {
                return hana::true_c;
            }

            template <typename Xs, typename Pred>
            static constexpr auto apply(hana::false_, Xs &&xs, Pred &&pred)
            {
                auto cond = hana::if_(pred(hana::at_c<k>(xs)), hana::true_c, hana::false_c);
                return any_of_impl::any_of_helper<k + 1, Len>::apply(cond, static_cast<Xs &&>(xs),
                                                                     static_cast<Pred &&>(pred));
            }
    };

    template <std::size_t Len>
    struct any_of_helper<Len, Len> {
            template <typename Cond, typename Xs, typename Pred>
            static constexpr auto apply(Cond cond, Xs &&, Pred &&)
            {
                return cond;
            }
    };

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            return any_of_impl::any_of_helper<0, len>::apply(hana::false_c, static_cast<Xs &&>(xs),
                                                             static_cast<Pred &&>(pred));
    }
};

template <typename It>
struct any_of_impl<It, when<hana::Iterable<It>::value && !Sequence<It>::value>> {
    template <typename Xs, typename Pred>
    static constexpr auto lazy_any_of_helper(hana::false_, bool prev_cond, Xs &&xs, Pred &&pred)
    {
            decltype(auto) tail = hana::drop_front(static_cast<Xs &&>(xs));
            constexpr bool done = decltype(hana::is_empty(tail))::value;
            return prev_cond ? hana::true_c
                             : lazy_any_of_helper(hana::bool_<done>{},
                                                  hana::if_(pred(hana::front(xs)), hana::true_{}, hana::false_{}),
                                                  static_cast<decltype(tail) &&>(tail), static_cast<Pred &&>(pred));
    }

    template <typename Xs, typename Pred>
    static constexpr auto lazy_any_of_helper(hana::false_, hana::true_, Xs &&, Pred &&)
    {
            return hana::true_c;
    }

    template <typename Xs, typename Pred>
    static constexpr auto lazy_any_of_helper(hana::false_, hana::false_, Xs &&xs, Pred &&pred)
    {
            constexpr bool done = decltype(hana::is_empty(hana::drop_front(xs)))::value;
            return lazy_any_of_helper(hana::bool_c<done>, hana::if_(pred(hana::front(xs)), hana::true_c, hana::false_c),
                                      hana::drop_front(static_cast<Xs &&>(xs)), static_cast<Pred &&>(pred));
    }

    template <typename Cond, typename Xs, typename Pred>
    static constexpr auto lazy_any_of_helper(hana::true_, Cond cond, Xs &&, Pred &&)
    {
            return cond;
    }

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            constexpr bool done = decltype(hana::is_empty(xs))::value;
            return lazy_any_of_helper(hana::bool_c<done>, hana::false_c, static_cast<Xs &&>(xs),
                                      static_cast<Pred &&>(pred));
    }
};

template <typename T, std::size_t N>
struct any_of_impl<T[N]> {
    template <typename Xs, typename Pred>
    static constexpr bool any_of_helper(bool cond, Xs &&xs, Pred &&pred)
    {
            if (cond) return true;
            for (std::size_t i = 1; i < N; ++i)
                if (pred(static_cast<Xs &&>(xs)[i])) return true;
            return false;
    }

    template <typename Xs, typename Pred>
    static constexpr auto any_of_helper(hana::true_, Xs &&, Pred &&)
    {
            return hana::true_c;
    }

    template <typename Xs, typename Pred>
    static constexpr auto any_of_helper(hana::false_, Xs &&, Pred &&)
    {
            return hana::false_c;
    }

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            auto cond = hana::if_(pred(static_cast<Xs &&>(xs)[0]), hana::true_c, hana::false_c);
            return any_of_helper(cond, static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
    }
};

template <typename S>
struct any_of_impl<S, when<hana::Struct<S>::value>> {
    template <typename X, typename Pred>
    static constexpr decltype(auto) apply(X const &, Pred &&pred)
    {
            return hana::any_of(hana::accessors<S>(), hana::compose(static_cast<Pred &&>(pred), hana::first));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto all_of_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using AllOf =
        ::std::conditional_t<(hana::Searchable<S>::value), all_of_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::all_of(xs, pred) requires 'xs' to be a Searchable");

    return AllOf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

template <typename S, bool condition>
struct all_of_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            return hana::not_(
                hana::any_of(static_cast<Xs &&>(xs), hana::compose(hana::not_, static_cast<Pred &&>(pred))));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) and_t::operator()(X &&x, Y &&y) const
{
    using Bool = typename hana::tag_of<X>::type;
    using And =
        ::std::conditional_t<(hana::Logical<Bool>::value), and_impl<Bool>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Logical<Bool>::value, "hana::and_(x, y) requires 'x' to be a Logical");

    return And::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename X, typename... Y>
constexpr decltype(auto) and_t::operator()(X &&x, Y &&...y) const
{
    return detail::variadic::foldl1(*this, static_cast<X &&>(x), static_cast<Y &&>(y)...);
}

template <typename L, bool condition>
struct and_impl<L, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::if_(x, static_cast<Y &&>(y), x);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct Comparable;
}
} // namespace boost

namespace boost
{
namespace hana
{
template <typename T>
struct Comparable
    : hana::integral_constant<bool,
                              !is_default<equal_impl<typename tag_of<T>::type, typename tag_of<T>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename T, typename U = T, typename = void>
struct EqualityComparable : std::false_type {};

template <typename T>
struct EqualityComparable<T, T,
                          detail::void_t<decltype(static_cast<T &&>(*(T *)0) == static_cast<T &&>(*(T *)0) ? 0 : 0),
                                         decltype(static_cast<T &&>(*(T *)0) != static_cast<T &&>(*(T *)0) ? 0 : 0)>>
    : std::true_type {};

template <typename T, typename U>
struct EqualityComparable<
    T, U,
    typename std::enable_if<!std::is_same<T, U>::value,
                            detail::void_t<decltype(static_cast<T &&>(*(T *)0) == static_cast<U &&>(*(U *)0) ? 0 : 0),
                                           decltype(static_cast<U &&>(*(U *)0) == static_cast<T &&>(*(T *)0) ? 0 : 0),
                                           decltype(static_cast<T &&>(*(T *)0) != static_cast<U &&>(*(U *)0) ? 0 : 0),
                                           decltype(static_cast<U &&>(*(U *)0) != static_cast<T &&>(*(T *)0) ? 0 : 0),
                                           typename detail::std_common_type<T, U>::type>>::type>
    : std::integral_constant<bool, EqualityComparable<T>::value && EqualityComparable<U>::value
                                       && EqualityComparable<typename detail::std_common_type<T, U>::type>::value> {};

template <typename T, typename U = T, typename = void>
struct LessThanComparable : std::false_type {};

template <typename T>
struct LessThanComparable<T, T,
                          detail::void_t<decltype(static_cast<T &&>(*(T *)0) < static_cast<T &&>(*(T *)0) ? 0 : 0)>>
    : std::true_type {};

template <typename T, typename U>
struct LessThanComparable<
    T, U,
    std::enable_if_t<!std::is_same<T, U>::value,
                     detail::void_t<decltype(static_cast<T &&>(*(T *)0) < static_cast<U &&>(*(U *)0) ? 0 : 0),
                                    decltype(static_cast<U &&>(*(U *)0) < static_cast<T &&>(*(T *)0) ? 0 : 0),
                                    typename detail::std_common_type<T, U>::type>>>
    : std::integral_constant<bool, LessThanComparable<T>::value && LessThanComparable<U>::value
                                       && LessThanComparable<typename detail::std_common_type<T, U>::type>::value> {};

} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <template <typename...> class Concept, typename T, typename U, typename = void>
struct has_common_embedding_impl : std::false_type {};

template <template <typename...> class Concept, typename T, typename U>
struct has_common_embedding_impl<Concept, T, U, detail::void_t<typename common<T, U>::type>> {
    using Common = typename common<T, U>::type;
    using type = std::integral_constant<bool, Concept<T>::value && Concept<U>::value && Concept<Common>::value
                                                  && is_embedded<T, Common>::value && is_embedded<U, Common>::value>;
};

template <template <typename...> class Concept, typename T, typename U>
using has_common_embedding = typename has_common_embedding_impl<Concept, T, U>::type;

template <template <typename...> class Concept, typename T, typename U>
struct has_nontrivial_common_embedding_impl : has_common_embedding_impl<Concept, T, U> {};

template <template <typename...> class Concept, typename T>
struct has_nontrivial_common_embedding_impl<Concept, T, T> : std::false_type {};

template <template <typename...> class Concept, typename T, typename U>
using has_nontrivial_common_embedding = typename has_nontrivial_common_embedding_impl<Concept, T, U>::type;
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename Algorithm>
template <typename X>
constexpr decltype(auto) nested_to_t<Algorithm>::operator()(X &&x) const
{
    return hana::partial(Algorithm{}, static_cast<X &&>(x));
}

} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr auto equal_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Equal = equal_impl<T, U>;
    return Equal::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct equal_impl<T, U, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr auto apply(X const &, Y const &)
    {
            using T_ = typename hana::tag_of<X>::type;
            static_assert(!hana::is_convertible<T_, U>::value && !hana::is_convertible<U, T_>::value,
                          "No default implementation of hana::equal is provided for related "
                          "types that can't be safely embedded into a common type, because "
                          "those are most likely programming errors. If this is really what "
                          "you want, you can manually convert both objects to a common "
                          "Comparable type before performing the comparison. If you think "
                          "you have made your types Comparable but you see this, perhaps you "
                          "forgot to define some of the necessary methods for an automatic "
                          "model of Comparable to kick in. A possible culprit is defining "
                          "'operator==' but not 'operator!='.");

            return hana::false_c;
    }
};

template <typename T, typename U>
struct equal_impl<T, U,
                  when<detail::has_nontrivial_common_embedding<Comparable, T, U>::value
                       && !detail::EqualityComparable<T, U>::value>> {
    using C = typename hana::common<T, U>::type;
    template <typename X, typename Y>
    static constexpr auto apply(X &&x, Y &&y)
    {
            return hana::equal(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};

template <typename T, typename U>
struct equal_impl<T, U, when<detail::EqualityComparable<T, U>::value>> {
    template <typename X, typename Y>
    static constexpr auto apply(X &&x, Y &&y)
    {
            return static_cast<X &&>(x) == static_cast<Y &&>(y);
    }
};

template <typename C>
struct equal_impl<C, C, when<hana::Constant<C>::value && Comparable<typename C::value_type>::value>> {
    template <typename X, typename Y>
    static constexpr auto apply(X const &, Y const &)
    {
            constexpr auto eq = hana::equal(hana::value<X>(), hana::value<Y>());
            constexpr bool truth_value = hana::if_(eq, true, false);
            return hana::bool_<truth_value>{};
    }
};

template <typename T, typename U>
struct equal_impl<T, U, when<hana::Product<T>::value && hana::Product<U>::value>> {
    template <typename X, typename Y>
    static constexpr auto apply(X const &x, Y const &y)
    {
            return hana::and_(hana::equal(hana::first(x), hana::first(y)),
                              hana::equal(hana::second(x), hana::second(y)));
    }
};

namespace detail
{
template <typename Xs, typename Ys, std::size_t Length>
struct compare_finite_sequences {
    Xs const &xs;
    Ys const &ys;

    template <std::size_t i>
    constexpr auto apply(hana::false_, hana::true_) const
    {
            return compare_finite_sequences::apply<i + 1>(
                hana::bool_<i + 1 == Length>{},
                hana::if_(hana::equal(hana::at_c<i>(xs), hana::at_c<i>(ys)), hana::true_c, hana::false_c));
    }

    template <std::size_t i>
    constexpr auto apply(hana::false_, hana::false_) const
    {
            return hana::false_c;
    }

    template <std::size_t i, typename Result>
    constexpr auto apply(hana::true_, Result r) const
    {
            return r;
    }

    template <std::size_t i>
    constexpr bool apply(hana::false_, bool b) const
    {
            return b
                   && compare_finite_sequences::apply<i + 1>(
                       hana::bool_<i + 1 == Length>{},
                       hana::if_(hana::equal(hana::at_c<i>(xs), hana::at_c<i>(ys)), hana::true_c, hana::false_c));
    }
};
} // namespace detail

template <typename T, typename U>
struct equal_impl<T, U, when<Sequence<T>::value && hana::Sequence<U>::value>> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs const &xs, Ys const &ys)
    {
            constexpr std::size_t xs_size = decltype(hana::length(xs))::value;
            constexpr std::size_t ys_size = decltype(hana::length(ys))::value;
            detail::compare_finite_sequences<Xs, Ys, xs_size> comp{xs, ys};
            return comp.template apply<0>(hana::bool_<xs_size == 0>{}, hana::bool_<xs_size == ys_size>{});
    }
};

namespace detail
{
template <typename X, typename Y>
struct compare_struct_members {
    X const &x;
    Y const &y;

    template <typename Member>
    constexpr auto operator()(Member &&member) const
    {
            auto accessor = hana::second(static_cast<Member &&>(member));
            return hana::equal(accessor(x), accessor(y));
    }
};
} // namespace detail

template <typename S>
struct equal_impl<S, S, when<hana::Struct<S>::value && !detail::EqualityComparable<S, S>::value>> {
    template <typename X, typename Y>
    static constexpr auto apply(X const &x, Y const &y)
    {
            return hana::all_of(hana::accessors<S>(), detail::compare_struct_members<X, Y>{x, y});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Value, typename F>
constexpr auto adjust_t::operator()(Xs &&xs, Value &&value, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Adjust =
        ::std::conditional_t<(hana::Functor<S>::value), adjust_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Functor<S>::value, "hana::adjust(xs, value, f) requires 'xs' to be a Functor");

    return Adjust::apply(static_cast<Xs &&>(xs), static_cast<Value &&>(value), static_cast<F &&>(f));
}

template <typename Fun, bool condition>
struct adjust_impl<Fun, when<condition>> : default_ {
    template <typename Xs, typename Value, typename F>
    static constexpr auto apply(Xs &&xs, Value &&value, F &&f)
    {
            return hana::adjust_if(static_cast<Xs &&>(xs), hana::equal.to(static_cast<Value &&>(value)),
                                   static_cast<F &&>(f));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct all_impl : all_impl<S, when<true>> {};

struct all_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr all_t all{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto all_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using All = ::std::conditional_t<(hana::Searchable<S>::value), all_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::all(xs) requires 'xs' to be a Searchable");

    return All::apply(static_cast<Xs &&>(xs));
}

template <typename S, bool condition>
struct all_impl<S, when<condition>> : default_ {
    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return hana::all_of(static_cast<Xs &&>(xs), hana::id);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct any_impl : any_impl<S, when<true>> {};

struct any_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr any_t any{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto any_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Any = ::std::conditional_t<(hana::Searchable<S>::value), any_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::any(xs) requires 'xs' to be a Searchable");

    return Any::apply(static_cast<Xs &&>(xs));
}

template <typename S, bool condition>
struct any_impl<S, when<condition>> : default_ {
    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return hana::any_of(static_cast<Xs &&>(xs), hana::id);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
struct Monad;
}
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename N>
constexpr N factorial(N n)
{
    N result = 1;
    while (n != 0) result *= n--;
    return result;
}

template <typename T, std::size_t Size>
struct array {
    T elems_[Size > 0 ? Size : 1];

    constexpr T &operator[](std::size_t n)
    {
            return elems_[n];
    }

    constexpr T const &operator[](std::size_t n) const
    {
            return elems_[n];
    }

    constexpr std::size_t size() const noexcept
    {
            return Size;
    }

    constexpr T *begin() noexcept
    {
            return elems_;
    }
    constexpr T const *begin() const noexcept
    {
            return elems_;
    }
    constexpr T *end() noexcept
    {
            return elems_ + Size;
    }
    constexpr T const *end() const noexcept
    {
            return elems_ + Size;
    }

    constexpr array reverse() const
    {
            array result = *this;
            detail::reverse(result.begin(), result.end());
            return result;
    }

    template <typename BinaryPred>
    constexpr auto permutations(BinaryPred pred) const
    {
            array<array<T, Size>, detail::factorial(Size)> result{};
            auto out = result.begin();
            array copy = *this;

            do *out++ = copy;
            while (detail::next_permutation(copy.begin(), copy.end(), pred));

            return result;
    }

    constexpr auto permutations() const
    {
            return this->permutations(hana::_ < hana::_);
    }

    template <typename BinaryPred>
    constexpr auto sort(BinaryPred pred) const
    {
            array result = *this;
            detail::sort(result.begin(), result.end(), pred);
            return result;
    }

    constexpr auto sort() const
    {
            return this->sort(hana::_ < hana::_);
    }

    template <typename U>
    constexpr auto iota(U value) const
    {
            array result = *this;
            detail::iota(result.begin(), result.end(), value);
            return result;
    }
};

template <typename T, std::size_t M, typename U, std::size_t N>
constexpr bool operator==(array<T, M> a, array<U, N> b)
{
    return M == N && detail::equal(a.begin(), a.end(), b.begin(), b.end());
}

template <typename T, std::size_t M, typename U, std::size_t N>
constexpr bool operator<(array<T, M> a, array<U, N> b)
{
    return M < N || detail::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <std::size_t... Lengths>
struct flatten_indices {
    static constexpr std::size_t lengths[sizeof...(Lengths) + 1] = {Lengths..., 0};
    static constexpr auto flat_length = detail::accumulate(lengths, lengths + sizeof...(Lengths), 0);

    template <bool Inner>
    static constexpr auto compute()
    {
            detail::array<std::size_t, flat_length> indices{};
            for (std::size_t index = 0, i = 0; i < sizeof...(Lengths); ++i)
                for (std::size_t j = 0; j < lengths[i]; ++j, ++index) indices[index] = (Inner ? i : j);
            return indices;
    }

    static constexpr auto inner = compute<true>();
    static constexpr auto outer = compute<false>();

    template <typename Xs, typename F, std::size_t... i>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f, std::index_sequence<i...>)
    {
            return static_cast<F &&>(f)(hana::at_c<outer[i]>(hana::at_c<inner[i]>(static_cast<Xs &&>(xs)))...);
    }
};

struct make_flatten_indices {
    template <typename... Xs>
    auto operator()(Xs const &...xs) const -> detail::flatten_indices<decltype(hana::length(xs))::value...>;
};

template <typename Xs, typename F>
constexpr decltype(auto) unpack_flatten(Xs &&xs, F &&f)
{
    using Indices = decltype(hana::unpack(xs, make_flatten_indices{}));
    return Indices::apply(static_cast<Xs &&>(xs), static_cast<F &&>(f),
                          std::make_index_sequence<Indices::flat_length>{});
}
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto flatten_t::operator()(Xs &&xs) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Flatten =
        ::std::conditional_t<(hana::Monad<M>::value), flatten_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Monad<M>::value, "hana::flatten(xs) requires 'xs' to be a Monad");

    return Flatten::apply(static_cast<Xs &&>(xs));
}

template <typename M, bool condition>
struct flatten_impl<M, when<condition>> : default_ {
    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return hana::chain(static_cast<Xs &&>(xs), hana::id);
    }
};

template <typename S>
struct flatten_impl<S, when<Sequence<S>::value>> {
    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return detail::unpack_flatten(static_cast<Xs &&>(xs), hana::make<S>);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename M>
struct Monad : hana::integral_constant<bool, !is_default<flatten_impl<typename tag_of<M>::type>>::value
                                                 || !is_default<chain_impl<typename tag_of<M>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename F>
constexpr decltype(auto) chain_t::operator()(Xs &&xs, F &&f) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Chain = ::std::conditional_t<(hana::Monad<M>::value), chain_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Monad<M>::value, "hana::chain(xs, f) requires 'xs' to be a Monad");

    return Chain::apply(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

template <typename M, bool condition>
struct chain_impl<M, when<condition>> : default_ {
    template <typename Xs, typename F>
    static constexpr auto apply(Xs &&xs, F &&f)
    {
            return hana::flatten(hana::transform(static_cast<Xs &&>(xs), static_cast<F &&>(f)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename A>
struct Applicative;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename A>
template <typename X>
constexpr auto lift_t<A>::operator()(X &&x) const
{
    static_assert(hana::Applicative<A>::value, "hana::lift<A> requires 'A' to be an Applicative");

    using Lift =
        ::std::conditional_t<(hana::Applicative<A>::value), lift_impl<A>, ::boost::hana::deleted_implementation>;

    return Lift::apply(static_cast<X &&>(x));
}

template <typename A, bool condition>
struct lift_impl<A, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...args) = delete;
};

template <typename S>
struct lift_impl<S, when<Sequence<S>::value>> {
    template <typename X>
    static constexpr decltype(auto) apply(X &&x)
    {
            return hana::make<S>(static_cast<X &&>(x));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename A>
struct Applicative : hana::integral_constant<bool, !is_default<ap_impl<typename tag_of<A>::type>>::value
                                                       && !is_default<lift_impl<typename tag_of<A>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct apply_t {
    template <typename F, typename... Args>
    constexpr auto operator()(F &&f, Args &&...args) const
        -> decltype(static_cast<F &&>(f)(static_cast<Args &&>(args)...))
    {
            return static_cast<F &&>(f)(static_cast<Args &&>(args)...);
    }

    template <typename Base, typename T, typename Derived>
    constexpr auto operator()(T Base::*pmd, Derived &&ref) const -> decltype(static_cast<Derived &&>(ref).*pmd)
    {
            return static_cast<Derived &&>(ref).*pmd;
    }

    template <typename PMD, typename Pointer>
    constexpr auto operator()(PMD pmd, Pointer &&ptr) const -> decltype((*static_cast<Pointer &&>(ptr)).*pmd)
    {
            return (*static_cast<Pointer &&>(ptr)).*pmd;
    }

    template <typename Base, typename T, typename Derived, typename... Args>
    constexpr auto operator()(T Base::*pmf, Derived &&ref, Args &&...args) const
        -> decltype((static_cast<Derived &&>(ref).*pmf)(static_cast<Args &&>(args)...))
    {
            return (static_cast<Derived &&>(ref).*pmf)(static_cast<Args &&>(args)...);
    }

    template <typename PMF, typename Pointer, typename... Args>
    constexpr auto operator()(PMF pmf, Pointer &&ptr, Args &&...args) const
        -> decltype(((*static_cast<Pointer &&>(ptr)).*pmf)(static_cast<Args &&>(args)...))
    {
            return ((*static_cast<Pointer &&>(ptr)).*pmf)(static_cast<Args &&>(args)...);
    }
};

constexpr apply_t apply{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <std::size_t n, typename F>
struct curry_t;

template <std::size_t n>
struct make_curry_t {
    template <typename F>
    constexpr curry_t<n, typename detail::decay<F>::type> operator()(F &&f) const
    {
            return {static_cast<F &&>(f)};
    }
};

template <std::size_t n>
constexpr make_curry_t<n> curry{};

namespace curry_detail
{
namespace
{
template <std::size_t n>
constexpr make_curry_t<n> curry_or_call{};

template <>
constexpr auto curry_or_call<0> = apply;
} // namespace
} // namespace curry_detail

template <std::size_t n, typename F>
struct curry_t {
    F f;

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const &
    {
            static_assert(sizeof...(x) <= n, "too many arguments provided to boost::hana::curry");
            return curry_detail::curry_or_call<n - sizeof...(x)>(partial(f, static_cast<X &&>(x)...));
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &
    {
            static_assert(sizeof...(x) <= n, "too many arguments provided to boost::hana::curry");
            return curry_detail::curry_or_call<n - sizeof...(x)>(partial(f, static_cast<X &&>(x)...));
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &&
    {
            static_assert(sizeof...(x) <= n, "too many arguments provided to boost::hana::curry");
            return curry_detail::curry_or_call<n - sizeof...(x)>(partial(std::move(f), static_cast<X &&>(x)...));
    }
};

template <typename F>
struct curry_t<0, F> {
    F f;

    constexpr decltype(auto) operator()() const &
    {
            return f();
    }

    constexpr decltype(auto) operator()() &
    {
            return f();
    }

    constexpr decltype(auto) operator()() &&
    {
            return std::move(f)();
    }
};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename A, bool condition>
struct ap_impl<A, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...args) = delete;
};

template <typename F, typename X>
constexpr decltype(auto) ap_t::operator()(F &&f, X &&x) const
{
    using Function = typename hana::tag_of<F>::type;
    using Value = typename hana::tag_of<X>::type;
    using Ap = ::std::conditional_t<(hana::Applicative<Function>::value && hana::Applicative<Value>::value),
                                    ap_impl<Function>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Applicative<Function>::value, "hana::ap(f, x) requires 'f' to be an Applicative");

    static_assert(hana::Applicative<Value>::value, "hana::ap(f, x) requires 'x' to be an Applicative");

    return Ap::apply(static_cast<F &&>(f), static_cast<X &&>(x));
}

template <typename F, typename... Xs>
constexpr decltype(auto) ap_t::operator()(F &&f, Xs &&...xs) const
{
    static_assert(sizeof...(xs) >= 1, "hana::ap must be called with at least two arguments");

    return detail::variadic::foldl1(*this, hana::transform(static_cast<F &&>(f), hana::curry<sizeof...(xs)>),
                                    static_cast<Xs &&>(xs)...);
}

template <typename S>
struct ap_impl<S, when<Sequence<S>::value>> {
    template <typename F, typename X>
    static constexpr decltype(auto) apply(F &&f, X &&x)
    {
            return hana::chain(static_cast<F &&>(f), hana::partial(hana::transform, static_cast<X &&>(x)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct append_impl : append_impl<M, when<true>> {};

struct append_t {
    template <typename Xs, typename X>
    constexpr auto operator()(Xs &&xs, X &&x) const;
};

constexpr append_t append{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
struct MonadPlus;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
constexpr auto empty_t<M>::operator()() const
{
    static_assert(hana::MonadPlus<M>::value, "hana::empty<M>() requires 'M' to be a MonadPlus");

    using Empty =
        ::std::conditional_t<(hana::MonadPlus<M>::value), empty_impl<M>, ::boost::hana::deleted_implementation>;

    return Empty::apply();
}

template <typename M, bool condition>
struct empty_impl<M, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename S>
struct empty_impl<S, when<Sequence<S>::value>> {
    static constexpr auto apply()
    {
            return hana::make<S>();
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename M>
struct MonadPlus : hana::integral_constant<bool, !is_default<concat_impl<typename tag_of<M>::type>>::value
                                                     && !is_default<empty_impl<typename tag_of<M>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto concat_t::operator()(Xs &&xs, Ys &&ys) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Concat =
        ::std::conditional_t<(hana::MonadPlus<M>::value && std::is_same<typename hana::tag_of<Ys>::type, M>::value),
                             concat_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(std::is_same<typename hana::tag_of<Ys>::type, M>::value,
                  "hana::concat(xs, ys) requires 'xs' and 'ys' to have the same tag");

    static_assert(hana::MonadPlus<M>::value, "hana::concat(xs, ys) requires 'xs' and 'ys' to be MonadPlus");

    return Concat::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys));
}

template <typename M, bool condition>
struct concat_impl<M, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename S>
struct concat_impl<S, when<Sequence<S>::value>> {
    template <typename Xs, typename Ys, std::size_t... xi, std::size_t... yi>
    static constexpr auto concat_helper(Xs &&xs, Ys &&ys, std::index_sequence<xi...>, std::index_sequence<yi...>)
    {
            return hana::make<S>(hana::at_c<xi>(static_cast<Xs &&>(xs))..., hana::at_c<yi>(static_cast<Ys &&>(ys))...);
    }

    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys &&ys)
    {
            constexpr std::size_t xi = decltype(hana::length(xs))::value;
            constexpr std::size_t yi = decltype(hana::length(ys))::value;
            return concat_helper(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys), std::make_index_sequence<xi>{},
                                 std::make_index_sequence<yi>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename X>
constexpr auto append_t::operator()(Xs &&xs, X &&x) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Append =
        ::std::conditional_t<(hana::MonadPlus<M>::value), append_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::append(xs, x) requires 'xs' to be a MonadPlus");

    return Append::apply(static_cast<Xs &&>(xs), static_cast<X &&>(x));
}

template <typename M, bool condition>
struct append_impl<M, when<condition>> : default_ {
    template <typename Xs, typename X>
    static constexpr auto apply(Xs &&xs, X &&x)
    {
            return hana::concat(static_cast<Xs &&>(xs), hana::lift<M>(static_cast<X &&>(x)));
    }
};

template <typename S>
struct append_impl<S, when<Sequence<S>::value>> {
    template <typename Xs, typename X, std::size_t... i>
    static constexpr auto append_helper(Xs &&xs, X &&x, std::index_sequence<i...>)
    {
            return hana::make<S>(hana::at_c<i>(static_cast<Xs &&>(xs))..., static_cast<X &&>(x));
    }

    template <typename Xs, typename X>
    static constexpr auto apply(Xs &&xs, X &&x)
    {
            constexpr std::size_t N = decltype(hana::length(xs))::value;
            return append_helper(static_cast<Xs &&>(xs), static_cast<X &&>(x), std::make_index_sequence<N>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct at_key_impl : at_key_impl<S, when<true>> {};

struct at_key_t {
    template <typename Xs, typename Key>
    constexpr decltype(auto) operator()(Xs &&xs, Key const &key) const;
};

constexpr at_key_t at_key{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Key>
constexpr auto find_t::operator()(Xs &&xs, Key const &key) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Find =
        ::std::conditional_t<(hana::Searchable<S>::value), find_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::find(xs, key) requires 'xs' to be Searchable");

    return Find::apply(static_cast<Xs &&>(xs), key);
}

namespace detail
{
template <typename T>
struct equal_to {
    T const &t;
    template <typename U>
    constexpr auto operator()(U const &u) const
    {
            return hana::equal(t, u);
    }
};
} // namespace detail

template <typename S, bool condition>
struct find_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Key>
    static constexpr auto apply(Xs &&xs, Key const &key)
    {
            return hana::find_if(static_cast<Xs &&>(xs), detail::equal_to<Key>{key});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F, typename G>
struct on_t {
    F f;
    G g;
    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const &
    {
            return f(g(static_cast<X &&>(x))...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &
    {
            return f(g(static_cast<X &&>(x))...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &&
    {
            return std::move(f)(g(static_cast<X &&>(x))...);
    }
};

constexpr auto on = infix(detail::create<on_t>{});

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Key>
constexpr decltype(auto) at_key_t::operator()(Xs &&xs, Key const &key) const
{
    using S = typename hana::tag_of<Xs>::type;
    using AtKey =
        ::std::conditional_t<(hana::Searchable<S>::value), at_key_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::at_key(xs, key) requires 'xs' to be Searchable");

    return AtKey::apply(static_cast<Xs &&>(xs), key);
}

template <typename S, bool condition>
struct at_key_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Key>
    static constexpr auto apply(Xs &&xs, Key const &key)
    {
            return hana::find(static_cast<Xs &&>(xs), key).value();
    }
};

namespace at_key_detail
{
template <typename T>
struct equal_to {
    T const &t;
    template <typename U>
    constexpr auto operator()(U const &u) const
    {
            return hana::equal(t, u);
    }
};
} // namespace at_key_detail

template <typename S>
struct at_key_impl<S, when<hana::Sequence<S>::value>> {
    template <typename Xs, typename Key>
    static constexpr decltype(auto) apply(Xs &&xs, Key const &key)
    {
            using Result = decltype(hana::index_if(static_cast<Xs &&>(xs), at_key_detail::equal_to<Key>{key}));

            return hana::at(static_cast<Xs &&>(xs), Result{}.value());
    }
};

template <typename S>
struct at_key_impl<S, when<hana::Struct<S>::value>> {
    template <typename X, typename Key>
    static constexpr decltype(auto) apply(X &&x, Key const &key)
    {
            auto accessor =
                hana::second(*hana::find_if(hana::accessors<S>(), hana::equal.to(key) ^ hana::on ^ hana::first));
            return accessor(static_cast<X &&>(x));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It, typename = void>
struct back_impl : back_impl<It, when<true>> {};

struct back_t {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const;
};

constexpr back_t back{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr decltype(auto) back_t::operator()(Xs &&xs) const
{
    using It = typename hana::tag_of<Xs>::type;
    using Back =
        ::std::conditional_t<(hana::Iterable<It>::value), back_impl<It>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It>::value, "hana::back(xs) requires 'xs' to be an Iterable");

    return Back::apply(static_cast<Xs &&>(xs));
}

template <typename It, bool condition>
struct back_impl<It, when<condition>> : default_ {
    template <typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            static_assert(len > 0, "hana::back(xs) requires 'xs' to be non-empty");
            return hana::at_c<len - 1>(static_cast<Xs &&>(xs));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct cartesian_product_impl : cartesian_product_impl<S, when<true>> {};

struct cartesian_product_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr cartesian_product_t cartesian_product{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto cartesian_product_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using CartesianProduct = ::std::conditional_t<(hana::Sequence<S>::value), cartesian_product_impl<S>,
                                                  ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::cartesian_product(xs) requires 'xs' to be a Sequence");

    return CartesianProduct::apply(static_cast<Xs &&>(xs));
}

namespace detail
{
template <std::size_t... Lengths>
struct cartesian_product_indices {
    static constexpr std::size_t total_length()
    {
            std::size_t lengths[sizeof...(Lengths)] = {Lengths...};
            std::size_t r = 1;
            for (std::size_t len : lengths) r *= len;
            return r;
    }

    static constexpr std::size_t length = total_length();

    static constexpr auto indices_of(std::size_t i)
    {
            constexpr std::size_t lengths[sizeof...(Lengths)] = {Lengths...};
            constexpr std::size_t n = sizeof...(Lengths);
            detail::array<std::size_t, n> result{};
            for (std::size_t j = n; j--;) {
                result[j] = i % lengths[j];
                i /= lengths[j];
            }
            return result;
    }

    template <typename S, std::size_t n, std::size_t... k, typename... Xs>
    static constexpr auto product_element(std::index_sequence<k...>, Xs &&...xs)
    {
            constexpr auto indices = indices_of(n);
            return hana::make<S>(hana::at_c<indices[k]>(xs)...);
    }

    template <typename S, std::size_t... n, typename... Xs>
    static constexpr auto create_product(std::index_sequence<n...>, Xs &&...xs)
    {
            return hana::make<S>(product_element<S, n>(std::make_index_sequence<sizeof...(Xs)>{}, xs...)...);
    }
};
} // namespace detail

template <typename S, bool condition>
struct cartesian_product_impl<S, when<condition>> : default_ {
    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return hana::unpack(static_cast<Xs &&>(xs), cartesian_product_impl{});
    }

    template <typename... Xs>
    constexpr auto operator()(Xs &&...xs) const
    {
            using indices = detail::cartesian_product_indices<decltype(hana::length(xs))::value...>;
            return indices::template create_product<S>(std::make_index_sequence<indices::length>{},
                                                       static_cast<Xs &&>(xs)...);
    }

    constexpr auto operator()() const
    {
            return hana::make<S>();
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct comparing_t {
    template <typename F>
    constexpr auto operator()(F &&f) const;
};

constexpr comparing_t comparing{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename F>
struct equal_by {
    F f;

    template <typename X, typename Y>
    constexpr auto operator()(X &&x, Y &&y) const &
    {
            return hana::equal(f(static_cast<X &&>(x)), f(static_cast<Y &&>(y)));
    }

    template <typename X, typename Y>
    constexpr auto operator()(X &&x, Y &&y) &
    {
            return hana::equal(f(static_cast<X &&>(x)), f(static_cast<Y &&>(y)));
    }
};
} // namespace detail

template <typename F>
constexpr auto comparing_t::operator()(F &&f) const
{
    return detail::equal_by<typename detail::decay<F>::type>{static_cast<F &&>(f)};
}

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename W>
struct Comonad;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename W, typename = void>
struct duplicate_impl : duplicate_impl<W, when<true>> {};

struct duplicate_t {
    template <typename W_>
    constexpr decltype(auto) operator()(W_ &&w) const;
};

constexpr duplicate_t duplicate{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename W, typename = void>
struct extend_impl : extend_impl<W, when<true>> {};

struct extend_t {
    template <typename W_, typename F>
    constexpr decltype(auto) operator()(W_ &&w, F &&f) const;
};

constexpr extend_t extend{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename W_, typename F>
constexpr decltype(auto) extend_t::operator()(W_ &&w, F &&f) const
{
    using W = typename hana::tag_of<W_>::type;
    using Extend =
        ::std::conditional_t<(hana::Comonad<W>::value), extend_impl<W>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Comonad<W>::value, "hana::extend(w, f) requires 'w' to be a Comonad");

    return Extend::apply(static_cast<W_ &&>(w), static_cast<F &&>(f));
}

template <typename W, bool condition>
struct extend_impl<W, when<condition>> : default_ {
    template <typename X, typename F>
    static constexpr decltype(auto) apply(X &&x, F &&f)
    {
            return hana::transform(hana::duplicate(static_cast<X &&>(x)), static_cast<F &&>(f));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename W_>
constexpr decltype(auto) duplicate_t::operator()(W_ &&w) const
{
    using W = typename hana::tag_of<W_>::type;
    using Duplicate =
        ::std::conditional_t<(hana::Comonad<W>::value), duplicate_impl<W>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Comonad<W>::value, "hana::duplicate(w) requires 'w' to be a Comonad");

    return Duplicate::apply(static_cast<W_ &&>(w));
}

template <typename W, bool condition>
struct duplicate_impl<W, when<condition>> : default_ {
    template <typename X>
    static constexpr decltype(auto) apply(X &&x)
    {
            return hana::extend(static_cast<X &&>(x), hana::id);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename W, typename = void>
struct extract_impl : extract_impl<W, when<true>> {};

struct extract_t {
    template <typename W_>
    constexpr decltype(auto) operator()(W_ &&w) const;
};

constexpr extract_t extract{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename W_>
constexpr decltype(auto) extract_t::operator()(W_ &&w) const
{
    using W = typename hana::tag_of<W_>::type;
    using Extract =
        ::std::conditional_t<(hana::Comonad<W>::value), extract_impl<W>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Comonad<W>::value, "hana::extract(w) requires 'w' to be a Comonad");

    return Extract::apply(static_cast<W_ &&>(w));
}

template <typename W, bool condition>
struct extract_impl<W, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...args) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename W>
struct Comonad : hana::integral_constant<bool, !is_default<extract_impl<typename tag_of<W>::type>>::value
                                                   && (!is_default<duplicate_impl<typename tag_of<W>::type>>::value
                                                       || !is_default<extend_impl<typename tag_of<W>::type>>::value)> {
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename R>
struct EuclideanRing;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) div_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Div = ::std::conditional_t<(hana::EuclideanRing<T>::value && hana::EuclideanRing<U>::value
                                      && !is_default<div_impl<T, U>>::value),
                                     decltype(div_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::EuclideanRing<T>::value, "hana::div(x, y) requires 'x' to be an EuclideanRing");

    static_assert(hana::EuclideanRing<U>::value, "hana::div(x, y) requires 'y' to be an EuclideanRing");

    static_assert(!is_default<div_impl<T, U>>::value, "hana::div(x, y) requires 'x' and 'y' to be embeddable "
                                                      "in a common EuclideanRing");

    return Div::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct div_impl<T, U, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T, typename U>
struct div_impl<T, U, when<detail::has_nontrivial_common_embedding<EuclideanRing, T, U>::value>> {
    using C = typename common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::div(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};

template <typename T>
struct div_impl<T, T, when<std::is_integral<T>::value && !std::is_same<T, bool>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return static_cast<X &&>(x) / static_cast<Y &&>(y);
    }
};

namespace detail
{
template <typename C, typename X, typename Y>
struct constant_from_div {
    static constexpr auto value = hana::div(hana::value<X>(), hana::value<Y>());
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct div_impl<C, C, when<hana::Constant<C>::value && EuclideanRing<typename C::value_type>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X const &, Y const &)
    {
            return hana::to<C>(detail::constant_from_div<C, X, Y>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) mod_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Mod = ::std::conditional_t<(hana::EuclideanRing<T>::value && hana::EuclideanRing<U>::value
                                      && !is_default<mod_impl<T, U>>::value),
                                     decltype(mod_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::EuclideanRing<T>::value, "hana::mod(x, y) requires 'x' to be an EuclideanRing");

    static_assert(hana::EuclideanRing<U>::value, "hana::mod(x, y) requires 'y' to be an EuclideanRing");

    static_assert(!is_default<mod_impl<T, U>>::value, "hana::mod(x, y) requires 'x' and 'y' to be embeddable "
                                                      "in a common EuclideanRing");

    return Mod::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct mod_impl<T, U, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T, typename U>
struct mod_impl<T, U, when<detail::has_nontrivial_common_embedding<EuclideanRing, T, U>::value>> {
    using C = typename common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::mod(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};

template <typename T>
struct mod_impl<T, T, when<std::is_integral<T>::value && !std::is_same<T, bool>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return static_cast<X &&>(x) % static_cast<Y &&>(y);
    }
};

namespace detail
{
template <typename C, typename X, typename Y>
struct constant_from_mod {
    static constexpr auto value = hana::mod(hana::value<X>(), hana::value<Y>());
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct mod_impl<C, C, when<hana::Constant<C>::value && EuclideanRing<typename C::value_type>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X const &, Y const &)
    {
            return hana::to<C>(detail::constant_from_mod<C, X, Y>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename R>
struct EuclideanRing
    : hana::integral_constant<bool,
                              !is_default<mod_impl<typename tag_of<R>::type, typename tag_of<R>::type>>::value
                                  && !is_default<div_impl<typename tag_of<R>::type, typename tag_of<R>::type>>::value> {
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename G>
struct Group;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
struct Monoid;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
constexpr decltype(auto) zero_t<M>::operator()() const
{
    static_assert(hana::Monoid<M>::value, "hana::zero<M>() requires 'M' to be a Monoid");

    using Zero = ::std::conditional_t<(hana::Monoid<M>::value), zero_impl<M>, ::boost::hana::deleted_implementation>;

    return Zero::apply();
}

template <typename M, bool condition>
struct zero_impl<M, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T>
struct zero_impl<T, when<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>> {
    static constexpr T apply()
    {
            return static_cast<T>(0);
    }
};

namespace detail
{
template <typename C>
struct constant_from_zero {
    static constexpr auto value = hana::zero<typename C::value_type>();
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct zero_impl<C, when<hana::Constant<C>::value && Monoid<typename C::value_type>::value>> {
    static constexpr decltype(auto) apply()
    {
            return hana::to<C>(detail::constant_from_zero<C>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename M>
struct Monoid : hana::integral_constant<
                    bool, !is_default<zero_impl<typename tag_of<M>::type>>::value
                              && !is_default<plus_impl<typename tag_of<M>::type, typename tag_of<M>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) plus_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Plus =
        ::std::conditional_t<(hana::Monoid<T>::value && hana::Monoid<U>::value && !is_default<plus_impl<T, U>>::value),
                             decltype(plus_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Monoid<T>::value, "hana::plus(x, y) requires 'x' to be a Monoid");

    static_assert(hana::Monoid<U>::value, "hana::plus(x, y) requires 'y' to be a Monoid");

    static_assert(!is_default<plus_impl<T, U>>::value, "hana::plus(x, y) requires 'x' and 'y' to be embeddable "
                                                       "in a common Monoid");

    return Plus::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct plus_impl<T, U, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T, typename U>
struct plus_impl<T, U, when<detail::has_nontrivial_common_embedding<Monoid, T, U>::value>> {
    using C = typename common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::plus(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};

template <typename T>
struct plus_impl<T, T, when<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return static_cast<X &&>(x) + static_cast<Y &&>(y);
    }
};

namespace detail
{
template <typename C, typename X, typename Y>
struct constant_from_plus {
    static constexpr auto value = hana::plus(hana::value<X>(), hana::value<Y>());
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct plus_impl<C, C, when<hana::Constant<C>::value && Monoid<typename C::value_type>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X const &, Y const &)
    {
            return hana::to<C>(detail::constant_from_plus<C, X, Y>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) minus_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Minus = ::std::conditional_t<(hana::Group<T>::value && hana::Group<U>::value), decltype(minus_impl<T, U>{}),
                                       ::boost::hana::deleted_implementation>;

    static_assert(hana::Group<T>::value, "hana::minus(x, y) requires 'x' to be in a Group");

    static_assert(hana::Group<U>::value, "hana::minus(x, y) requires 'y' to be in a Group");

    return Minus::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct minus_impl<T, U, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T, bool condition>
struct minus_impl<T, T, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::plus(static_cast<X &&>(x), hana::negate(static_cast<Y &&>(y)));
    }
};

template <typename T, typename U>
struct minus_impl<T, U, when<detail::has_nontrivial_common_embedding<Group, T, U>::value>> {
    using C = typename common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::minus(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};

template <typename T>
struct minus_impl<T, T, when<std::is_arithmetic<T>::value && !std::is_same<bool, T>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return static_cast<X &&>(x) - static_cast<Y &&>(y);
    }
};

namespace detail
{
template <typename C, typename X, typename Y>
struct constant_from_minus {
    static constexpr auto value = hana::minus(hana::value<X>(), hana::value<Y>());
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct minus_impl<C, C, when<hana::Constant<C>::value && Group<typename C::value_type>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X const &, Y const &)
    {
            return hana::to<C>(detail::constant_from_minus<C, X, Y>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X>
constexpr decltype(auto) negate_t::operator()(X &&x) const
{
    using G = typename hana::tag_of<X>::type;
    using Negate = ::std::conditional_t<(hana::Group<G>::value), negate_impl<G>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Group<G>::value, "hana::negate(x) requires 'x' to be in a Group");

    return Negate::apply(static_cast<X &&>(x));
}

template <typename T, bool condition>
struct negate_impl<T, when<condition>> : default_ {
    template <typename X>
    static constexpr decltype(auto) apply(X &&x)
    {
            return hana::minus(hana::zero<T>(), static_cast<X &&>(x));
    }
};

template <typename T>
struct negate_impl<T, when<std::is_arithmetic<T>::value && !std::is_same<bool, T>::value>> {
    template <typename X>
    static constexpr decltype(auto) apply(X &&x)
    {
            return -static_cast<X &&>(x);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename G>
struct Group : hana::integral_constant<
                   bool, !is_default<negate_impl<typename tag_of<G>::type>>::value
                             || !is_default<minus_impl<typename tag_of<G>::type, typename tag_of<G>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T>
struct Hashable;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X>
constexpr auto hash_t::operator()(X const &x) const
{
    using Tag = typename hana::tag_of<X>::type;
    using Hash =
        ::std::conditional_t<(hana::Hashable<Tag>::value), hash_impl<Tag>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Hashable<Tag>::value, "hana::hash(x) requires 'x' to be Hashable");

    return Hash::apply(x);
}

template <typename Tag, bool condition>
struct hash_impl<Tag, when<condition>> : default_ {
    template <typename X>
    static constexpr auto apply(X const &) = delete;
};

namespace detail
{
template <typename T, typename = void>
struct hash_integral_helper;

template <typename Member, typename T>
struct hash_integral_helper<Member T::*> {
    template <typename X>
    static constexpr auto apply(X const &)
    {
            return hana::type_c<hana::integral_constant<Member T::*, X::value>>;
    }
};

template <typename T>
struct hash_integral_helper<T, typename std::enable_if<std::is_signed<T>::value>::type> {
    template <typename X>
    static constexpr auto apply(X const &)
    {
            constexpr signed long long x = X::value;
            return hana::type_c<hana::integral_constant<signed long long, x>>;
    }
};

template <typename T>
struct hash_integral_helper<T, typename std::enable_if<std::is_unsigned<T>::value>::type> {
    template <typename X>
    static constexpr auto apply(X const &)
    {
            constexpr unsigned long long x = X::value;
            return hana::type_c<hana::integral_constant<unsigned long long, x>>;
    }
};

template <>
struct hash_integral_helper<bool> {
    template <typename X>
    static constexpr auto apply(X const &)
    {
            return hana::type_c<hana::integral_constant<bool, X::value>>;
    }
};

template <>
struct hash_integral_helper<char> {
    template <typename X>
    static constexpr auto apply(X const &)
    {
            using T = std::conditional<std::is_signed<char>::value, signed long long, unsigned long long>::type;
            constexpr T x = X::value;
            return hana::type_c<hana::integral_constant<T, x>>;
    }
};
} // namespace detail

template <typename Tag>
struct hash_impl<Tag, when<hana::IntegralConstant<Tag>::value>> {
    template <typename X>
    static constexpr auto apply(X const &x)
    {
            using T = typename std::remove_cv<decltype(X::value)>::type;
            return detail::hash_integral_helper<T>::apply(x);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename T>
struct Hashable : hana::integral_constant<bool, !is_default<hash_impl<typename tag_of<T>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename F, typename Tag = typename tag_of<F>::type>
struct metafunction_dispatch : hana::integral_constant<bool, Metafunction<Tag>::value> {};

template <typename F>
struct metafunction_dispatch<F, F> : hana::integral_constant<bool, false> {};
} // namespace detail

template <typename F>
struct Metafunction : detail::metafunction_dispatch<F> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Ord>
struct Orderable;
}
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename Algorithm>
template <typename X>
constexpr decltype(auto) nested_than_t<Algorithm>::operator()(X &&x) const
{
    return hana::partial(hana::flip(Algorithm{}), static_cast<X &&>(x));
}

} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr auto less_equal_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using LessEqual = ::std::conditional_t<(hana::Orderable<T>::value && hana::Orderable<U>::value),
                                           decltype(less_equal_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Orderable<T>::value, "hana::less_equal(x, y) requires 'x' to be Orderable");

    static_assert(hana::Orderable<U>::value, "hana::less_equal(x, y) requires 'y' to be Orderable");

    return LessEqual::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct less_equal_impl<T, U, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::not_(hana::less(static_cast<Y &&>(y), static_cast<X &&>(x)));
    }
};

template <typename T, typename U>
struct less_equal_impl<T, U, when<detail::has_nontrivial_common_embedding<Orderable, T, U>::value>> {
    using C = typename hana::common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::less_equal(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct lexicographical_compare_impl : lexicographical_compare_impl<T, when<true>> {};

struct lexicographical_compare_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs const &xs, Ys const &ys) const;

    template <typename Xs, typename Ys, typename Pred>
    constexpr auto operator()(Xs const &xs, Ys const &ys, Pred const &pred) const;
};

constexpr lexicographical_compare_t lexicographical_compare{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto lexicographical_compare_t::operator()(Xs const &xs, Ys const &ys) const
{
    return hana::lexicographical_compare(xs, ys, hana::less);
}

template <typename Xs, typename Ys, typename Pred>
constexpr auto lexicographical_compare_t::operator()(Xs const &xs, Ys const &ys, Pred const &pred) const
{
    using It1 = typename hana::tag_of<Xs>::type;
    using It2 = typename hana::tag_of<Ys>::type;
    using LexicographicalCompare =
        ::std::conditional_t<(hana::Iterable<It1>::value && hana::Iterable<It2>::value),
                             lexicographical_compare_impl<It1>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It1>::value,
                  "hana::lexicographical_compare(xs, ys, pred) requires 'xs' to be Iterable");

    static_assert(hana::Iterable<It2>::value,
                  "hana::lexicographical_compare(xs, ys, pred) requires 'ys' to be Iterable");

    return LexicographicalCompare::apply(xs, ys, pred);
}

template <typename It, bool condition>
struct lexicographical_compare_impl<It, when<condition>> : default_ {
    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper2(Xs const &, Ys const &, Pred const &, hana::true_)
    {
            return hana::false_c;
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper2(Xs const &xs, Ys const &ys, Pred const &pred, hana::false_)
    {
            return apply(hana::drop_front(xs), hana::drop_front(ys), pred);
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper2(Xs const &xs, Ys const &ys, Pred const &pred, bool is_greater)
    {
            return is_greater ? false : apply(hana::drop_front(xs), hana::drop_front(ys), pred);
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper1(Xs const &, Ys const &, Pred const &, hana::true_)
    {
            return hana::true_c;
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper1(Xs const &xs, Ys const &ys, Pred const &pred, hana::false_)
    {
            return helper2(xs, ys, pred,
                           hana::if_(pred(hana::front(ys), hana::front(xs)), hana::true_c, hana::false_c));
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper1(Xs const &xs, Ys const &ys, Pred const &pred, bool is_less)
    {
            return is_less ? true
                           : helper2(xs, ys, pred,
                                     hana::if_(pred(hana::front(ys), hana::front(xs)), hana::true_c, hana::false_c));
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper(Xs const &, Ys const &ys, Pred const &, hana::true_)
    {
            return hana::not_(hana::is_empty(ys));
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto helper(Xs const &xs, Ys const &ys, Pred const &pred, hana::false_)
    {
            return helper1(xs, ys, pred,
                           hana::if_(pred(hana::front(xs), hana::front(ys)), hana::true_c, hana::false_c));
    }

    template <typename Xs, typename Ys, typename Pred>
    static constexpr auto apply(Xs const &xs, Ys const &ys, Pred const &pred)
    {
            return helper(xs, ys, pred,
                          hana::bool_c < decltype(hana::is_empty(xs))::value || decltype(hana::is_empty(ys))::value >);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) or_t::operator()(X &&x, Y &&y) const
{
    using Bool = typename hana::tag_of<X>::type;
    using Or = ::std::conditional_t<(hana::Logical<Bool>::value), or_impl<Bool>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Logical<Bool>::value, "hana::or_(x, y) requires 'x' to be a Logical");

    return Or::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename X, typename... Y>
constexpr decltype(auto) or_t::operator()(X &&x, Y &&...y) const
{
    return detail::variadic::foldl1(*this, static_cast<X &&>(x), static_cast<Y &&>(y)...);
}

template <typename L, bool condition>
struct or_impl<L, when<condition>> : hana::default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::if_(x, x, static_cast<Y &&>(y));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr auto less_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Less = ::std::conditional_t<(hana::Orderable<T>::value && hana::Orderable<U>::value
                                       && !is_default<less_impl<T, U>>::value),
                                      decltype(less_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Orderable<T>::value, "hana::less(x, y) requires 'x' to be Orderable");

    static_assert(hana::Orderable<U>::value, "hana::less(x, y) requires 'y' to be Orderable");

    static_assert(!is_default<less_impl<T, U>>::value, "hana::less(x, y) requires 'x' and 'y' to be embeddable "
                                                       "in a common Orderable");

    return Less::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct less_impl<T, U, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T, typename U>
struct less_impl<
    T, U,
    when<detail::has_nontrivial_common_embedding<Orderable, T, U>::value && !detail::LessThanComparable<T, U>::value>> {
    using C = typename hana::common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::less(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};

template <typename T, typename U>
struct less_impl<T, U, when<detail::LessThanComparable<T, U>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return static_cast<X &&>(x) < static_cast<Y &&>(y);
    }
};

template <typename C>
struct less_impl<C, C, when<hana::Constant<C>::value && Orderable<typename C::value_type>::value>> {
    template <typename X, typename Y>
    static constexpr auto apply(X const &, Y const &)
    {
            constexpr auto is_less = hana::less(hana::value<X>(), hana::value<Y>());
            constexpr bool truth_value = hana::if_(is_less, true, false);
            return hana::bool_c<truth_value>;
    }
};

template <typename T, typename U>
struct less_impl<T, U, when<hana::Product<T>::value && hana::Product<U>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X const &x, Y const &y)
    {
            return hana::or_(hana::less(hana::first(x), hana::first(y)),
                             hana::and_(hana::less_equal(hana::first(x), hana::first(y)),
                                        hana::less(hana::second(x), hana::second(y))));
    }
};

template <typename T, typename U>
struct less_impl<T, U, when<hana::Sequence<T>::value && hana::Sequence<U>::value>> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs const &xs, Ys const &ys)
    {
            return hana::lexicographical_compare(xs, ys);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename Ord>
struct Orderable
    : hana::integral_constant<bool,
                              !is_default<less_impl<typename tag_of<Ord>::type, typename tag_of<Ord>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename R>
struct Ring;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) mult_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Mult =
        ::std::conditional_t<(hana::Ring<T>::value && hana::Ring<U>::value && !is_default<mult_impl<T, U>>::value),
                             decltype(mult_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Ring<T>::value, "hana::mult(x, y) requires 'x' to be in a Ring");

    static_assert(hana::Ring<U>::value, "hana::mult(x, y) requires 'y' to be in a Ring");

    static_assert(!is_default<mult_impl<T, U>>::value, "hana::mult(x, y) requires 'x' and 'y' to be embeddable "
                                                       "in a common Ring");

    return Mult::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct mult_impl<T, U, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T, typename U>
struct mult_impl<T, U, when<detail::has_nontrivial_common_embedding<Ring, T, U>::value>> {
    using C = typename common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::mult(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};

template <typename T>
struct mult_impl<T, T, when<std::is_arithmetic<T>::value && !std::is_same<bool, T>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return static_cast<X &&>(x) * static_cast<Y &&>(y);
    }
};

namespace detail
{
template <typename C, typename X, typename Y>
struct constant_from_mult {
    static constexpr auto value = hana::mult(hana::value<X>(), hana::value<Y>());
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct mult_impl<C, C, when<hana::Constant<C>::value && Ring<typename C::value_type>::value>> {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X const &, Y const &)
    {
            return hana::to<C>(detail::constant_from_mult<C, X, Y>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename R, typename = void>
struct one_impl : one_impl<R, when<true>> {};

template <typename R>
struct one_t {
    constexpr decltype(auto) operator()() const;
};

template <typename R>
constexpr one_t<R> one{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename R>
constexpr decltype(auto) one_t<R>::operator()() const
{
    static_assert(hana::Ring<R>::value, "hana::one<R>() requires 'R' to be a Ring");

    using One = ::std::conditional_t<(hana::Ring<R>::value), one_impl<R>, ::boost::hana::deleted_implementation>;

    return One::apply();
}

template <typename R, bool condition>
struct one_impl<R, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename T>
struct one_impl<T, when<std::is_arithmetic<T>::value && !std::is_same<bool, T>::value>> {
    static constexpr T apply()
    {
            return static_cast<T>(1);
    }
};

namespace detail
{
template <typename C>
struct constant_from_one {
    static constexpr auto value = hana::one<typename C::value_type>();
    using hana_tag = detail::CanonicalConstant<typename C::value_type>;
};
} // namespace detail

template <typename C>
struct one_impl<C, when<hana::Constant<C>::value && Ring<typename C::value_type>::value>> {
    static constexpr decltype(auto) apply()
    {
            return hana::to<C>(detail::constant_from_one<C>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename R>
struct Ring : hana::integral_constant<
                  bool, !is_default<one_impl<typename tag_of<R>::type>>::value
                            && !is_default<mult_impl<typename tag_of<R>::type, typename tag_of<R>::type>>::value> {};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Key>
constexpr auto contains_t::operator()(Xs &&xs, Key &&key) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Contains =
        ::std::conditional_t<(hana::Searchable<S>::value), contains_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::contains(xs, key) requires 'xs' to be a Searchable");

    return Contains::apply(static_cast<Xs &&>(xs), static_cast<Key &&>(key));
}

template <typename S, bool condition>
struct contains_impl<S, when<condition>> : default_ {
    template <typename Xs, typename X>
    static constexpr auto apply(Xs &&xs, X &&x)
    {
            return hana::any_of(static_cast<Xs &&>(xs), hana::equal.to(static_cast<X &&>(x)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Tag, typename... T>
struct is_a_t;

template <typename Tag, typename... T>
constexpr is_a_t<Tag, T...> is_a{};

template <typename Tag, typename... T>
constexpr is_a_t<Tag, T...> is_an{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename DataType, typename T>
struct is_a_t<DataType, T> : integral_constant<bool, std::is_same<DataType, typename hana::tag_of<T>::type>::value> {};

template <typename DataType>
struct is_a_t<DataType> {
    template <typename T>
    constexpr auto operator()(T const &) const
    {
            return hana::is_a<DataType, T>;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct count_impl : count_impl<T, when<true>> {};

struct count_t {
    template <typename Xs, typename Value>
    constexpr auto operator()(Xs &&xs, Value &&value) const;
};

constexpr count_t count{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct count_if_impl : count_if_impl<T, when<true>> {};

struct count_if_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr count_if_t count_if{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto count_if_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using CountIf =
        ::std::conditional_t<(hana::Foldable<S>::value), count_if_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::count_if(xs, pred) requires 'xs' to be Foldable");

    return CountIf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

namespace detail
{
template <typename Pred>
struct count_pred {
    Pred pred;
    template <typename... Xs, typename = typename std::enable_if<detail::fast_and<
                                  Constant<decltype((*pred)(std::declval<Xs &&>()))>::value...>::value>::type>
    constexpr auto operator()(Xs &&...xs) const
    {
            constexpr bool results[] = {false,
                                        static_cast<bool>(hana::value<decltype((*pred)(static_cast<Xs &&>(xs)))>())...};
            constexpr std::size_t total = detail::count(results, results + sizeof(results), true);
            return hana::size_c<total>;
    }

    template <typename... Xs, typename = void,
              typename = typename std::enable_if<
                  !detail::fast_and<Constant<decltype((*pred)(std::declval<Xs &&>()))>::value...>::value>::type>
    constexpr auto operator()(Xs &&...xs) const
    {
            std::size_t total = 0;
            using Swallow = std::size_t[];
            (void)Swallow{0, ((*pred)(static_cast<Xs &&>(xs)) ? ++total : 0)...};
            return total;
    }
};
} // namespace detail

template <typename T, bool condition>
struct count_if_impl<T, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr decltype(auto) apply(Xs &&xs, Pred &&pred)
    {
            return hana::unpack(static_cast<Xs &&>(xs), detail::count_pred<decltype(&pred)>{&pred});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Value>
constexpr auto count_t::operator()(Xs &&xs, Value &&value) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Count =
        ::std::conditional_t<(hana::Foldable<S>::value), count_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::count(xs, value) requires 'xs' to be Foldable");

    return Count::apply(static_cast<Xs &&>(xs), static_cast<Value &&>(value));
}

template <typename T, bool condition>
struct count_impl<T, when<condition>> : default_ {
    template <typename Xs, typename Value>
    static constexpr auto apply(Xs &&xs, Value &&value)
    {
            return hana::count_if(static_cast<Xs &&>(xs), hana::equal.to(static_cast<Value &&>(value)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct cycle_impl : cycle_impl<M, when<true>> {};

struct cycle_t {
    template <typename Xs, typename N>
    constexpr auto operator()(Xs &&xs, N const &n) const;
};

constexpr cycle_t cycle{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr auto cycle_t::operator()(Xs &&xs, N const &n) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Cycle =
        ::std::conditional_t<(hana::MonadPlus<M>::value), cycle_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::cycle(xs, n) requires 'xs' to be a MonadPlus");

    static_assert(hana::IntegralConstant<N>::value, "hana::cycle(xs, n) requires 'n' to be an IntegralConstant");

    static_assert(N::value >= 0, "hana::cycle(xs, n) requires 'n' to be non-negative");

    return Cycle::apply(static_cast<Xs &&>(xs), n);
}

namespace detail
{
template <typename M, std::size_t n, bool = n % 2 == 0>
struct cycle_helper;

template <typename M>
struct cycle_helper<M, 0, true> {
    template <typename Xs>
    static constexpr auto apply(Xs const &)
    {
            return hana::empty<M>();
    }
};

template <typename M, std::size_t n>
struct cycle_helper<M, n, true> {
    template <typename Xs>
    static constexpr auto apply(Xs const &xs)
    {
            return cycle_helper<M, n / 2>::apply(hana::concat(xs, xs));
    }
};

template <typename M, std::size_t n>
struct cycle_helper<M, n, false> {
    template <typename Xs>
    static constexpr auto apply(Xs const &xs)
    {
            return hana::concat(xs, cycle_helper<M, n - 1>::apply(xs));
    }
};
} // namespace detail

template <typename M, bool condition>
struct cycle_impl<M, when<condition>> : default_ {
    template <typename Xs, typename N>
    static constexpr auto apply(Xs const &xs, N const &)
    {
            constexpr std::size_t n = N::value;
            return detail::cycle_helper<M, n>::apply(xs);
    }
};

namespace detail
{
template <std::size_t N, std::size_t Len>
struct cycle_indices {
    static constexpr auto compute_value()
    {
            detail::array<std::size_t, N * Len> indices{};

            std::size_t len = Len;
            for (std::size_t i = 0; i < N * Len; ++i) indices[i] = i % len;
            return indices;
    }

    static constexpr auto value = compute_value();
};
} // namespace detail

template <typename S>
struct cycle_impl<S, when<Sequence<S>::value>> {
    template <typename Indices, typename Xs, std::size_t... i>
    static constexpr auto cycle_helper(Xs &&xs, std::index_sequence<i...>)
    {
            constexpr auto indices = Indices::value;
            (void)indices;
            return hana::make<S>(hana::at_c<indices[i]>(xs)...);
    }

    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &)
    {
            constexpr std::size_t n = N::value;
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            using Indices = detail::cycle_indices<n, len>;
            return cycle_helper<Indices>(static_cast<Xs &&>(xs), std::make_index_sequence<n * len>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct difference_impl : difference_impl<S, when<true>> {};

struct difference_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs &&, Ys &&) const;
};

constexpr difference_t difference{};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct erase_key_impl : erase_key_impl<T, when<true>> {};

struct erase_key_t {
    template <typename Set, typename... Args>
    constexpr decltype(auto) operator()(Set &&set, Args &&...args) const;
};

constexpr erase_key_t erase_key{};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Set, typename... Args>
constexpr decltype(auto) erase_key_t::operator()(Set &&set, Args &&...args) const
{
    return erase_key_impl<typename hana::tag_of<Set>::type>::apply(static_cast<Set &&>(set),
                                                                   static_cast<Args &&>(args)...);
}

template <typename T, bool condition>
struct erase_key_impl<T, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto difference_t::operator()(Xs &&xs, Ys &&ys) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Difference = ::std::conditional_t<(true), difference_impl<S>, ::boost::hana::deleted_implementation>;

    return Difference::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys));
}

template <typename S, bool condition>
struct difference_impl<S, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct drop_back_impl : drop_back_impl<S, when<true>> {};

struct drop_back_t {
    template <typename Xs, typename N>
    constexpr auto operator()(Xs &&xs, N const &n) const;

    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr drop_back_t drop_back{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr auto drop_back_t::operator()(Xs &&xs, N const &n) const
{
    using S = typename hana::tag_of<Xs>::type;
    using DropBack = ::std::conditional_t<(hana::Sequence<S>::value && hana::IntegralConstant<N>::value),
                                          drop_back_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::drop_back(xs, n) requires 'xs' to be a Sequence");

    static_assert(hana::IntegralConstant<N>::value, "hana::drop_back(xs, n) requires 'n' to be an IntegralConstant");

    static_assert(N::value >= 0, "hana::drop_back(xs, n) requires 'n' to be non-negative");

    return DropBack::apply(static_cast<Xs &&>(xs), n);
}

template <typename Xs>
constexpr auto drop_back_t::operator()(Xs &&xs) const
{
    return (*this)(static_cast<Xs &&>(xs), hana::size_c<1>);
}

template <typename S, bool condition>
struct drop_back_impl<S, when<condition>> : default_ {
    template <typename Xs, std::size_t... n>
    static constexpr auto drop_back_helper(Xs &&xs, std::index_sequence<n...>)
    {
            return hana::make<S>(hana::at_c<n>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &)
    {
            constexpr std::size_t n = N::value;
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            return drop_back_helper(static_cast<Xs &&>(xs), std::make_index_sequence<(n > len ? 0 : len - n)>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It, typename = void>
struct drop_front_exactly_impl : drop_front_exactly_impl<It, when<true>> {};

struct drop_front_exactly_t {
    template <typename Xs, typename N>
    constexpr auto operator()(Xs &&xs, N const &n) const;

    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr drop_front_exactly_t drop_front_exactly{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr auto drop_front_exactly_t::operator()(Xs &&xs, N const &n) const
{
    using It = typename hana::tag_of<Xs>::type;
    using DropFrontExactly = ::std::conditional_t<(hana::Iterable<It>::value && hana::IntegralConstant<N>::value),
                                                  drop_front_exactly_impl<It>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It>::value, "hana::drop_front_exactly(xs, n) requires 'xs' to be an Iterable");

    static_assert(hana::IntegralConstant<N>::value,
                  "hana::drop_front_exactly(xs, n) requires 'n' to be an IntegralConstant");

    static_assert(N::value >= 0, "hana::drop_front_exactly(xs, n) requires 'n' to be non-negative");

    return DropFrontExactly::apply(static_cast<Xs &&>(xs), n);
}

template <typename Xs>
constexpr auto drop_front_exactly_t::operator()(Xs &&xs) const
{
    return (*this)(static_cast<Xs &&>(xs), hana::size_c<1>);
}

namespace detail
{
template <typename Xs, typename N>
constexpr void check_dfe_overflow(Xs const &xs, N const &, hana::true_)
{
    constexpr bool n_overflew_length =
        decltype(hana::is_empty(hana::drop_front(xs, hana::size_c<N::value - 1>)))::value;
    static_assert(!n_overflew_length, "hana::drop_front_exactly(xs, n) requires 'n' to be less than or "
                                      "equal to the number of elements in 'xs'");
}

template <typename Xs, typename N>
constexpr void check_dfe_overflow(Xs const &, N const &, hana::false_)
{
}
} // namespace detail

template <typename It, bool condition>
struct drop_front_exactly_impl<It, when<condition>> : default_ {
    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &n)
    {
            auto result = hana::drop_front(static_cast<Xs &&>(xs), n);
            constexpr bool check_for_overflow = decltype(hana::is_empty(result))::value && N::value != 0;

            detail::check_dfe_overflow(xs, n, hana::bool_c<check_for_overflow>);

            return result;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename It, typename = void>
struct drop_while_impl : drop_while_impl<It, when<true>> {};

struct drop_while_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr drop_while_t drop_while{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <bool, typename Pred, typename... Xs>
struct find_tail_size;

template <typename Pred, typename X, typename... Xs>
struct find_tail_size<true, Pred, X, Xs...> {
    static constexpr int value =
        find_tail_size<static_cast<bool>(hana::value<decltype(std::declval<Pred>()(std::declval<X>()))>()), Pred,
                       Xs...>::value;
};

template <typename Pred>
struct find_tail_size<true, Pred> {
    static constexpr int value = -1;
};

template <typename Pred, typename... Xs>
struct find_tail_size<false, Pred, Xs...> {
    static constexpr int value = sizeof...(Xs);
};

template <typename Pred>
struct first_unsatisfied_index {
    template <typename... Xs>
    constexpr auto operator()(Xs &&...) const
    {
            return hana::size_c<sizeof...(Xs) - 1 - find_tail_size<true, Pred, Xs &&...>::value>;
    }
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct lazy_tag {};

constexpr auto make_lazy = make<lazy_tag>;
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Indices, typename F, typename... Args>
struct lazy_apply_t;

namespace detail
{
struct lazy_secret {};
} // namespace detail

template <std::size_t... n, typename F, typename... Args>
struct lazy_apply_t<std::index_sequence<n...>, F, Args...> : detail::operators::adl<> {
    template <typename... T>
    constexpr lazy_apply_t(detail::lazy_secret, T &&...t) : storage_{static_cast<T &&>(t)...}
    {
    }

    basic_tuple<F, Args...> storage_;
    using hana_tag = lazy_tag;
};

template <typename X>
struct lazy_value_t : detail::operators::adl<> {
    template <typename Y>
    constexpr lazy_value_t(detail::lazy_secret, Y &&y) : storage_{static_cast<Y &&>(y)}
    {
    }

    basic_tuple<X> storage_;
    using hana_tag = lazy_tag;

    template <typename... Args>
    constexpr lazy_apply_t<std::make_index_sequence<sizeof...(Args)>, X, typename detail::decay<Args>::type...>
    operator()(Args &&...args) const &
    {
            return {detail::lazy_secret{}, hana::at_c<0>(storage_), static_cast<Args &&>(args)...};
    }

    template <typename... Args>
    constexpr lazy_apply_t<std::make_index_sequence<sizeof...(Args)>, X, typename detail::decay<Args>::type...>
    operator()(Args &&...args) &&
    {
            return {detail::lazy_secret{}, static_cast<X &&>(hana::at_c<0>(storage_)), static_cast<Args &&>(args)...};
    }
};

template <>
struct make_impl<lazy_tag> {
    template <typename X>
    static constexpr lazy_value_t<typename detail::decay<X>::type> apply(X &&x)
    {
            return {detail::lazy_secret{}, static_cast<X &&>(x)};
    }
};

namespace detail
{
template <>
struct monad_operators<lazy_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <>
struct eval_impl<lazy_tag> {
    template <std::size_t... n, typename F, typename... Args>
    static constexpr decltype(auto) apply(lazy_apply_t<std::index_sequence<n...>, F, Args...> const &expr)
    {
            return hana::at_c<0>(expr.storage_)(hana::at_c<n + 1>(expr.storage_)...);
    }

    template <std::size_t... n, typename F, typename... Args>
    static constexpr decltype(auto) apply(lazy_apply_t<std::index_sequence<n...>, F, Args...> &expr)
    {
            return hana::at_c<0>(expr.storage_)(hana::at_c<n + 1>(expr.storage_)...);
    }

    template <std::size_t... n, typename F, typename... Args>
    static constexpr decltype(auto) apply(lazy_apply_t<std::index_sequence<n...>, F, Args...> &&expr)
    {
            return static_cast<F &&>(hana::at_c<0>(expr.storage_))(
                static_cast<Args &&>(hana::at_c<n + 1>(expr.storage_))...);
    }

    template <typename X>
    static constexpr X const &apply(lazy_value_t<X> const &expr)
    {
            return hana::at_c<0>(expr.storage_);
    }

    template <typename X>
    static constexpr X &apply(lazy_value_t<X> &expr)
    {
            return hana::at_c<0>(expr.storage_);
    }

    template <typename X>
    static constexpr X apply(lazy_value_t<X> &&expr)
    {
            return static_cast<X &&>(hana::at_c<0>(expr.storage_));
    }
};

template <>
struct transform_impl<lazy_tag> {
    template <typename Expr, typename F>
    static constexpr auto apply(Expr &&expr, F &&f)
    {
            return hana::make_lazy(hana::compose(static_cast<F &&>(f), hana::eval))(static_cast<Expr &&>(expr));
    }
};

template <>
struct lift_impl<lazy_tag> {
    template <typename X>
    static constexpr lazy_value_t<typename detail::decay<X>::type> apply(X &&x)
    {
            return {detail::lazy_secret{}, static_cast<X &&>(x)};
    }
};

template <>
struct ap_impl<lazy_tag> {
    template <typename F, typename X>
    static constexpr decltype(auto) apply(F &&f, X &&x)
    {
            return hana::make_lazy(hana::on(hana::apply, hana::eval))(static_cast<F &&>(f), static_cast<X &&>(x));
    }
};

template <>
struct flatten_impl<lazy_tag> {
    template <typename Expr>
    static constexpr decltype(auto) apply(Expr &&expr)
    {
            return hana::make_lazy(hana::compose(hana::eval, hana::eval))(static_cast<Expr &&>(expr));
    }
};

template <>
struct extract_impl<lazy_tag> {
    template <typename Expr>
    static constexpr decltype(auto) apply(Expr &&expr)
    {
            return hana::eval(static_cast<Expr &&>(expr));
    }
};

template <>
struct duplicate_impl<lazy_tag> {
    template <typename Expr>
    static constexpr decltype(auto) apply(Expr &&expr)
    {
            return hana::make_lazy(static_cast<Expr &&>(expr));
    }
};

template <>
struct extend_impl<lazy_tag> {
    template <typename Expr, typename F>
    static constexpr decltype(auto) apply(Expr &&expr, F &&f)
    {
            return hana::make_lazy(static_cast<F &&>(f))(static_cast<Expr &&>(expr));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto drop_while_t::operator()(Xs &&xs, Pred &&pred) const
{
    using It = typename hana::tag_of<Xs>::type;
    using DropWhile =
        ::std::conditional_t<(hana::Iterable<It>::value), drop_while_impl<It>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Iterable<It>::value, "hana::drop_while(xs, pred) requires 'xs' to be an Iterable");

    return DropWhile::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

namespace iterable_detail
{
struct drop_while_helper {
    struct next {
            template <typename Xs, typename Pred>
            constexpr decltype(auto) operator()(Xs &&xs, Pred &&pred) const
            {
                return hana::drop_while(hana::drop_front(static_cast<Xs &&>(xs)), static_cast<Pred &&>(pred));
            }
    };

    template <typename Xs, typename Pred>
    constexpr decltype(auto) operator()(Xs &&xs, Pred &&pred) const
    {
            return hana::eval_if(pred(hana::front(xs)), hana::make_lazy(next{})(xs, pred), hana::make_lazy(xs));
    }
};
} // namespace iterable_detail

template <typename It, bool condition>
struct drop_while_impl<It, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            return hana::eval_if(hana::is_empty(xs), hana::make_lazy(xs),
                                 hana::make_lazy(iterable_detail::drop_while_helper{})(xs, static_cast<Pred &&>(pred)));
    }
};

template <typename S>
struct drop_while_impl<S, when<hana::Foldable<S>::value>> {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&)
    {
            using FirstUnsatisfied =
                decltype(hana::unpack(static_cast<Xs &&>(xs), detail::first_unsatisfied_index<Pred &&>{}));
            return hana::drop_front(static_cast<Xs &&>(xs), FirstUnsatisfied{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename = void>
struct fill_impl : fill_impl<Xs, when<true>> {};

struct fill_t {
    template <typename Xs, typename Value>
    constexpr auto operator()(Xs &&xs, Value &&value) const;
};

constexpr fill_t fill{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Value>
constexpr auto fill_t::operator()(Xs &&xs, Value &&value) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Fill = ::std::conditional_t<(hana::Functor<S>::value), fill_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Functor<S>::value, "hana::fill(xs, value) requires 'xs' to be a Functor");

    return Fill::apply(static_cast<Xs &&>(xs), static_cast<Value &&>(value));
}

template <typename Fun, bool condition>
struct fill_impl<Fun, when<condition>> : default_ {
    template <typename Xs, typename Value>
    static constexpr auto apply(Xs &&xs, Value &&v)
    {
            return hana::transform(static_cast<Xs &&>(xs), hana::always(static_cast<Value &&>(v)));
    }
};

template <typename S>
struct fill_impl<S, when<Sequence<S>::value>> {
    template <typename V>
    struct filler {
            V const &v;
            template <typename... Xs>
            constexpr auto operator()(Xs const &...xs) const
            {
                return hana::make<S>(((void)xs, v)...);
            }
    };

    template <typename Xs, typename V>
    static constexpr auto apply(Xs const &xs, V const &v)
    {
            return hana::unpack(xs, filler<V>{v});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct filter_impl : filter_impl<M, when<true>> {};

struct filter_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr filter_t filter{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto filter_t::operator()(Xs &&xs, Pred &&pred) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Filter =
        ::std::conditional_t<(hana::MonadPlus<M>::value), filter_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::filter(xs, pred) requires 'xs' to be a MonadPlus");

    return Filter::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

namespace detail
{
template <typename Pred, typename M>
struct lift_or_empty {
    template <typename X>
    static constexpr auto helper(X &&x, hana::true_)
    {
            return hana::lift<M>(static_cast<X &&>(x));
    }

    template <typename X>
    static constexpr auto helper(X &&, hana::false_)
    {
            return hana::empty<M>();
    }

    template <typename X>
    constexpr auto operator()(X &&x) const
    {
            constexpr bool cond = decltype(std::declval<Pred>()(x))::value;
            return helper(static_cast<X &&>(x), hana::bool_c<cond>);
    }
};
} // namespace detail

template <typename M, bool condition>
struct filter_impl<M, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr decltype(auto) apply(Xs &&xs, Pred const &)
    {
            return hana::chain(static_cast<Xs &&>(xs), detail::lift_or_empty<Pred, M>{});
    }
};

namespace detail
{
template <bool... b>
struct filter_indices {
    static constexpr auto compute_indices()
    {
            constexpr bool bs[] = {b..., false};
            constexpr std::size_t N = detail::count(bs, bs + sizeof(bs), true);
            detail::array<std::size_t, N> indices{};
            std::size_t *keep = &indices[0];
            for (std::size_t i = 0; i < sizeof...(b); ++i)
                if (bs[i]) *keep++ = i;
            return indices;
    }

    static constexpr auto cached_indices = compute_indices();
};

template <typename Pred>
struct make_filter_indices {
    Pred const &pred;
    template <typename... X>
    auto operator()(X &&...x) const
        -> filter_indices<static_cast<bool>(detail::decay<decltype(pred(static_cast<X &&>(x)))>::type::value)...>
    {
            return {};
    }
};
} // namespace detail

template <typename S>
struct filter_impl<S, when<Sequence<S>::value>> {
    template <typename Indices, typename Xs, std::size_t... i>
    static constexpr auto filter_helper(Xs &&xs, std::index_sequence<i...>)
    {
            return hana::make<S>(hana::at_c<Indices::cached_indices[i]>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred const &pred)
    {
            using Indices = decltype(hana::unpack(static_cast<Xs &&>(xs), detail::make_filter_indices<Pred>{pred}));

            return filter_impl::filter_helper<Indices>(static_cast<Xs &&>(xs),
                                                       std::make_index_sequence<Indices::cached_indices.size()>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

constexpr auto fold = fold_left;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct fold_right_impl : fold_right_impl<T, when<true>> {};

struct fold_right_t {
    template <typename Xs, typename State, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, State &&state, F &&f) const;

    template <typename Xs, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, F &&f) const;
};

constexpr fold_right_t fold_right{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
namespace variadic
{

template <unsigned int n, typename = when<true>>
struct foldr1_impl;

template <>
struct foldr1_impl<1> {
    template <typename F, typename X1>
    static constexpr X1 apply(F &&, X1 &&x1)
    {
            return static_cast<X1 &&>(x1);
    }
};

template <>
struct foldr1_impl<2> {
    template <typename F, typename X1, typename X2>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2)
    {
            return static_cast<F &&>(f)(static_cast<X1 &&>(x1), static_cast<X2 &&>(x2));
    }
};

template <>
struct foldr1_impl<3> {
    template <typename F, typename X1, typename X2, typename X3>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3)
    {
            return f(static_cast<X1 &&>(x1), f(static_cast<X2 &&>(x2), static_cast<X3 &&>(x3)));
    }
};

template <>
struct foldr1_impl<4> {
    template <typename F, typename X1, typename X2, typename X3, typename X4>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4)
    {
            return f(static_cast<X1 &&>(x1),
                     f(static_cast<X2 &&>(x2), f(static_cast<X3 &&>(x3), static_cast<X4 &&>(x4))));
    }
};

template <>
struct foldr1_impl<5> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5)
    {
            return f(static_cast<X1 &&>(x1),
                     f(static_cast<X2 &&>(x2),
                       f(static_cast<X3 &&>(x3), f(static_cast<X4 &&>(x4), static_cast<X5 &&>(x5)))));
    }
};

template <>
struct foldr1_impl<6> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6)
    {
            return f(static_cast<X1 &&>(x1),
                     f(static_cast<X2 &&>(x2),
                       f(static_cast<X3 &&>(x3),
                         f(static_cast<X4 &&>(x4), f(static_cast<X5 &&>(x5), static_cast<X6 &&>(x6))))));
    }
};

template <unsigned int n>
struct foldr1_impl<n, when<(n >= 7) && (n < 14)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7,
                                          Xn &&...xn)
    {
            return f(static_cast<X1 &&>(x1),
                     f(static_cast<X2 &&>(x2),
                       f(static_cast<X3 &&>(x3),
                         f(static_cast<X4 &&>(x4),
                           f(static_cast<X5 &&>(x5),
                             f(static_cast<X6 &&>(x6), foldr1_impl<sizeof...(xn) + 1>::apply(
                                                           f, static_cast<X7 &&>(x7), static_cast<Xn &&>(xn)...)))))));
    }
};

template <unsigned int n>
struct foldr1_impl<n, when<(n >= 14) && (n < 28)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename X8, typename X9, typename X10, typename X11, typename X12, typename X13, typename X14,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7, X8 &&x8,
                                          X9 &&x9, X10 &&x10, X11 &&x11, X12 &&x12, X13 &&x13, X14 &&x14, Xn &&...xn)
    {
            return f(static_cast<X1 &&>(x1),
                     f(static_cast<X2 &&>(x2),
                       f(static_cast<X3 &&>(x3),
                         f(static_cast<X4 &&>(x4),
                           f(static_cast<X5 &&>(x5),
                             f(static_cast<X6 &&>(x6),
                               f(static_cast<X7 &&>(x7),
                                 f(static_cast<X8 &&>(x8),
                                   f(static_cast<X9 &&>(x9),
                                     f(static_cast<X10 &&>(x10),
                                       f(static_cast<X11 &&>(x11),
                                         f(static_cast<X12 &&>(x12),
                                           f(static_cast<X13 &&>(x13),
                                             foldr1_impl<sizeof...(xn) + 1>::apply(
                                                 f, static_cast<X14 &&>(x14), static_cast<Xn &&>(xn)...))))))))))))));
    }
};

template <unsigned int n>
struct foldr1_impl<n, when<(n >= 28) && (n < 56)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename X8, typename X9, typename X10, typename X11, typename X12, typename X13, typename X14,
              typename X15, typename X16, typename X17, typename X18, typename X19, typename X20, typename X21,
              typename X22, typename X23, typename X24, typename X25, typename X26, typename X27, typename X28,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7, X8 &&x8,
                                          X9 &&x9, X10 &&x10, X11 &&x11, X12 &&x12, X13 &&x13, X14 &&x14, X15 &&x15,
                                          X16 &&x16, X17 &&x17, X18 &&x18, X19 &&x19, X20 &&x20, X21 &&x21, X22 &&x22,
                                          X23 &&x23, X24 &&x24, X25 &&x25, X26 &&x26, X27 &&x27, X28 &&x28, Xn &&...xn)
    {
            return f(static_cast<X1 &&>(x1),
                     f(static_cast<X2 &&>(x2),
                       f(static_cast<X3 &&>(x3),
                         f(static_cast<X4 &&>(x4),
                           f(static_cast<X5 &&>(x5),
                             f(static_cast<X6 &&>(x6),
                               f(static_cast<X7 &&>(x7),
                                 f(static_cast<X8 &&>(x8),
                                   f(static_cast<X9 &&>(x9),
                                     f(static_cast<X10 &&>(x10),
                                       f(static_cast<X11 &&>(x11),
                                         f(static_cast<X12 &&>(x12),
                                           f(static_cast<X13 &&>(x13),
                                             f(static_cast<X14 &&>(x14),
                                               f(static_cast<X15 &&>(x15),
                                                 f(static_cast<X16 &&>(x16),
                                                   f(static_cast<X17 &&>(x17),
                                                     f(static_cast<X18 &&>(x18),
                                                       f(static_cast<X19 &&>(x19),
                                                         f(static_cast<X20 &&>(x20),
                                                           f(static_cast<X21 &&>(x21),
                                                             f(static_cast<X22 &&>(x22),
                                                               f(static_cast<X23 &&>(x23),
                                                                 f(static_cast<X24 &&>(x24),
                                                                   f(static_cast<X25 &&>(x25),
                                                                     f(static_cast<X26 &&>(x26),
                                                                       f(static_cast<X27 &&>(x27),
                                                                         foldr1_impl<sizeof...(xn) + 1>::apply(
                                                                             f, static_cast<X28 &&>(x28),
                                                                             static_cast<Xn &&>(
                                                                                 xn)...))))))))))))))))))))))))))));
    }
};

template <unsigned int n>
struct foldr1_impl<n, when<(n >= 56)>> {
    template <typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7,
              typename X8, typename X9, typename X10, typename X11, typename X12, typename X13, typename X14,
              typename X15, typename X16, typename X17, typename X18, typename X19, typename X20, typename X21,
              typename X22, typename X23, typename X24, typename X25, typename X26, typename X27, typename X28,
              typename X29, typename X30, typename X31, typename X32, typename X33, typename X34, typename X35,
              typename X36, typename X37, typename X38, typename X39, typename X40, typename X41, typename X42,
              typename X43, typename X44, typename X45, typename X46, typename X47, typename X48, typename X49,
              typename X50, typename X51, typename X52, typename X53, typename X54, typename X55, typename X56,
              typename... Xn>
    static constexpr decltype(auto) apply(F &&f, X1 &&x1, X2 &&x2, X3 &&x3, X4 &&x4, X5 &&x5, X6 &&x6, X7 &&x7, X8 &&x8,
                                          X9 &&x9, X10 &&x10, X11 &&x11, X12 &&x12, X13 &&x13, X14 &&x14, X15 &&x15,
                                          X16 &&x16, X17 &&x17, X18 &&x18, X19 &&x19, X20 &&x20, X21 &&x21, X22 &&x22,
                                          X23 &&x23, X24 &&x24, X25 &&x25, X26 &&x26, X27 &&x27, X28 &&x28, X29 &&x29,
                                          X30 &&x30, X31 &&x31, X32 &&x32, X33 &&x33, X34 &&x34, X35 &&x35, X36 &&x36,
                                          X37 &&x37, X38 &&x38, X39 &&x39, X40 &&x40, X41 &&x41, X42 &&x42, X43 &&x43,
                                          X44 &&x44, X45 &&x45, X46 &&x46, X47 &&x47, X48 &&x48, X49 &&x49, X50 &&x50,
                                          X51 &&x51, X52 &&x52, X53 &&x53, X54 &&x54, X55 &&x55, X56 &&x56, Xn &&...xn)
    {
            return f(static_cast<X1&&>(x1), f(static_cast<X2&&>(x2), f(static_cast<X3&&>(x3), f(static_cast<X4&&>(x4), f(static_cast<X5&&>(x5), f(static_cast<X6&&>(x6), f(static_cast<X7&&>(x7),
                   f(static_cast<X8&&>(x8), f(static_cast<X9&&>(x9), f(static_cast<X10&&>(x10), f(static_cast<X11&&>(x11), f(static_cast<X12&&>(x12), f(static_cast<X13&&>(x13), f(static_cast<X14&&>(x14),
                   f(static_cast<X15&&>(x15), f(static_cast<X16&&>(x16), f(static_cast<X17&&>(x17), f(static_cast<X18&&>(x18), f(static_cast<X19&&>(x19), f(static_cast<X20&&>(x20), f(static_cast<X21&&>(x21),
                   f(static_cast<X22&&>(x22), f(static_cast<X23&&>(x23), f(static_cast<X24&&>(x24), f(static_cast<X25&&>(x25), f(static_cast<X26&&>(x26), f(static_cast<X27&&>(x27), f(static_cast<X28&&>(x28),
                   f(static_cast<X29&&>(x29), f(static_cast<X30&&>(x30), f(static_cast<X31&&>(x31), f(static_cast<X32&&>(x32), f(static_cast<X33&&>(x33), f(static_cast<X34&&>(x34), f(static_cast<X35&&>(x35),
                   f(static_cast<X36&&>(x36), f(static_cast<X37&&>(x37), f(static_cast<X38&&>(x38), f(static_cast<X39&&>(x39), f(static_cast<X40&&>(x40), f(static_cast<X41&&>(x41), f(static_cast<X42&&>(x42),
                   f(static_cast<X43&&>(x43), f(static_cast<X44&&>(x44), f(static_cast<X45&&>(x45), f(static_cast<X46&&>(x46), f(static_cast<X47&&>(x47), f(static_cast<X48&&>(x48), f(static_cast<X49&&>(x49),
                   f(static_cast<X50&&>(x50), f(static_cast<X51&&>(x51), f(static_cast<X52&&>(x52), f(static_cast<X53&&>(x53), f(static_cast<X54&&>(x54), f(static_cast<X55&&>(x55),
                     foldr1_impl<sizeof...(xn) + 1>::apply(f, static_cast<X56&&>(x56), static_cast<Xn&&>(xn)...))))))))))))))))))))))))))))))))))))))))))))))))))))))));
    }
};

struct foldr1_t {
    template <typename F, typename X1, typename... Xn>
    constexpr decltype(auto) operator()(F &&f, X1 &&x1, Xn &&...xn) const
    {
            return foldr1_impl<sizeof...(xn) + 1>::apply(static_cast<F &&>(f), static_cast<X1 &&>(x1),
                                                         static_cast<Xn &&>(xn)...);
    }
};

constexpr foldr1_t foldr1{};

struct foldr_t {
    template <typename F, typename State, typename... Xn>
    constexpr decltype(auto) operator()(F &&f, State &&state, Xn &&...xn) const
    {
            return foldr1_impl<sizeof...(xn) + 1>::apply(static_cast<F &&>(f), static_cast<Xn &&>(xn)...,
                                                         static_cast<State &&>(state));
    }
};

constexpr foldr_t foldr{};
} // namespace variadic
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename State, typename F>
constexpr decltype(auto) fold_right_t::operator()(Xs &&xs, State &&state, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using FoldRight =
        ::std::conditional_t<(hana::Foldable<S>::value), fold_right_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::fold_right(xs, state, f) requires 'xs' to be Foldable");

    return FoldRight::apply(static_cast<Xs &&>(xs), static_cast<State &&>(state), static_cast<F &&>(f));
}

template <typename Xs, typename F>
constexpr decltype(auto) fold_right_t::operator()(Xs &&xs, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using FoldRight =
        ::std::conditional_t<(hana::Foldable<S>::value), fold_right_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::fold_right(xs, f) requires 'xs' to be Foldable");

    return FoldRight::apply(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

namespace detail
{
template <typename F, typename State>
struct variadic_foldr {
    F &f;
    State &state;
    template <typename... T>
    constexpr decltype(auto) operator()(T &&...t) const
    {
            return detail::variadic::foldr(static_cast<F &&>(f), static_cast<State &&>(state), static_cast<T &&>(t)...);
    }
};
} // namespace detail

template <typename T, bool condition>
struct fold_right_impl<T, when<condition>> : default_ {
    template <typename Xs, typename S, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, S &&s, F &&f)
    {
            return hana::unpack(static_cast<Xs &&>(xs), detail::variadic_foldr<F, S>{f, s});
    }

    template <typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            return hana::unpack(static_cast<Xs &&>(xs), hana::partial(detail::variadic::foldr1, static_cast<F &&>(f)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct for_each_impl : for_each_impl<T, when<true>> {};

struct for_each_t {
    template <typename Xs, typename F>
    constexpr void operator()(Xs &&xs, F &&f) const;
};

constexpr for_each_t for_each{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename F>
constexpr void for_each_t::operator()(Xs &&xs, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using ForEach =
        ::std::conditional_t<(hana::Foldable<S>::value), for_each_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::for_each(xs, f) requires 'xs' to be Foldable");

    return ForEach::apply(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

namespace detail
{
template <typename F>
struct on_each {
    F f;
    template <typename... Xs>
    constexpr void operator()(Xs &&...xs) const
    {
            using Swallow = int[];
            (void)Swallow{0, ((void)(*f)(static_cast<Xs &&>(xs)), 0)...};
    }
};
} // namespace detail

template <typename T, bool condition>
struct for_each_impl<T, when<condition>> : default_ {
    template <typename Xs, typename F>
    static constexpr void apply(Xs &&xs, F &&f)
    {
            hana::unpack(static_cast<Xs &&>(xs), detail::on_each<decltype(&f)>{&f});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <std::size_t n, typename = void>
struct arg_t;

template <>
struct arg_t<1> {
    template <typename X1, typename... Xn>
    constexpr X1 operator()(X1 &&x1, Xn &&...) const
    {
            return static_cast<X1 &&>(x1);
    }
};

template <>
struct arg_t<2> {
    template <typename X1, typename X2, typename... Xn>
    constexpr X2 operator()(X1 &&, X2 &&x2, Xn &&...) const
    {
            return static_cast<X2 &&>(x2);
    }
};

template <>
struct arg_t<3> {
    template <typename X1, typename X2, typename X3, typename... Xn>
    constexpr X3 operator()(X1 &&, X2 &&, X3 &&x3, Xn &&...) const
    {
            return static_cast<X3 &&>(x3);
    }
};

template <>
struct arg_t<4> {
    template <typename X1, typename X2, typename X3, typename X4, typename... Xn>
    constexpr X4 operator()(X1 &&, X2 &&, X3 &&, X4 &&x4, Xn &&...) const
    {
            return static_cast<X4 &&>(x4);
    }
};

template <>
struct arg_t<5> {
    template <typename X1, typename X2, typename X3, typename X4, typename X5, typename... Xn>
    constexpr X5 operator()(X1 &&, X2 &&, X3 &&, X4 &&, X5 &&x5, Xn &&...) const
    {
            return static_cast<X5 &&>(x5);
    }
};

template <std::size_t n, typename>
struct arg_t {
    static_assert(n > 0, "invalid usage of boost::hana::arg<n> with n == 0");

    template <typename X1, typename X2, typename X3, typename X4, typename X5, typename... Xn>
    constexpr decltype(auto) operator()(X1 &&, X2 &&, X3 &&, X4 &&, X5 &&, Xn &&...xn) const
    {
            static_assert(sizeof...(xn) >= n - 5, "invalid usage of boost::hana::arg<n> with too few arguments");

            return arg_t < n == 0 ? 1 : n - 5 > {}(static_cast<Xn &&>(xn)...);
    }
};

template <std::size_t n>
struct arg_t<n, std::enable_if_t<(n > 25)>> {
    template <typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7, typename X8,
              typename X9, typename X10, typename X11, typename X12, typename X13, typename X14, typename X15,
              typename X16, typename X17, typename X18, typename X19, typename X20, typename X21, typename X22,
              typename X23, typename X24, typename X25, typename... Xn>
    constexpr decltype(auto) operator()(X1 &&, X2 &&, X3 &&, X4 &&, X5 &&, X6 &&, X7 &&, X8 &&, X9 &&, X10 &&, X11 &&,
                                        X12 &&, X13 &&, X14 &&, X15 &&, X16 &&, X17 &&, X18 &&, X19 &&, X20 &&, X21 &&,
                                        X22 &&, X23 &&, X24 &&, X25 &&, Xn &&...xn) const
    {
            return arg_t<n - 25>{}(static_cast<Xn &&>(xn)...);
    }
};

template <std::size_t n>
constexpr arg_t<n> arg{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

namespace detail
{
template <typename F, typename Closure, std::size_t... i>
constexpr auto apply_capture(F &&f, Closure &&closure, std::index_sequence<i...>)
{
    return hana::partial(static_cast<F &&>(f), hana::at_c<i>(static_cast<Closure &&>(closure).storage_)...);
}
} // namespace detail

template <typename... X>
struct capture_t;

struct make_capture_t {
    struct secret {};
    template <typename... X>
    constexpr capture_t<typename detail::decay<X>::type...> operator()(X &&...x) const
    {
            return {secret{}, static_cast<X &&>(x)...};
    }
};

template <typename... X>
struct capture_t {
    template <typename... Y>
    constexpr capture_t(make_capture_t::secret, Y &&...y) : storage_{static_cast<Y &&>(y)...}
    {
    }

    basic_tuple<X...> storage_;

    template <typename F>
    constexpr auto operator()(F &&f) const &
    {
            return detail::apply_capture(static_cast<F &&>(f), *this, std::make_index_sequence<sizeof...(X)>{});
    }

    template <typename F>
    constexpr auto operator()(F &&f) &
    {
            return detail::apply_capture(static_cast<F &&>(f), *this, std::make_index_sequence<sizeof...(X)>{});
    }

    template <typename F>
    constexpr auto operator()(F &&f) &&
    {
            return detail::apply_capture(static_cast<F &&>(f), static_cast<capture_t &&>(*this),
                                         std::make_index_sequence<sizeof...(X)>{});
    }
};

constexpr make_capture_t capture{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F>
struct pre_demux_t;

struct make_pre_demux_t {
    struct secret {};
    template <typename F>
    constexpr pre_demux_t<typename detail::decay<F>::type> operator()(F &&f) const
    {
            return {static_cast<F &&>(f)};
    }
};

template <typename Indices, typename F, typename... G>
struct demux_t;

template <typename F>
struct pre_demux_t {
    F f;

    template <typename... G>
    constexpr demux_t<std::make_index_sequence<sizeof...(G)>, F, typename detail::decay<G>::type...> operator()(
        G &&...g) const &
    {
            return {make_pre_demux_t::secret{}, this->f, static_cast<G &&>(g)...};
    }

    template <typename... G>
    constexpr demux_t<std::make_index_sequence<sizeof...(G)>, F, typename detail::decay<G>::type...> operator()(
        G &&...g) &&
    {
            return {make_pre_demux_t::secret{}, static_cast<F &&>(this->f), static_cast<G &&>(g)...};
    }
};

template <std::size_t... n, typename F, typename... G>
struct demux_t<std::index_sequence<n...>, F, G...> {
    template <typename... T>
    constexpr demux_t(make_pre_demux_t::secret, T &&...t) : storage_{static_cast<T &&>(t)...}
    {
    }

    basic_tuple<F, G...> storage_;

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const &
    {
            return hana::at_c<0>(storage_)(hana::at_c<n + 1>(storage_)(x...)...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &
    {
            return hana::at_c<0>(storage_)(hana::at_c<n + 1>(storage_)(x...)...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &&
    {
            return static_cast<F &&>(hana::at_c<0>(storage_))(static_cast<G &&>(hana::at_c<n + 1>(storage_))(x...)...);
    }
};

template <typename F, typename G>
struct demux_t<std::index_sequence<0>, F, G> {
    template <typename... T>
    constexpr demux_t(make_pre_demux_t::secret, T &&...t) : storage_{static_cast<T &&>(t)...}
    {
    }

    basic_tuple<F, G> storage_;

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const &
    {
            return hana::at_c<0>(storage_)(hana::at_c<1>(storage_)(static_cast<X &&>(x)...));
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &
    {
            return hana::at_c<0>(storage_)(hana::at_c<1>(storage_)(static_cast<X &&>(x)...));
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &&
    {
            return static_cast<F &&>(hana::at_c<0>(storage_))(
                static_cast<G &&>(hana::at_c<1>(storage_))(static_cast<X &&>(x)...));
    }
};

constexpr make_pre_demux_t demux{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F>
struct fix_t;

constexpr detail::create<fix_t> fix{};

template <typename F>
struct fix_t {
    F f;

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const &
    {
            return f(fix(f), static_cast<X &&>(x)...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &
    {
            return f(fix(f), static_cast<X &&>(x)...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &&
    {
            return std::move(f)(fix(f), static_cast<X &&>(x)...);
    }
};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <std::size_t n, typename = when<true>>
struct iterate_t;

template <>
struct iterate_t<0> {
    template <typename F, typename X>
    constexpr X operator()(F &&, X &&x) const
    {
            return static_cast<X &&>(x);
    }
};

template <>
struct iterate_t<1> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return f(static_cast<X &&>(x));
    }
};

template <>
struct iterate_t<2> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return f(f(static_cast<X &&>(x)));
    }
};

template <>
struct iterate_t<3> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return f(f(f(static_cast<X &&>(x))));
    }
};

template <>
struct iterate_t<4> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return f(f(f(f(static_cast<X &&>(x)))));
    }
};

template <>
struct iterate_t<5> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return f(f(f(f(f(static_cast<X &&>(x))))));
    }
};

template <std::size_t n>
struct iterate_t<n, when<(n >= 6) && (n < 12)>> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return iterate_t<n - 6>{}(f, f(f(f(f(f(f(static_cast<X &&>(x))))))));
    }
};

template <std::size_t n>
struct iterate_t<n, when<(n >= 12) && (n < 24)>> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return iterate_t<n - 12>{}(f, f(f(f(f(f(f(f(f(f(f(f(f(static_cast<X &&>(x))))))))))))));
    }
};

template <std::size_t n>
struct iterate_t<n, when<(n >= 24) && (n < 48)>> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return iterate_t<n - 24>{}(
                f, f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(static_cast<X &&>(x))))))))))))))))))))))))));
    }
};

template <std::size_t n>
struct iterate_t<n, when<(n >= 48)>> {
    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return iterate_t<n - 48>{}(
                f, f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(
                       f(f(f(f(f(f(f(f(f(f(f(f(f(static_cast<X &&>(x))))))))))))))))))))))))))))))))))))))))))))))))));
    }
};

template <std::size_t n>
struct make_iterate_t {
    template <typename F>
    constexpr decltype(auto) operator()(F &&f) const
    {
            return hana::partial(iterate_t<n>{}, static_cast<F &&>(f));
    }

    template <typename F, typename X>
    constexpr decltype(auto) operator()(F &&f, X &&x) const
    {
            return iterate_t<n>{}(static_cast<F &&>(f), static_cast<X &&>(x));
    }
};

template <std::size_t n>
constexpr make_iterate_t<n> iterate{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Indices, typename F, typename... G>
struct lockstep_t;

template <typename F>
struct pre_lockstep_t;

struct make_pre_lockstep_t {
    struct secret {};
    template <typename F>
    constexpr pre_lockstep_t<typename detail::decay<F>::type> operator()(F &&f) const
    {
            return {static_cast<F &&>(f)};
    }
};

template <std::size_t... n, typename F, typename... G>
struct lockstep_t<std::index_sequence<n...>, F, G...> {
    template <typename... T>
    constexpr lockstep_t(make_pre_lockstep_t::secret, T &&...t) : storage_{static_cast<T &&>(t)...}
    {
    }

    basic_tuple<F, G...> storage_;

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) const &
    {
            return hana::at_c<0>(storage_)(hana::at_c<n + 1>(storage_)(static_cast<X &&>(x))...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &
    {
            return hana::at_c<0>(storage_)(hana::at_c<n + 1>(storage_)(static_cast<X &&>(x))...);
    }

    template <typename... X>
    constexpr decltype(auto) operator()(X &&...x) &&
    {
            return static_cast<F &&>(hana::at_c<0>(storage_))(
                static_cast<G &&>(hana::at_c<n + 1>(storage_))(static_cast<X &&>(x))...);
    }
};

template <typename F>
struct pre_lockstep_t {
    F f;

    template <typename... G>
    constexpr lockstep_t<std::make_index_sequence<sizeof...(G)>, F, typename detail::decay<G>::type...> operator()(
        G &&...g) const &
    {
            return {make_pre_lockstep_t::secret{}, this->f, static_cast<G &&>(g)...};
    }

    template <typename... G>
    constexpr lockstep_t<std::make_index_sequence<sizeof...(G)>, F, typename detail::decay<G>::type...> operator()(
        G &&...g) &&
    {
            return {make_pre_lockstep_t::secret{}, static_cast<F &&>(this->f), static_cast<G &&>(g)...};
    }
};

constexpr make_pre_lockstep_t lockstep{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F, typename... G>
struct overload_t : overload_t<F>::type, overload_t<G...>::type {
    using type = overload_t;
    using overload_t<F>::type::operator();
    using overload_t<G...>::type::operator();

    template <typename F_, typename... G_>
    constexpr explicit overload_t(F_ &&f, G_ &&...g)
        : overload_t<F>::type(static_cast<F_ &&>(f)), overload_t<G...>::type(static_cast<G_ &&>(g)...)
    {
    }
};

template <typename F>
struct overload_t<F> {
    using type = F;
};

template <typename R, typename... Args>
struct overload_t<R (*)(Args...)> {
    using type = overload_t;
    R (*fptr_)(Args...);

    explicit constexpr overload_t(R (*fp)(Args...)) : fptr_(fp) {}

    constexpr R operator()(Args... args) const
    {
            return fptr_(static_cast<Args &&>(args)...);
    }
};

struct make_overload_t {
    template <typename... F, typename Overload = typename overload_t<typename detail::decay<F>::type...>::type>
    constexpr Overload operator()(F &&...f) const
    {
            return Overload(static_cast<F &&>(f)...);
    }
};

constexpr make_overload_t overload{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F, typename G>
struct overload_linearly_t {
    F f;
    G g;

  private:
    template <typename... Args, typename = decltype(std::declval<F const &>()(std::declval<Args>()...))>
    constexpr F const &which(int) const &
    {
            return f;
    }

    template <typename... Args, typename = decltype(std::declval<F &>()(std::declval<Args>()...))>
    constexpr F &which(int) &
    {
            return f;
    }

    template <typename... Args, typename = decltype(std::declval<F &&>()(std::declval<Args>()...))>
    constexpr F which(int) &&
    {
            return static_cast<F &&>(f);
    }

    template <typename... Args>
    constexpr G const &which(long) const &
    {
            return g;
    }

    template <typename... Args>
    constexpr G &which(long) &
    {
            return g;
    }

    template <typename... Args>
    constexpr G which(long) &&
    {
            return static_cast<G &&>(g);
    }

  public:
    template <typename... Args>
    constexpr decltype(auto) operator()(Args &&...args) const &
    {
            return which<Args...>(int{})(static_cast<Args &&>(args)...);
    }

    template <typename... Args>
    constexpr decltype(auto) operator()(Args &&...args) &
    {
            return which<Args...>(int{})(static_cast<Args &&>(args)...);
    }

    template <typename... Args>
    constexpr decltype(auto) operator()(Args &&...args) &&
    {
            return which<Args...>(int{})(static_cast<Args &&>(args)...);
    }
};

struct make_overload_linearly_t {
    template <typename F, typename G>
    constexpr overload_linearly_t<typename detail::decay<F>::type, typename detail::decay<G>::type> operator()(
        F &&f, G &&g) const
    {
            return {static_cast<F &&>(f), static_cast<G &&>(g)};
    }

    template <typename F, typename G, typename... H>
    constexpr decltype(auto) operator()(F &&f, G &&g, H &&...h) const
    {
            return (*this)(static_cast<F &&>(f), (*this)(static_cast<G &&>(g), static_cast<H &&>(h)...));
    }

    template <typename F>
    constexpr typename detail::decay<F>::type operator()(F &&f) const
    {
            return static_cast<F &&>(f);
    }
};

constexpr make_overload_linearly_t overload_linearly{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct fuse_t {
    template <typename F>
    constexpr auto operator()(F &&f) const;
};

constexpr fuse_t fuse{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename F>
struct fused {
    F f;
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const &
    {
            return hana::unpack(static_cast<Xs &&>(xs), f);
    }

    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) &
    {
            return hana::unpack(static_cast<Xs &&>(xs), f);
    }

    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) &&
    {
            return hana::unpack(static_cast<Xs &&>(xs), static_cast<F &&>(f));
    }
};
} // namespace detail

template <typename F>
constexpr auto fuse_t::operator()(F &&f) const
{
    return detail::fused<typename detail::decay<F>::type>{static_cast<F &&>(f)};
}

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) greater_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Greater = ::std::conditional_t<(hana::Orderable<T>::value && hana::Orderable<U>::value),
                                         decltype(greater_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Orderable<T>::value, "hana::greater(x, y) requires 'x' to be Orderable");

    static_assert(hana::Orderable<U>::value, "hana::greater(x, y) requires 'y' to be Orderable");

    return Greater::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct greater_impl<T, U, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::less(static_cast<Y &&>(y), static_cast<X &&>(x));
    }
};

template <typename T, typename U>
struct greater_impl<T, U, when<detail::has_nontrivial_common_embedding<Orderable, T, U>::value>> {
    using C = typename hana::common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::greater(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) greater_equal_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using GreaterEqual =
        ::std::conditional_t<(hana::Orderable<T>::value && hana::Orderable<U>::value),
                             decltype(greater_equal_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Orderable<T>::value, "hana::greater_equal(x, y) requires 'x' to be Orderable");

    static_assert(hana::Orderable<U>::value, "hana::greater_equal(x, y) requires 'y' to be Orderable");

    return GreaterEqual::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct greater_equal_impl<T, U, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X x, Y y)
    {
            return hana::not_(hana::less(static_cast<X &&>(x), static_cast<Y &&>(y)));
    }
};

template <typename T, typename U>
struct greater_equal_impl<T, U, when<detail::has_nontrivial_common_embedding<Orderable, T, U>::value>> {
    using C = typename hana::common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::greater_equal(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Algorithm>
struct nested_by_t {
    template <typename Predicate, typename Object>
    constexpr decltype(auto) operator()(Predicate &&predicate, Object &&object) const;

    template <typename Predicate>
    constexpr decltype(auto) operator()(Predicate &&predicate) const;
};

template <typename Algorithm>
struct nested_by {
    static constexpr nested_by_t<Algorithm> by{};
};

template <typename Algorithm>
constexpr nested_by_t<Algorithm> nested_by<Algorithm>::by;
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct group_impl : group_impl<S, when<true>> {};

struct group_t : detail::nested_by<group_t> {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;

    template <typename Xs, typename Predicate>
    constexpr auto operator()(Xs &&xs, Predicate &&pred) const;
};

constexpr group_t group{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{

template <typename Algorithm>
template <typename Predicate, typename Object>
constexpr decltype(auto) nested_by_t<Algorithm>::operator()(Predicate &&predicate, Object &&object) const
{
    return Algorithm{}(static_cast<Object &&>(object), static_cast<Predicate &&>(predicate));
}

template <typename Algorithm>
template <typename Predicate>
constexpr decltype(auto) nested_by_t<Algorithm>::operator()(Predicate &&predicate) const
{
    return hana::partial(hana::flip(Algorithm{}), static_cast<Predicate &&>(predicate));
}

} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto group_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Group =
        ::std::conditional_t<(hana::Sequence<S>::value), group_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::group(xs) requires 'xs' to be a Sequence");

    return Group::apply(static_cast<Xs &&>(xs));
}

template <typename Xs, typename Predicate>
constexpr auto group_t::operator()(Xs &&xs, Predicate &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Group =
        ::std::conditional_t<(hana::Sequence<S>::value), group_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::group(xs, predicate) requires 'xs' to be a Sequence");

    return Group::apply(static_cast<Xs &&>(xs), static_cast<Predicate &&>(pred));
}

namespace detail
{
template <typename Xs, std::size_t... i>
constexpr auto get_subsequence_(Xs &&xs, std::index_sequence<i...>)
{
    using S = typename hana::tag_of<Xs>::type;
    return hana::make<S>(hana::at_c<i>(static_cast<Xs &&>(xs))...);
}

template <std::size_t offset, typename Indices>
struct offset_by;

template <std::size_t offset, std::size_t... i>
struct offset_by<offset, std::index_sequence<i...>> {
    using type = std::index_sequence<(offset + i)...>;
};

template <bool... b>
struct group_indices {
    static constexpr bool bs[sizeof...(b)] = {b...};
    static constexpr std::size_t n_groups = detail::count(bs, bs + sizeof(bs), false) + 1;

    static constexpr auto compute_info()
    {
            detail::array<std::size_t, n_groups> sizes{}, offsets{};
            for (std::size_t g = 0, i = 0, offset = 0; g < n_groups; ++g) {
                offsets[g] = offset;

                sizes[g] = 1;
                while (i < sizeof...(b) && bs[i++]) ++sizes[g];

                offset += sizes[g];
            }
            return std::make_pair(offsets, sizes);
    }

    static constexpr auto info = compute_info();
    static constexpr auto group_offsets = info.first;
    static constexpr auto group_sizes = info.second;

    template <typename S, typename Xs, std::size_t... i>
    static constexpr auto finish(Xs &&xs, std::index_sequence<i...>)
    {
            return hana::make<S>(detail::get_subsequence_(
                static_cast<Xs &&>(xs),
                typename offset_by<group_offsets[i], std::make_index_sequence<group_sizes[i]>>::type{})...);
    }
};
} // namespace detail

template <typename S, bool condition>
struct group_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Pred, std::size_t... i>
    static constexpr auto group_helper(Xs &&xs, Pred &&pred, std::index_sequence<0, i...>)
    {
            using info = detail::group_indices<static_cast<bool>(decltype(pred(
                hana::at_c<i - 1>(static_cast<Xs &&>(xs)), hana::at_c<i>(static_cast<Xs &&>(xs))))::value)...>;
            return info::template finish<S>(static_cast<Xs &&>(xs), std::make_index_sequence<info::n_groups>{});
    }

    template <typename Xs, typename Pred>
    static constexpr auto group_helper(Xs &&xs, Pred &&, std::index_sequence<0>)
    {
            return hana::make<S>(static_cast<Xs &&>(xs));
    }

    template <typename Xs, typename Pred>
    static constexpr auto group_helper(Xs &&, Pred &&, std::index_sequence<>)
    {
            return hana::make<S>();
    }

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            return group_helper(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred), std::make_index_sequence<len>{});
    }

    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return group_impl::apply(static_cast<Xs &&>(xs), hana::equal);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct insert_impl : insert_impl<T, when<true>> {};

struct insert_t {
    template <typename Set, typename... Args>
    constexpr decltype(auto) operator()(Set &&set, Args &&...args) const;
};

constexpr insert_t insert{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct take_front_impl : take_front_impl<S, when<true>> {};

struct take_front_t {
    template <typename Xs, typename N>
    constexpr auto operator()(Xs &&xs, N const &n) const;
};

constexpr take_front_t take_front{};

template <std::size_t n>
struct take_front_c_t;

template <std::size_t n>
constexpr take_front_c_t<n> take_front_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr auto take_front_t::operator()(Xs &&xs, N const &n) const
{
    using S = typename hana::tag_of<Xs>::type;
    using TakeFront = ::std::conditional_t<(hana::Sequence<S>::value && hana::IntegralConstant<N>::value),
                                           take_front_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::take_front(xs, n) requires 'xs' to be a Sequence");

    static_assert(hana::IntegralConstant<N>::value, "hana::take_front(xs, n) requires 'n' to be an IntegralConstant");

    return TakeFront::apply(static_cast<Xs &&>(xs), n);
}

template <typename S, bool condition>
struct take_front_impl<S, when<condition>> : default_ {
    template <typename Xs, std::size_t... n>
    static constexpr auto take_front_helper(Xs &&xs, std::index_sequence<n...>)
    {
            return hana::make<S>(hana::at_c<n>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &)
    {
            constexpr std::size_t n = N::value;
            constexpr std::size_t size = decltype(hana::length(xs))::value;
            return take_front_helper(static_cast<Xs &&>(xs), std::make_index_sequence<(n < size ? n : size)>{});
    }
};

template <std::size_t n>
struct take_front_c_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const
    {
            return hana::take_front(static_cast<Xs &&>(xs), hana::size_c<n>);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Set, typename... Args>
constexpr decltype(auto) insert_t::operator()(Set &&set, Args &&...args) const
{
    return insert_impl<typename hana::tag_of<Set>::type>::apply(static_cast<Set &&>(set),
                                                                static_cast<Args &&>(args)...);
}

template <typename T, bool condition>
struct insert_impl<T, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename S>
struct insert_impl<S, when<Sequence<S>::value>> {
    template <typename Xs, typename N, typename Element>
    static constexpr auto apply(Xs &&xs, N const &n, Element &&e)
    {
            return hana::concat(hana::append(hana::take_front(xs, n), static_cast<Element &&>(e)),
                                hana::drop_front(xs, n));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct insert_range_impl : insert_range_impl<S, when<true>> {};

struct insert_range_t {
    template <typename Xs, typename N, typename Elements>
    constexpr auto operator()(Xs &&xs, N &&n, Elements &&elements) const;
};

constexpr insert_range_t insert_range{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N, typename Elements>
constexpr auto insert_range_t::operator()(Xs &&xs, N &&n, Elements &&elements) const
{
    using S = typename hana::tag_of<Xs>::type;
    using InsertRange = ::std::conditional_t<(hana::Sequence<Xs>::value && hana::Foldable<Elements>::value),
                                             insert_range_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<Xs>::value, "hana::insert_range(xs, n, elements) requires 'xs' to be a Sequence");

    static_assert(hana::Foldable<Elements>::value,
                  "hana::insert_range(xs, n, elements) requires 'elements' to be a Foldable");

    return InsertRange::apply(static_cast<Xs &&>(xs), static_cast<N &&>(n), static_cast<Elements &&>(elements));
}

template <typename S, bool condition>
struct insert_range_impl<S, when<condition>> {
    template <typename Xs, typename N, typename Elements>
    static constexpr auto apply(Xs &&xs, N const &n, Elements &&e)
    {
            return hana::concat(hana::concat(hana::take_front(xs, n), hana::to<S>(static_cast<Elements &&>(e))),
                                hana::drop_front(xs, n));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct intersection_impl : intersection_impl<S, when<true>> {};

struct intersection_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs &&, Ys &&) const;
};

constexpr intersection_t intersection{};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto intersection_t::operator()(Xs &&xs, Ys &&ys) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Intersection = ::std::conditional_t<(true), intersection_impl<S>, ::boost::hana::deleted_implementation>;

    return Intersection::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys));
}

template <typename S, bool condition>
struct intersection_impl<S, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct intersperse_impl : intersperse_impl<S, when<true>> {};

struct intersperse_t {
    template <typename Xs, typename Z>
    constexpr auto operator()(Xs &&xs, Z &&z) const;
};

constexpr intersperse_t intersperse{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Z>
constexpr auto intersperse_t::operator()(Xs &&xs, Z &&z) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Intersperse =
        ::std::conditional_t<(hana::Sequence<S>::value), intersperse_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::intersperse(xs, z) requires 'xs' to be a Sequence");

    return Intersperse::apply(static_cast<Xs &&>(xs), static_cast<Z &&>(z));
}

template <typename S, bool condition>
struct intersperse_impl<S, when<condition>> : default_ {
    template <std::size_t i, typename Xs, typename Z>
    static constexpr decltype(auto) pick(Xs &&, Z &&z, hana::false_)
    {
            return static_cast<Z &&>(z);
    }

    template <std::size_t i, typename Xs, typename Z>
    static constexpr decltype(auto) pick(Xs &&xs, Z &&, hana::true_)
    {
            return hana::at_c<(i + 1) / 2>(static_cast<Xs &&>(xs));
    }

    template <typename Xs, typename Z, std::size_t... i>
    static constexpr auto intersperse_helper(Xs &&xs, Z &&z, std::index_sequence<i...>)
    {
            return hana::make<S>(pick<i>(static_cast<Xs &&>(xs), static_cast<Z &&>(z), hana::bool_c<(i % 2 == 0)>)...);
    }

    template <typename Xs, typename Z>
    static constexpr auto apply(Xs &&xs, Z &&z)
    {
            constexpr std::size_t size = decltype(hana::length(xs))::value;
            constexpr std::size_t new_size = size == 0 ? 0 : (size * 2) - 1;
            return intersperse_helper(static_cast<Xs &&>(xs), static_cast<Z &&>(z),
                                      std::make_index_sequence<new_size>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S1, typename S2, typename = void>
struct is_disjoint_impl : is_disjoint_impl<S1, S2, when<true>> {};

struct is_disjoint_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs &&xs, Ys &&ys) const;
};

constexpr is_disjoint_t is_disjoint{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct none_of_impl : none_of_impl<S, when<true>> {};

struct none_of_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr none_of_t none_of{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto none_of_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using NoneOf =
        ::std::conditional_t<(hana::Searchable<S>::value), none_of_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::none_of(xs, pred) requires 'xs' to be a Searchable");

    return NoneOf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

template <typename S, bool condition>
struct none_of_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            return hana::not_(hana::any_of(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto is_disjoint_t::operator()(Xs &&xs, Ys &&ys) const
{
    using S1 = typename hana::tag_of<Xs>::type;
    using S2 = typename hana::tag_of<Ys>::type;
    using IsDisjoint =
        ::std::conditional_t<(hana::Searchable<S1>::value && hana::Searchable<S2>::value),
                             decltype(is_disjoint_impl<S1, S2>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S1>::value, "hana::is_disjoint(xs, ys) requires 'xs' to be Searchable");

    static_assert(hana::Searchable<S2>::value, "hana::is_disjoint(xs, ys) requires 'ys' to be Searchable");

    return IsDisjoint::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys));
}

namespace detail
{
template <typename Ys>
struct in_by_reference {
    Ys const &ys;
    template <typename X>
    constexpr auto operator()(X const &x) const
    {
            return hana::contains(ys, x);
    }
};
} // namespace detail

template <typename S1, typename S2, bool condition>
struct is_disjoint_impl<S1, S2, when<condition>> : default_ {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs const &xs, Ys const &ys)
    {
            return hana::none_of(xs, detail::in_by_reference<Ys>{ys});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S1, typename S2, typename = void>
struct is_subset_impl : is_subset_impl<S1, S2, when<true>> {};

struct is_subset_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs &&xs, Ys &&ys) const;
};

constexpr auto is_subset = hana::infix(is_subset_t{});

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto is_subset_t::operator()(Xs &&xs, Ys &&ys) const
{
    using S1 = typename hana::tag_of<Xs>::type;
    using S2 = typename hana::tag_of<Ys>::type;
    using IsSubset = ::std::conditional_t<(hana::Searchable<S1>::value && hana::Searchable<S2>::value
                                           && !is_default<is_subset_impl<S1, S2>>::value),
                                          decltype(is_subset_impl<S1, S2>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S1>::value, "hana::is_subset(xs, ys) requires 'xs' to be Searchable");

    static_assert(hana::Searchable<S2>::value, "hana::is_subset(xs, ys) requires 'ys' to be Searchable");

    static_assert(!is_default<is_subset_impl<S1, S2>>::value,
                  "hana::is_subset(xs, ys) requires 'xs' and 'ys' to be embeddable "
                  "in a common Searchable");

    return IsSubset::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys));
}

template <typename S1, typename S2, bool condition>
struct is_subset_impl<S1, S2, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename S, bool condition>
struct is_subset_impl<S, S, when<condition>> {
    template <typename Xs, typename Ys>
    static constexpr decltype(auto) apply(Xs &&xs, Ys &&ys)
    {
            return hana::all_of(static_cast<Xs &&>(xs), hana::partial(hana::contains, static_cast<Ys &&>(ys)));
    }
};

template <typename S1, typename S2>
struct is_subset_impl<S1, S2, when<detail::has_nontrivial_common_embedding<Searchable, S1, S2>::value>> {
    using C = typename common<S1, S2>::type;
    template <typename Xs, typename Ys>
    static constexpr decltype(auto) apply(Xs &&xs, Ys &&ys)
    {
            return hana::is_subset(hana::to<C>(static_cast<Xs &&>(xs)), hana::to<C>(static_cast<Ys &&>(ys)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct keys_impl : keys_impl<T, when<true>> {};

struct keys_t {
    template <typename Map>
    constexpr auto operator()(Map &&map) const;
};

constexpr keys_t keys{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Map>
constexpr auto keys_t::operator()(Map &&map) const
{
    return keys_impl<typename hana::tag_of<Map>::type>::apply(static_cast<Map &&>(map));
}

template <typename T, bool condition>
struct keys_impl<T, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};

template <typename S>
struct keys_impl<S, when<hana::Struct<S>::value>> {
    template <typename Object>
    static constexpr auto apply(Object const &)
    {
            return hana::transform(hana::accessors<S>(), hana::first);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct map_tag {};

namespace detail
{
template <typename... Pairs>
struct make_map_type;
}

template <typename... Pairs>
using map = typename detail::make_map_type<Pairs...>::type;

constexpr auto make_map = make<map_tag>;

constexpr auto to_map = to<map_tag>;

struct values_t {
    template <typename Map>
    constexpr decltype(auto) operator()(Map &&map) const;
};

constexpr values_t values{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename T, typename... U>
constexpr std::size_t pack_count()
{
    std::size_t c = 0;
    std::size_t expand[] = {0, (decltype(hana::equal(std::declval<T>(), std::declval<U>()))::value ? ++c : c)...};
    (void)expand;

    return c;
}

template <typename... T>
struct has_duplicates {
    static constexpr bool value = sizeof...(T) > 0 && !detail::fast_and<(detail::pack_count<T, T...>() == 1)...>::value;
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace ext
{
namespace std
{
template <typename T>
struct integral_constant_tag {
    using value_type = T;
};
} // namespace std
} // namespace ext

namespace detail
{
template <typename T, T v>
constexpr bool is_std_integral_constant(std::integral_constant<T, v> *)
{
    return true;
}

constexpr bool is_std_integral_constant(...)
{
    return false;
}

template <typename T, T v>
constexpr bool is_hana_integral_constant(hana::integral_constant<T, v> *)
{
    return true;
}

constexpr bool is_hana_integral_constant(...)
{
    return false;
}
} // namespace detail

template <typename T>
struct tag_of<T, when<detail::is_std_integral_constant((T *)0) && !detail::is_hana_integral_constant((T *)0)>> {
    using type = ext::std::integral_constant_tag<typename hana::tag_of<typename T::value_type>::type>;
};

template <typename T>
struct IntegralConstant<ext::std::integral_constant_tag<T>> {
    static constexpr bool value = true;
};

template <typename T, typename C>
struct to_impl<ext::std::integral_constant_tag<T>, C, when<hana::IntegralConstant<C>::value>>
    : embedding<is_embedded<typename C::value_type, T>::value> {
    template <typename N>
    static constexpr auto apply(N const &)
    {
            return std::integral_constant<T, N::value>{};
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace ext
{
namespace std
{
struct integer_sequence_tag;
}
} // namespace ext

template <typename T, T... v>
struct tag_of<std::integer_sequence<T, v...>> {
    using type = ext::std::integer_sequence_tag;
};

template <>
struct equal_impl<ext::std::integer_sequence_tag, ext::std::integer_sequence_tag> {
    template <typename X, X... xs, typename Y, Y... ys>
    static constexpr hana::bool_<detail::fast_and<(xs == ys)...>::value> apply(std::integer_sequence<X, xs...> const &,
                                                                               std::integer_sequence<Y, ys...> const &)
    {
            return {};
    }

    template <typename Xs, typename Ys>
    static constexpr hana::false_ apply(Xs const &, Ys const &, ...)
    {
            return {};
    }
};

template <>
struct unpack_impl<ext::std::integer_sequence_tag> {
    template <typename T, T... v, typename F>
    static constexpr decltype(auto) apply(std::integer_sequence<T, v...> const &, F &&f)
    {
            return static_cast<F &&>(f)(std::integral_constant<T, v>{}...);
    }
};

template <>
struct at_impl<ext::std::integer_sequence_tag> {
    template <typename T, T... v, typename N>
    static constexpr auto apply(std::integer_sequence<T, v...> const &, N const &)
    {
            constexpr std::size_t n = N::value;
            constexpr T values_[] = {v...};
            return std::integral_constant<T, values_[n]>{};
    }
};

template <>
struct drop_front_impl<ext::std::integer_sequence_tag> {
    template <std::size_t n, typename T, T... t, std::size_t... i>
    static constexpr auto drop_front_helper(std::integer_sequence<T, t...>, std::index_sequence<i...>)
    {
            constexpr T ts[sizeof...(t) + 1] = {t...};
            return std::integer_sequence<T, ts[n + i]...>{};
    }

    template <typename T, T... t, typename N>
    static constexpr auto apply(std::integer_sequence<T, t...> ts, N const &)
    {
            constexpr std::size_t n = N::value;
            constexpr std::size_t len = sizeof...(t);
            return drop_front_helper<n>(ts, std::make_index_sequence<(n < len ? len - n : 0)>{});
    }
};

template <>
struct is_empty_impl<ext::std::integer_sequence_tag> {
    template <typename T, T... xs>
    static constexpr auto apply(std::integer_sequence<T, xs...> const &)
    {
            return hana::bool_c<sizeof...(xs) == 0>;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, T from, T to>
struct range;

struct range_tag {};

constexpr auto make_range = make<range_tag>;

template <typename T, T from, T to>
constexpr range<T, from, to> range_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct maximum_impl : maximum_impl<T, when<true>> {};

template <typename T, typename = void>
struct maximum_pred_impl : maximum_pred_impl<T, when<true>> {};

struct maximum_t : detail::nested_by<maximum_t> {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const;

    template <typename Xs, typename Predicate>
    constexpr decltype(auto) operator()(Xs &&xs, Predicate &&pred) const;
};

constexpr maximum_t maximum{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct minimum_impl : minimum_impl<T, when<true>> {};

template <typename T, typename = void>
struct minimum_pred_impl : minimum_pred_impl<T, when<true>> {};

struct minimum_t : detail::nested_by<minimum_t> {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const;

    template <typename Xs, typename Predicate>
    constexpr decltype(auto) operator()(Xs &&xs, Predicate &&pred) const;
};

constexpr minimum_t minimum{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct product_impl : product_impl<T, when<true>> {};

template <typename R>
struct product_t {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const;
};

template <typename R = integral_constant_tag<int>>
constexpr product_t<R> product{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct sum_impl : sum_impl<T, when<true>> {};

template <typename M>
struct sum_t {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const;
};

template <typename M = integral_constant_tag<int>>
constexpr sum_t<M> sum{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, T From, T To>
struct range : detail::operators::adl<range<T, From, To>>, detail::iterable_operators<range<T, From, To>> {
    static_assert(From <= To, "hana::make_range(from, to) requires 'from <= to'");

    using value_type = T;
    static constexpr value_type from = From;
    static constexpr value_type to = To;
};

template <typename T, T From, T To>
struct tag_of<range<T, From, To>> {
    using type = range_tag;
};

template <>
struct make_impl<range_tag> {
    template <typename From, typename To>
    static constexpr auto apply(From const &, To const &)
    {
            static_assert(hana::IntegralConstant<From>::value,
                          "hana::make_range(from, to) requires 'from' to be an IntegralConstant");

            static_assert(hana::IntegralConstant<To>::value,
                          "hana::make_range(from, to) requires 'to' to be an IntegralConstant");

            using T = typename common<typename hana::tag_of<From>::type::value_type,
                                      typename hana::tag_of<To>::type::value_type>::type;
            constexpr T from = hana::to<T>(From::value);
            constexpr T to = hana::to<T>(To::value);
            return range<T, from, to>{};
    }
};

namespace detail
{
template <>
struct comparable_operators<range_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <>
struct equal_impl<range_tag, range_tag> {
    template <typename R1, typename R2>
    static constexpr auto apply(R1 const &, R2 const &)
    {
            return hana::bool_c < (R1::from == R1::to && R2::from == R2::to)
                   || (R1::from == R2::from && R1::to == R2::to) > ;
    }
};

template <>
struct unpack_impl<range_tag> {
    template <typename T, T from, typename F, T... v>
    static constexpr decltype(auto) unpack_helper(F &&f, std::integer_sequence<T, v...>)
    {
            return static_cast<F &&>(f)(integral_constant<T, from + v>{}...);
    }

    template <typename T, T from, T to, typename F>
    static constexpr decltype(auto) apply(range<T, from, to> const &, F &&f)
    {
            return unpack_helper<T, from>(static_cast<F &&>(f), std::make_integer_sequence<T, to - from>{});
    }
};

template <>
struct length_impl<range_tag> {
    template <typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return hana::size_c<static_cast<std::size_t>(to - from)>;
    }
};

template <>
struct minimum_impl<range_tag> {
    template <typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return integral_c<T, from>;
    }
};

template <>
struct maximum_impl<range_tag> {
    template <typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return integral_c<T, to - 1>;
    }
};

template <>
struct sum_impl<range_tag> {
    template <typename I>
    static constexpr I sum_helper(I m, I n)
    {
            if (m == n)
                return m;

            else if (0 == m)
                return n * (n + 1) / 2;

            else if (0 < m)
                return sum_helper(0, n) - sum_helper(0, m - 1);

            else if (0 <= n)
                return sum_helper(0, n) - sum_helper(0, -m);

            else
                return -sum_helper(-n, -m);
    }

    template <typename, typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return integral_c < T, from == to ? 0 : sum_helper(from, to - 1) > ;
    }
};

template <>
struct product_impl<range_tag> {
    template <typename I>
    static constexpr I product_helper(I m, I n)
    {
            if (m <= 0 && 0 < n)
                return 0;
            else {
                I p = 1;
                for (; m != n; ++m) p *= m;
                return p;
            }
    }

    template <typename, typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return integral_c<T, product_helper(from, to)>;
    }
};

template <>
struct find_impl<range_tag> {
    template <typename T, T from, typename N>
    static constexpr auto find_helper(hana::true_)
    {
            constexpr T n = N::value;
            return hana::just(hana::integral_c<T, n>);
    }

    template <typename T, T from, typename N>
    static constexpr auto find_helper(hana::false_)
    {
            return hana::nothing;
    }

    template <typename T, T from, T to, typename N>
    static constexpr auto apply(range<T, from, to> const &, N const &)
    {
            constexpr auto n = N::value;
            return find_helper<T, from, N>(hana::bool_c<(n >= from && n < to)>);
    }
};

template <>
struct contains_impl<range_tag> {
    template <typename T, T from, T to, typename N>
    static constexpr auto apply(range<T, from, to> const &, N const &)
    {
            constexpr auto n = N::value;
            return bool_c<(n >= from && n < to)>;
    }
};

template <>
struct front_impl<range_tag> {
    template <typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return integral_c<T, from>;
    }
};

template <>
struct is_empty_impl<range_tag> {
    template <typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return bool_c<from == to>;
    }
};

template <>
struct at_impl<range_tag> {
    template <typename T, T from, T to, typename N>
    static constexpr auto apply(range<T, from, to> const &, N const &)
    {
            constexpr auto n = N::value;
            return integral_c<T, from + n>;
    }
};

template <>
struct back_impl<range_tag> {
    template <typename T, T from, T to>
    static constexpr auto apply(range<T, from, to> const &)
    {
            return integral_c<T, to - 1>;
    }
};

template <>
struct drop_front_impl<range_tag> {
    template <typename T, T from, T to, typename N>
    static constexpr auto apply(range<T, from, to> const &, N const &)
    {
            constexpr auto n = N::value;
            return range<T, (to < from + n ? to : from + n), to>{};
    }
};

template <>
struct drop_front_exactly_impl<range_tag> {
    template <typename T, T from, T to, typename N>
    static constexpr auto apply(range<T, from, to> const &, N const &)
    {
            constexpr auto n = N::value;
            return range<T, from + n, to>{};
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Hash, std::size_t... i>
struct bucket {};

template <typename... Buckets>
struct hash_table : Buckets... {};

template <typename Hash, std::size_t... i>
std::index_sequence<i...> find_indices_impl(bucket<Hash, i...> const &);

template <typename Hash>
std::index_sequence<> find_indices_impl(...);

template <typename Map, typename Key>
struct find_indices {
    using Hash = typename decltype(hana::hash(std::declval<Key>()))::type;
    using type = decltype(detail::find_indices_impl<Hash>(std::declval<Map>()));
};

template <template <std::size_t> class KeyAtIndex, typename Key>
struct find_pred {
    template <typename Index>
    auto operator()(Index const &) const
        -> decltype(hana::equal(std::declval<KeyAtIndex<Index::value>>(), std::declval<Key>()));
};

template <typename Indices, typename Key, template <std::size_t> class KeyAtIndex>
struct find_index_impl {
    using type = decltype(hana::find_if(Indices{}, find_pred<KeyAtIndex, Key>{}));
};

template <std::size_t i, typename Key, template <std::size_t> class KeyAtIndex>
struct find_index_impl<std::index_sequence<i>, Key, KeyAtIndex> {
    using Equal = decltype(hana::equal(std::declval<KeyAtIndex<i>>(), std::declval<Key>()));
    using type = typename std::conditional<Equal::value, hana::optional<std::integral_constant<std::size_t, i>>,
                                           hana::optional<>>::type;
};

template <typename Map, typename Key, template <std::size_t> class KeyAtIndex>
struct find_index {
    using Indices = typename find_indices<Map, Key>::type;
    using type = typename find_index_impl<Indices, Key, KeyAtIndex>::type;
};

template <typename Bucket, typename Hash, std::size_t Index>
struct update_bucket {
    using type = Bucket;
};

template <std::size_t... i, typename Hash, std::size_t Index>
struct update_bucket<bucket<Hash, i...>, Hash, Index> {
    using type = bucket<Hash, i..., Index>;
};

template <typename Map, typename Key, std::size_t Index, bool = (find_indices<Map, Key>::type::size() > 0)>
struct bucket_insert;

template <typename... Buckets, typename Key, std::size_t Index>
struct bucket_insert<hash_table<Buckets...>, Key, Index, true> {
    using Hash = typename decltype(hana::hash(std::declval<Key>()))::type;
    using type = hash_table<typename update_bucket<Buckets, Hash, Index>::type...>;
};

template <typename... Buckets, typename Key, std::size_t Index>
struct bucket_insert<hash_table<Buckets...>, Key, Index, false> {
    using Hash = typename decltype(hana::hash(std::declval<Key>()))::type;
    using type = hash_table<Buckets..., bucket<Hash, Index>>;
};

template <template <std::size_t> class KeyAtIndex, std::size_t N, typename Indices = std::make_index_sequence<N>>
struct make_hash_table;

template <template <std::size_t> class KeyAtIndex, std::size_t N, std::size_t... i>
struct make_hash_table<KeyAtIndex, N, std::index_sequence<i...>> {
    using type = hash_table<bucket<typename decltype(hana::hash(std::declval<KeyAtIndex<i>>()))::type, i>...>;
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename Derived>
struct searchable_operators {
    template <typename Key>
    constexpr decltype(auto) operator[](Key &&key) &
    {
            return hana::at_key(static_cast<Derived &>(*this), static_cast<Key &&>(key));
    }

    template <typename Key>
    constexpr decltype(auto) operator[](Key &&key) &&
    {
            return hana::at_key(static_cast<Derived &&>(*this), static_cast<Key &&>(key));
    }

    template <typename Key>
    constexpr decltype(auto) operator[](Key &&key) const &
    {
            return hana::at_key(static_cast<Derived const &>(*this), static_cast<Key &&>(key));
    }
};
} // namespace detail
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct union_impl : union_impl<T, when<true>> {};

struct union_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs &&, Ys &&) const;
};

constexpr union_t union_{};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct remove_if_impl : remove_if_impl<M, when<true>> {};

struct remove_if_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr remove_if_t remove_if{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto remove_if_t::operator()(Xs &&xs, Pred &&pred) const
{
    using M = typename hana::tag_of<Xs>::type;
    using RemoveIf =
        ::std::conditional_t<(hana::MonadPlus<M>::value), remove_if_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::remove_if(xs, predicate) requires 'xs' to be a MonadPlus");

    return RemoveIf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

template <typename M, bool condition>
struct remove_if_impl<M, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            return hana::filter(static_cast<Xs &&>(xs), hana::compose(hana::not_, static_cast<Pred &&>(pred)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

namespace detail
{
template <>
struct comparable_operators<map_tag> {
    static constexpr bool value = true;
};
} // namespace detail

namespace detail
{
template <typename...>
struct storage_is_default_constructible;
template <typename... T>
struct storage_is_default_constructible<hana::basic_tuple<T...>> {
    static constexpr bool value = detail::fast_and<::std::is_constructible<T>::value...>::value;
};

template <typename...>
struct storage_is_copy_constructible;
template <typename... T>
struct storage_is_copy_constructible<hana::basic_tuple<T...>> {
    static constexpr bool value = detail::fast_and<::std::is_constructible<T, T const &>::value...>::value;
};

template <typename...>
struct storage_is_move_constructible;
template <typename... T>
struct storage_is_move_constructible<hana::basic_tuple<T...>> {
    static constexpr bool value = detail::fast_and<::std::is_constructible<T, T &&>::value...>::value;
};

template <typename...>
struct storage_is_copy_assignable;
template <typename... T>
struct storage_is_copy_assignable<hana::basic_tuple<T...>> {
    static constexpr bool value = detail::fast_and<::std::is_assignable<T, T const &>::value...>::value;
};

template <typename...>
struct storage_is_move_assignable;
template <typename... T>
struct storage_is_move_assignable<hana::basic_tuple<T...>> {
    static constexpr bool value = detail::fast_and<::std::is_assignable<T, T &&>::value...>::value;
};

template <typename HashTable, typename Storage>
struct map_impl final : detail::searchable_operators<map_impl<HashTable, Storage>>,
                        detail::operators::adl<map_impl<HashTable, Storage>> {
    using hash_table_type = HashTable;
    using storage_type = Storage;

    Storage storage;

    using hana_tag = map_tag;

    template <typename... P, typename = typename std::enable_if<std::is_same<
                                 Storage, hana::basic_tuple<typename detail::decay<P>::type...>>::value>::type>
    explicit constexpr map_impl(P &&...pairs) : storage{static_cast<P &&>(pairs)...}
    {
    }

    explicit constexpr map_impl(Storage &&xs) : storage(static_cast<Storage &&>(xs)) {}

    template <typename... Dummy, typename = typename std::enable_if<
                                     detail::storage_is_default_constructible<Storage, Dummy...>::value>::type>
    constexpr map_impl() : storage()
    {
    }

    template <typename... Dummy,
              typename = typename std::enable_if<detail::storage_is_copy_constructible<Storage, Dummy...>::value>::type>
    constexpr map_impl(map_impl const &other) : storage(other.storage)
    {
    }

    template <typename... Dummy,
              typename = typename std::enable_if<detail::storage_is_move_constructible<Storage, Dummy...>::value>::type>
    constexpr map_impl(map_impl &&other) : storage(static_cast<Storage &&>(other.storage))
    {
    }

    template <typename... Dummy,
              typename = typename std::enable_if<detail::storage_is_move_assignable<Storage, Dummy...>::value>::type>
    constexpr map_impl &operator=(map_impl &&other)
    {
            storage = static_cast<Storage &&>(other.storage);
            return *this;
    }

    template <typename... Dummy,
              typename = typename std::enable_if<detail::storage_is_copy_assignable<Storage, Dummy...>::value>::type>
    constexpr map_impl &operator=(map_impl const &other)
    {
            storage = other.storage;
            return *this;
    }

    ~map_impl() = default;
};

template <typename Storage>
struct KeyAtIndex {
    template <std::size_t i>
    using apply = decltype(hana::first(hana::at_c<i>(std::declval<Storage>())));
};

template <typename... Pairs>
struct make_map_type {
    using Storage = hana::basic_tuple<Pairs...>;
    using HashTable =
        typename detail::make_hash_table<detail::KeyAtIndex<Storage>::template apply, sizeof...(Pairs)>::type;
    using type = detail::map_impl<HashTable, Storage>;
};
} // namespace detail

template <>
struct make_impl<map_tag> {
    template <typename... Pairs>
    static constexpr auto apply(Pairs &&...pairs)
    {
            using Map = typename detail::make_map_type<typename detail::decay<Pairs>::type...>::type;
            return Map{hana::make_basic_tuple(static_cast<Pairs &&>(pairs)...)};
    }
};

template <>
struct keys_impl<map_tag> {
    template <typename Map>
    static constexpr decltype(auto) apply(Map &&map)
    {
            return hana::transform(static_cast<Map &&>(map).storage, hana::first);
    }
};

template <typename Map>
constexpr decltype(auto) values_t::operator()(Map &&map) const
{
    return hana::transform(static_cast<Map &&>(map).storage, hana::second);
}

template <>
struct insert_impl<map_tag> {
    template <typename Map, typename Pair>
    static constexpr auto helper(Map &&map, Pair &&pair, ...)
    {
            using RawMap = typename std::remove_reference<Map>::type;
            using HashTable = typename RawMap::hash_table_type;
            using NewHashTable = typename detail::bucket_insert<HashTable, decltype(hana::first(pair)),
                                                                decltype(hana::length(map.storage))::value>::type;

            using NewStorage = decltype(hana::append(static_cast<Map &&>(map).storage, static_cast<Pair &&>(pair)));
            return detail::map_impl<NewHashTable, NewStorage>(
                hana::append(static_cast<Map &&>(map).storage, static_cast<Pair &&>(pair)));
    }

    template <typename Map, typename Pair, std::size_t i>
    static constexpr auto helper(Map &&map, Pair &&, hana::optional<std::integral_constant<std::size_t, i>>)
    {
            return static_cast<Map &&>(map);
    }

    template <typename Map, typename Pair>
    static constexpr auto apply(Map &&map, Pair &&pair)
    {
            using RawMap = typename std::remove_reference<Map>::type;
            using Storage = typename RawMap::storage_type;
            using HashTable = typename RawMap::hash_table_type;
            using Key = decltype(hana::first(pair));
            using MaybeIndex =
                typename detail::find_index<HashTable, Key, detail::KeyAtIndex<Storage>::template apply>::type;
            return helper(static_cast<Map &&>(map), static_cast<Pair &&>(pair), MaybeIndex{});
    }
};

template <>
struct erase_key_impl<map_tag> {
    template <typename Map, typename Key>
    static constexpr auto erase_key_helper(Map &&map, Key const &, hana::false_)
    {
            return static_cast<Map &&>(map);
    }

    template <typename Map, typename Key>
    static constexpr auto erase_key_helper(Map &&map, Key const &key, hana::true_)
    {
            return hana::unpack(
                hana::remove_if(static_cast<Map &&>(map).storage, hana::on(hana::equal.to(key), hana::first)),
                hana::make_map);
    }

    template <typename Map, typename Key>
    static constexpr auto apply_impl(Map &&map, Key const &key, hana::false_)
    {
            return erase_key_helper(static_cast<Map &&>(map), key, hana::contains(map, key));
    }

    template <typename Map, typename Key>
    static constexpr auto apply_impl(Map &&map, Key const &, hana::true_)
    {
            return static_cast<Map &&>(map);
    }

    template <typename Map, typename Key>
    static constexpr auto apply(Map &&map, Key const &key)
    {
            constexpr bool is_empty = decltype(hana::length(map))::value == 0;
            return apply_impl(static_cast<Map &&>(map), key, hana::bool_<is_empty>{});
    }
};

template <>
struct equal_impl<map_tag, map_tag> {
    template <typename M1, typename M2>
    static constexpr auto equal_helper(M1 const &, M2 const &, hana::false_)
    {
            return hana::false_c;
    }

    template <typename M1, typename M2>
    static constexpr auto equal_helper(M1 const &m1, M2 const &m2, hana::true_)
    {
            return hana::all_of(hana::keys(m1),
                                hana::demux(equal)(hana::partial(hana::find, m1), hana::partial(hana::find, m2)));
    }

    template <typename M1, typename M2>
    static constexpr auto apply(M1 const &m1, M2 const &m2)
    {
            return equal_impl::equal_helper(
                m1, m2,
                hana::bool_c<decltype(hana::length(m1.storage))::value == decltype(hana::length(m2.storage))::value>);
    }
};

template <>
struct find_impl<map_tag> {
    template <typename Map>
    static constexpr auto find_helper(Map &&, ...)
    {
            return hana::nothing;
    }

    template <typename Map, std::size_t i>
    static constexpr auto find_helper(Map &&map, hana::optional<std::integral_constant<std::size_t, i>>)
    {
            return hana::just(hana::second(hana::at_c<i>(static_cast<Map &&>(map).storage)));
    }

    template <typename Map, typename Key>
    static constexpr auto apply(Map &&map, Key const &)
    {
            using RawMap = typename std::remove_reference<Map>::type;
            using Storage = typename RawMap::storage_type;
            using HashTable = typename RawMap::hash_table_type;
            using MaybeIndex =
                typename detail::find_index<HashTable, Key, detail::KeyAtIndex<Storage>::template apply>::type;
            return find_helper(static_cast<Map &&>(map), MaybeIndex{});
    }
};

template <>
struct find_if_impl<map_tag> {
    template <typename M, typename Pred>
    static constexpr auto apply(M &&map, Pred &&pred)
    {
            return hana::transform(
                hana::find_if(static_cast<M &&>(map).storage, hana::compose(static_cast<Pred &&>(pred), hana::first)),
                hana::second);
    }
};

template <>
struct contains_impl<map_tag> {
    template <typename Map, typename Key>
    static constexpr auto apply(Map const &, Key const &)
    {
            using RawMap = typename std::remove_reference<Map>::type;
            using HashTable = typename RawMap::hash_table_type;
            using Storage = typename RawMap::storage_type;
            using MaybeIndex =
                typename detail::find_index<HashTable, Key, detail::KeyAtIndex<Storage>::template apply>::type;
            return hana::bool_<!decltype(hana::is_nothing(MaybeIndex{}))::value>{};
    }
};

template <>
struct any_of_impl<map_tag> {
    template <typename M, typename Pred>
    static constexpr auto apply(M const &map, Pred const &pred)
    {
            return hana::any_of(hana::keys(map), pred);
    }
};

template <>
struct is_subset_impl<map_tag, map_tag> {
    template <typename Ys>
    struct all_contained {
            Ys const &ys;
            template <typename... X>
            constexpr auto operator()(X const &...x) const
            {
                return hana::bool_c<detail::fast_and<hana::value<decltype(hana::contains(ys, x))>()...>::value>;
            }
    };

    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs const &xs, Ys const &ys)
    {
            auto ys_keys = hana::keys(ys);
            return hana::unpack(hana::keys(xs), all_contained<decltype(ys_keys)>{ys_keys});
    }
};

template <>
struct at_key_impl<map_tag> {
    template <typename Map, typename Key>
    static constexpr decltype(auto) apply(Map &&map, Key const &)
    {
            using RawMap = typename std::remove_reference<Map>::type;
            using HashTable = typename RawMap::hash_table_type;
            using Storage = typename RawMap::storage_type;
            using MaybeIndex =
                typename detail::find_index<HashTable, Key, detail::KeyAtIndex<Storage>::template apply>::type;
            static_assert(!decltype(hana::is_nothing(MaybeIndex{}))::value,
                          "hana::at_key(map, key) requires the 'key' to be present in the 'map'");
            constexpr std::size_t index = decltype(*MaybeIndex{}){}();
            return hana::second(hana::at_c<index>(static_cast<Map &&>(map).storage));
    }
};

template <>
struct union_impl<map_tag> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys &&ys)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys), hana::insert);
    }
};

namespace detail
{
template <typename Ys>
struct map_insert_if_contains {
    Ys const &ys;

    template <typename Result, typename Pair>
    static constexpr auto helper(Result &&result, Pair &&pair, hana::true_)
    {
            return hana::insert(static_cast<Result &&>(result), static_cast<Pair &&>(pair));
    }

    template <typename Result, typename Pair>
    static constexpr auto helper(Result &&result, Pair &&, hana::false_)
    {
            return static_cast<Result &&>(result);
    }

    template <typename Result, typename Pair>
    constexpr auto operator()(Result &&result, Pair &&pair) const
    {
            constexpr bool keep = hana::value<decltype(hana::contains(ys, hana::first(pair)))>();
            return map_insert_if_contains::helper(static_cast<Result &&>(result), static_cast<Pair &&>(pair),
                                                  hana::bool_c<keep>);
    }
};
} // namespace detail

template <>
struct intersection_impl<map_tag> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys const &ys)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), hana::make_map(), detail::map_insert_if_contains<Ys>{ys});
    }
};

template <>
struct difference_impl<map_tag> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys &&ys)
    {
            return hana::fold_left(hana::keys(static_cast<Ys &&>(ys)), static_cast<Xs &&>(xs), hana::erase_key);
    }
};

template <>
struct unpack_impl<map_tag> {
    template <typename M, typename F>
    static constexpr decltype(auto) apply(M &&map, F &&f)
    {
            return hana::unpack(static_cast<M &&>(map).storage, static_cast<F &&>(f));
    }
};

template <typename F>
struct to_impl<map_tag, F, when<hana::Foldable<F>::value>> {
    template <typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), hana::make_map(), hana::insert);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct max_impl : max_impl<T, U, when<true>> {};

struct max_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr max_t max{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) max_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Max = ::std::conditional_t<(hana::Orderable<T>::value && hana::Orderable<U>::value),
                                     decltype(max_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Orderable<T>::value, "hana::max(x, y) requires 'x' to be Orderable");

    static_assert(hana::Orderable<U>::value, "hana::max(x, y) requires 'y' to be Orderable");

    return Max::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct max_impl<T, U, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            decltype(auto) cond = hana::less(x, y);
            return hana::if_(static_cast<decltype(cond) &&>(cond), static_cast<Y &&>(y), static_cast<X &&>(x));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr decltype(auto) maximum_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Maximum =
        ::std::conditional_t<(hana::Foldable<S>::value), maximum_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::maximum(xs) requires 'xs' to be Foldable");

    return Maximum::apply(static_cast<Xs &&>(xs));
}

template <typename Xs, typename Predicate>
constexpr decltype(auto) maximum_t::operator()(Xs &&xs, Predicate &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Maximum =
        ::std::conditional_t<(hana::Foldable<S>::value), maximum_pred_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::maximum(xs, predicate) requires 'xs' to be Foldable");

    return Maximum::apply(static_cast<Xs &&>(xs), static_cast<Predicate &&>(pred));
}

namespace detail
{
template <typename Pred>
struct max_by {
    Pred pred;

    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const
    {
            auto result = (*pred)(x, y);
            return hana::if_(result, static_cast<Y &&>(y), static_cast<X &&>(x));
    }
};
} // namespace detail

template <typename T, bool condition>
struct maximum_pred_impl<T, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr decltype(auto) apply(Xs &&xs, Pred &&pred)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), detail::max_by<decltype(&pred)>{&pred});
    }
};

template <typename T, bool condition>
struct maximum_impl<T, when<condition>> : default_ {
    template <typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::maximum(static_cast<Xs &&>(xs), hana::less);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct members_impl : members_impl<S, when<true>> {};

struct members_t {
    template <typename Object>
    constexpr auto operator()(Object &&object) const;
};

constexpr members_t members{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Object>
constexpr auto members_t::operator()(Object &&object) const
{
    using S = typename hana::tag_of<Object>::type;
    using Members =
        ::std::conditional_t<(hana::Struct<S>::value), members_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Struct<S>::value, "hana::members(object) requires 'object' to be a Struct");

    return Members::apply(static_cast<Object &&>(object));
}

namespace struct_detail
{
template <typename Holder, typename Forward>
struct members_helper {
    Holder object;
    template <typename Accessor>
    constexpr decltype(auto) operator()(Accessor &&accessor) const
    {
            return hana::second(static_cast<Accessor &&>(accessor))(static_cast<Forward>(object));
    }
};
} // namespace struct_detail

template <typename S, bool condition>
struct members_impl<S, when<condition>> : default_ {
    template <typename Object>
    static constexpr auto apply(Object &&object)
    {
            return hana::transform(hana::accessors<S>(), struct_detail::members_helper<Object &, Object &&>{object});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename U, typename = void>
struct min_impl : min_impl<T, U, when<true>> {};

struct min_t {
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const;
};

constexpr min_t min{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr decltype(auto) min_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using Min = ::std::conditional_t<(hana::Orderable<T>::value && hana::Orderable<U>::value),
                                     decltype(min_impl<T, U>{}), ::boost::hana::deleted_implementation>;

    static_assert(hana::Orderable<T>::value, "hana::min(x, y) requires 'x' to be Orderable");

    static_assert(hana::Orderable<U>::value, "hana::min(x, y) requires 'y' to be Orderable");

    return Min::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct min_impl<T, U, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            decltype(auto) cond = hana::less(x, y);
            return hana::if_(static_cast<decltype(cond) &&>(cond), static_cast<X &&>(x), static_cast<Y &&>(y));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr decltype(auto) minimum_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Minimum =
        ::std::conditional_t<(hana::Foldable<S>::value), minimum_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::minimum(xs) requires 'xs' to be Foldable");

    return Minimum::apply(static_cast<Xs &&>(xs));
}

template <typename Xs, typename Predicate>
constexpr decltype(auto) minimum_t::operator()(Xs &&xs, Predicate &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Minimum =
        ::std::conditional_t<(hana::Foldable<S>::value), minimum_pred_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Foldable<S>::value, "hana::minimum(xs, predicate) requires 'xs' to be Foldable");

    return Minimum::apply(static_cast<Xs &&>(xs), static_cast<Predicate &&>(pred));
}

namespace detail
{
template <typename Pred>
struct min_by {
    Pred pred;

    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const
    {
            auto result = (*pred)(x, y);
            return hana::if_(result, static_cast<X &&>(x), static_cast<Y &&>(y));
    }
};
} // namespace detail

template <typename T, bool condition>
struct minimum_pred_impl<T, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr decltype(auto) apply(Xs &&xs, Pred const &pred)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), detail::min_by<decltype(&pred)>{&pred});
    }
};

template <typename T, bool condition>
struct minimum_impl<T, when<condition>> : default_ {
    template <typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::minimum(static_cast<Xs &&>(xs), hana::less);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct monadic_compose_t {
    template <typename F, typename G>
    constexpr auto operator()(F &&f, G &&g) const;
};

constexpr monadic_compose_t monadic_compose{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
struct monadic_compose_helper {
    template <typename F, typename G, typename X>
    constexpr decltype(auto) operator()(F &&f, G &&g, X &&x) const
    {
            using M = typename hana::tag_of<decltype(g(x))>::type;

            static_assert(hana::Monad<M>::value, "hana::monadic_compose(f, g) requires 'g' to return a monadic value");

            return hana::chain(static_cast<G &&>(g)(static_cast<X &&>(x)), static_cast<F &&>(f));
    }
};
} // namespace detail

template <typename F, typename G>
constexpr auto monadic_compose_t::operator()(F &&f, G &&g) const
{
    return hana::partial(detail::monadic_compose_helper{}, static_cast<F &&>(f), static_cast<G &&>(g));
}

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct monadic_fold_left_impl : monadic_fold_left_impl<T, when<true>> {};

template <typename M>
struct monadic_fold_left_t {
    template <typename Xs, typename State, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, State &&state, F &&f) const;

    template <typename Xs, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, F &&f) const;
};

template <typename M>
constexpr monadic_fold_left_t<M> monadic_fold_left{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
template <typename Xs, typename State, typename F>
constexpr decltype(auto) monadic_fold_left_t<M>::operator()(Xs &&xs, State &&state, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using MonadicFoldLeft = ::std::conditional_t<(hana::Foldable<S>::value), monadic_fold_left_impl<S>,
                                                 ::boost::hana::deleted_implementation>;

    static_assert(hana::Monad<M>::value, "hana::monadic_fold_left<M> requires 'M' to be a Monad");

    static_assert(hana::Foldable<S>::value, "hana::monadic_fold_left<M>(xs, state, f) requires 'xs' to be Foldable");

    return MonadicFoldLeft::template apply<M>(static_cast<Xs &&>(xs), static_cast<State &&>(state),
                                              static_cast<F &&>(f));
}

template <typename M>
template <typename Xs, typename F>
constexpr decltype(auto) monadic_fold_left_t<M>::operator()(Xs &&xs, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using MonadicFoldLeft = ::std::conditional_t<(hana::Foldable<S>::value), monadic_fold_left_impl<S>,
                                                 ::boost::hana::deleted_implementation>;

    static_assert(hana::Monad<M>::value, "hana::monadic_fold_left<M> requires 'M' to be a Monad");

    static_assert(hana::Foldable<S>::value, "hana::monadic_fold_left<M>(xs, f) requires 'xs' to be Foldable");

    return MonadicFoldLeft::template apply<M>(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

namespace detail
{
struct foldlM_helper {
    template <typename F, typename X, typename K, typename Z>
    constexpr decltype(auto) operator()(F &&f, X &&x, K &&k, Z &&z) const
    {
            return hana::chain(static_cast<F &&>(f)(static_cast<Z &&>(z), static_cast<X &&>(x)), static_cast<K &&>(k));
    }
};

template <typename End, typename M, typename F>
struct monadic_foldl1_helper {
    F f;
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const
    {
            return f(static_cast<X &&>(x), static_cast<Y &&>(y));
    }
    template <typename Y>
    constexpr decltype(auto) operator()(End, Y &&y) const
    {
            return hana::lift<M>(static_cast<Y &&>(y));
    }
};
} // namespace detail

template <typename T, bool condition>
struct monadic_fold_left_impl<T, when<condition>> : default_ {
    template <typename M, typename Xs, typename S, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, S &&s, F &&f)
    {
            return hana::fold_right(
                static_cast<Xs &&>(xs), hana::lift<M>,
                hana::curry<3>(hana::partial(detail::foldlM_helper{}, static_cast<F &&>(f))))(static_cast<S &&>(s));
    }

    template <typename M, typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            struct end {};
            using G = detail::monadic_foldl1_helper<end, M, typename detail::decay<F>::type>;
            decltype(auto) result = hana::monadic_fold_left<M>(static_cast<Xs &&>(xs), end{}, G{static_cast<F &&>(f)});

            static_assert(!std::is_same<std::remove_reference_t<decltype(result)>, decltype(hana::lift<M>(end{}))>{},
                          "hana::monadic_fold_left<M>(xs, f) requires 'xs' to be non-empty");
            return result;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename T, typename = void>
struct monadic_fold_right_impl : monadic_fold_right_impl<T, when<true>> {};

template <typename M>
struct monadic_fold_right_t {
    template <typename Xs, typename State, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, State &&state, F &&f) const;

    template <typename Xs, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, F &&f) const;
};

template <typename M>
constexpr monadic_fold_right_t<M> monadic_fold_right{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
template <typename Xs, typename State, typename F>
constexpr decltype(auto) monadic_fold_right_t<M>::operator()(Xs &&xs, State &&state, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using MonadicFoldRight = ::std::conditional_t<(hana::Foldable<S>::value), monadic_fold_right_impl<S>,
                                                  ::boost::hana::deleted_implementation>;

    static_assert(hana::Monad<M>::value, "hana::monadic_fold_right<M> requires 'M' to be a Monad");

    static_assert(hana::Foldable<S>::value, "hana::monadic_fold_right<M>(xs, state, f) requires 'xs' to be Foldable");

    return MonadicFoldRight::template apply<M>(static_cast<Xs &&>(xs), static_cast<State &&>(state),
                                               static_cast<F &&>(f));
}

template <typename M>
template <typename Xs, typename F>
constexpr decltype(auto) monadic_fold_right_t<M>::operator()(Xs &&xs, F &&f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using MonadicFoldRight = ::std::conditional_t<(hana::Foldable<S>::value), monadic_fold_right_impl<S>,
                                                  ::boost::hana::deleted_implementation>;

    static_assert(hana::Monad<M>::value, "hana::monadic_fold_right<M> requires 'M' to be a Monad");

    static_assert(hana::Foldable<S>::value, "hana::monadic_fold_right<M>(xs, f) requires 'xs' to be Foldable");

    return MonadicFoldRight::template apply<M>(static_cast<Xs &&>(xs), static_cast<F &&>(f));
}

namespace detail
{
struct foldrM_helper {
    template <typename F, typename K, typename X, typename Z>
    constexpr decltype(auto) operator()(F &&f, K &&k, X &&x, Z &&z) const
    {
            return hana::chain(static_cast<F &&>(f)(static_cast<X &&>(x), static_cast<Z &&>(z)), static_cast<K &&>(k));
    }
};

template <typename End, typename M, typename F>
struct monadic_foldr1_helper {
    F f;
    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const
    {
            return f(static_cast<X &&>(x), static_cast<Y &&>(y));
    }
    template <typename X>
    constexpr decltype(auto) operator()(X &&x, End) const
    {
            return hana::lift<M>(static_cast<X &&>(x));
    }
};
} // namespace detail

template <typename T, bool condition>
struct monadic_fold_right_impl<T, when<condition>> : default_ {
    template <typename M, typename Xs, typename S, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, S &&s, F &&f)
    {
            return hana::fold_left(
                static_cast<Xs &&>(xs), hana::lift<M>,
                hana::curry<3>(hana::partial(detail::foldrM_helper{}, static_cast<F &&>(f))))(static_cast<S &&>(s));
    }

    template <typename M, typename Xs, typename F>
    static constexpr decltype(auto) apply(Xs &&xs, F &&f)
    {
            struct end {};
            using G = detail::monadic_foldr1_helper<end, M, typename detail::decay<F>::type>;
            decltype(auto) result = hana::monadic_fold_right<M>(static_cast<Xs &&>(xs), end{}, G{static_cast<F &&>(f)});

            static_assert(!std::is_same<std::remove_reference_t<decltype(result)>, decltype(hana::lift<M>(end{}))>{},
                          "hana::monadic_fold_right<M>(xs, f) requires 'xs' to be non-empty");
            return result;
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct none_impl : none_impl<S, when<true>> {};

struct none_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr none_t none{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto none_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using None =
        ::std::conditional_t<(hana::Searchable<S>::value), none_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Searchable<S>::value, "hana::none(xs) requires 'xs' to be a Searchable");

    return None::apply(static_cast<Xs &&>(xs));
}

template <typename S, bool condition>
struct none_impl<S, when<condition>> : default_ {
    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return hana::none_of(static_cast<Xs &&>(xs), hana::id);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename Y>
constexpr auto not_equal_t::operator()(X &&x, Y &&y) const
{
    using T = typename hana::tag_of<X>::type;
    using U = typename hana::tag_of<Y>::type;
    using NotEqual = not_equal_impl<T, U>;
    return NotEqual::apply(static_cast<X &&>(x), static_cast<Y &&>(y));
}

template <typename T, typename U, bool condition>
struct not_equal_impl<T, U, when<condition>> : default_ {
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::not_(hana::equal(static_cast<X &&>(x), static_cast<Y &&>(y)));
    }
};

template <typename T, typename U>
struct not_equal_impl<T, U, when<detail::has_nontrivial_common_embedding<Comparable, T, U>::value>> {
    using C = typename hana::common<T, U>::type;
    template <typename X, typename Y>
    static constexpr decltype(auto) apply(X &&x, Y &&y)
    {
            return hana::not_equal(hana::to<C>(static_cast<X &&>(x)), hana::to<C>(static_cast<Y &&>(y)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct ordering_t {
    template <typename F>
    constexpr auto operator()(F &&f) const;
};

constexpr ordering_t ordering{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace detail
{
template <typename F>
struct less_by {
    F f;

    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) const &
    {
            return hana::less(f(static_cast<X &&>(x)), f(static_cast<Y &&>(y)));
    }

    template <typename X, typename Y>
    constexpr decltype(auto) operator()(X &&x, Y &&y) &
    {
            return hana::less(f(static_cast<X &&>(x)), f(static_cast<Y &&>(y)));
    }
};
} // namespace detail

template <typename F>
constexpr auto ordering_t::operator()(F &&f) const
{
    return detail::less_by<typename detail::decay<F>::type>{static_cast<F &&>(f)};
}

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct partition_impl : partition_impl<S, when<true>> {};

struct partition_t : detail::nested_by<partition_t> {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr partition_t partition{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto partition_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Partition =
        ::std::conditional_t<(hana::Sequence<S>::value), partition_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::partition(xs, pred) requires 'xs' to be a Sequence");

    return Partition::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

namespace detail
{
template <bool... B>
struct partition_indices {
    static constexpr detail::array<bool, sizeof...(B)> results{{B...}};
    static constexpr std::size_t left_size = detail::count(results.begin(), results.end(), true);
    static constexpr std::size_t right_size = sizeof...(B) - left_size;

    static constexpr auto compute_left()
    {
            detail::array<std::size_t, left_size> indices{};
            std::size_t *left = &indices[0];
            for (std::size_t i = 0; i < sizeof...(B); ++i)
                if (results[i]) *left++ = i;
            return indices;
    }

    static constexpr auto compute_right()
    {
            detail::array<std::size_t, right_size> indices{};
            std::size_t *right = &indices[0];
            for (std::size_t i = 0; i < sizeof...(B); ++i)
                if (!results[i]) *right++ = i;
            return indices;
    }

    static constexpr auto left_indices = compute_left();
    static constexpr auto right_indices = compute_right();

    template <typename S, typename Xs, std::size_t... l, std::size_t... r>
    static constexpr auto apply(Xs &&xs, std::index_sequence<l...>, std::index_sequence<r...>)
    {
            return hana::make<hana::pair_tag>(hana::make<S>(hana::at_c<left_indices[l]>(static_cast<Xs &&>(xs))...),
                                              hana::make<S>(hana::at_c<right_indices[r]>(static_cast<Xs &&>(xs))...));
    }
};

template <typename Pred>
struct deduce_partition_indices {
    template <typename... Xs>
    auto operator()(Xs &&...xs) const -> detail::partition_indices<
        static_cast<bool>(detail::decay<decltype(std::declval<Pred>()(static_cast<Xs &&>(xs)))>::type::value)...>
    {
            return {};
    }
};
} // namespace detail

template <typename S, bool condition>
struct partition_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&)
    {
            using Indices = decltype(hana::unpack(static_cast<Xs &&>(xs), detail::deduce_partition_indices<Pred &&>{}));
            return Indices::template apply<S>(static_cast<Xs &&>(xs), std::make_index_sequence<Indices::left_size>{},
                                              std::make_index_sequence<Indices::right_size>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct permutations_impl : permutations_impl<S, when<true>> {};

struct permutations_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr permutations_t permutations{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto permutations_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Permutations =
        ::std::conditional_t<(hana::Sequence<S>::value), permutations_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::permutations(xs) requires 'xs' to be a Sequence");

    return Permutations::apply(static_cast<Xs &&>(xs));
}

namespace detail
{
template <std::size_t N>
struct permutation_indices {
    static constexpr auto value = detail::array<std::size_t, N>{}.iota(0).permutations();
};
} // namespace detail

template <typename S, bool condition>
struct permutations_impl<S, when<condition>> : default_ {
    template <std::size_t n, typename Xs, std::size_t... i>
    static constexpr auto nth_permutation(Xs const &xs, std::index_sequence<i...>)
    {
            constexpr auto indices = detail::permutation_indices<sizeof...(i)>::value;
            (void)indices;
            return hana::make<S>(hana::at_c<indices[n][i]>(xs)...);
    }

    template <std::size_t N, typename Xs, std::size_t... n>
    static constexpr auto permutations_helper(Xs const &xs, std::index_sequence<n...>)
    {
            return hana::make<S>(nth_permutation<n>(xs, std::make_index_sequence<N>{})...);
    }

    template <typename Xs>
    static constexpr auto apply(Xs const &xs)
    {
            constexpr std::size_t N = decltype(hana::length(xs))::value;
            constexpr std::size_t total_perms = detail::factorial(N);
            return permutations_helper<N>(xs, std::make_index_sequence<total_perms>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename R, typename = void>
struct power_impl : power_impl<R, when<true>> {};

struct power_t {
    template <typename X, typename N>
    constexpr decltype(auto) operator()(X &&x, N const &n) const;
};

constexpr power_t power{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename X, typename N>
constexpr decltype(auto) power_t::operator()(X &&x, N const &n) const
{
    using R = typename hana::tag_of<X>::type;
    using Power = ::std::conditional_t<(hana::Ring<R>::value && hana::IntegralConstant<N>::value), power_impl<R>,
                                       ::boost::hana::deleted_implementation>;

    static_assert(hana::Ring<R>::value, "hana::power(x, n) requires 'x' to be in a Ring");

    static_assert(hana::IntegralConstant<N>::value, "hana::power(x, n) requires 'n' to be an IntegralConstant");

    static_assert(N::value >= 0, "hana::power(x, n) requires 'n' to be non-negative");

    return Power::apply(static_cast<X &&>(x), n);
}

template <typename R, bool condition>
struct power_impl<R, when<condition>> : default_ {
    template <typename X, typename N>
    static constexpr decltype(auto) apply(X &&x, N const &)
    {
            constexpr std::size_t n = N::value;
            return hana::iterate<n>(hana::partial(hana::mult, static_cast<X &&>(x)), hana::one<R>());
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct prefix_impl : prefix_impl<M, when<true>> {};

struct prefix_t {
    template <typename Xs, typename Pref>
    constexpr auto operator()(Xs &&xs, Pref &&pref) const;
};

constexpr prefix_t prefix{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pref>
constexpr auto prefix_t::operator()(Xs &&xs, Pref &&pref) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Prefix =
        ::std::conditional_t<(hana::MonadPlus<M>::value), prefix_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::prefix(xs, pref) requires 'xs' to be a MonadPlus");

    return Prefix::apply(static_cast<Xs &&>(xs), static_cast<Pref &&>(pref));
}

template <typename M, bool condition>
struct prefix_impl<M, when<condition>> : default_ {
    template <typename Xs, typename Z>
    static constexpr decltype(auto) apply(Xs &&xs, Z &&z)
    {
            return hana::chain(static_cast<Xs &&>(xs),
                               hana::partial(hana::append, hana::lift<M>(static_cast<Z &&>(z))));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct prepend_impl : prepend_impl<M, when<true>> {};

struct prepend_t {
    template <typename Xs, typename X>
    constexpr auto operator()(Xs &&xs, X &&x) const;
};

constexpr prepend_t prepend{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename X>
constexpr auto prepend_t::operator()(Xs &&xs, X &&x) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Prepend =
        ::std::conditional_t<(hana::MonadPlus<M>::value), prepend_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::prepend(xs, x) requires 'xs' to be a MonadPlus");

    return Prepend::apply(static_cast<Xs &&>(xs), static_cast<X &&>(x));
}

template <typename M, bool condition>
struct prepend_impl<M, when<condition>> : default_ {
    template <typename Xs, typename X>
    static constexpr auto apply(Xs &&xs, X &&x)
    {
            return hana::concat(hana::lift<M>(static_cast<X &&>(x)), static_cast<Xs &&>(xs));
    }
};

template <typename S>
struct prepend_impl<S, when<Sequence<S>::value>> {
    template <typename Xs, typename X, std::size_t... i>
    static constexpr auto prepend_helper(Xs &&xs, X &&x, std::index_sequence<i...>)
    {
            return hana::make<S>(static_cast<X &&>(x), hana::at_c<i>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename X>
    static constexpr auto apply(Xs &&xs, X &&x)
    {
            constexpr std::size_t N = decltype(hana::length(xs))::value;
            return prepend_helper(static_cast<Xs &&>(xs), static_cast<X &&>(x), std::make_index_sequence<N>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename R>
template <typename Xs>
constexpr decltype(auto) product_t<R>::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Product =
        ::std::conditional_t<(hana::Foldable<S>::value), product_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Ring<R>::value, "hana::product<R> requires 'R' to be a Ring");

    static_assert(hana::Foldable<S>::value, "hana::product<R>(xs) requires 'xs' to be Foldable");

    return Product::template apply<R>(static_cast<Xs &&>(xs));
}

template <typename T, bool condition>
struct product_impl<T, when<condition>> : default_ {
    template <typename R, typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), hana::one<R>(), hana::mult);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct remove_impl : remove_impl<M, when<true>> {};

struct remove_t {
    template <typename Xs, typename Value>
    constexpr auto operator()(Xs &&xs, Value &&value) const;
};

constexpr remove_t remove{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Value>
constexpr auto remove_t::operator()(Xs &&xs, Value &&value) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Remove =
        ::std::conditional_t<(hana::MonadPlus<M>::value), remove_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::remove(xs, value) requires 'xs' to be a MonadPlus");

    return Remove::apply(static_cast<Xs &&>(xs), static_cast<Value &&>(value));
}

template <typename M, bool condition>
struct remove_impl<M, when<condition>> : default_ {
    template <typename Xs, typename Value>
    static constexpr auto apply(Xs &&xs, Value &&value)
    {
            return hana::filter(static_cast<Xs &&>(xs),
                                hana::compose(hana::not_, hana::equal.to(static_cast<Value &&>(value))));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct remove_at_impl : remove_at_impl<S, when<true>> {};

struct remove_at_t {
    template <typename Xs, typename N>
    constexpr auto operator()(Xs &&xs, N const &n) const;
};

constexpr remove_at_t remove_at{};

template <std::size_t n>
struct remove_at_c_t;

template <std::size_t n>
constexpr remove_at_c_t<n> remove_at_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr auto remove_at_t::operator()(Xs &&xs, N const &n) const
{
    using S = typename hana::tag_of<Xs>::type;
    using RemoveAt = ::std::conditional_t<(hana::Sequence<S>::value && hana::IntegralConstant<N>::value),
                                          remove_at_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::remove_at(xs, n) requires 'xs' to be a Sequence");

    static_assert(hana::IntegralConstant<N>::value, "hana::remove_at(xs, n) requires 'n' to be an IntegralConstant");

    static_assert(N::value >= 0, "hana::remove_at(xs, n) requires 'n' to be non-negative");

    return RemoveAt::apply(static_cast<Xs &&>(xs), n);
}

template <typename S, bool condition>
struct remove_at_impl<S, when<condition>> : default_ {
    template <typename Xs, std::size_t... before, std::size_t... after>
    static constexpr auto remove_at_helper(Xs &&xs, std::index_sequence<before...>, std::index_sequence<after...>)
    {
            return hana::make<S>(hana::at_c<before>(static_cast<Xs &&>(xs))...,
                                 hana::at_c<after + sizeof...(before) + 1>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &)
    {
            constexpr std::size_t n = N::value;
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            static_assert(n < len, "hana::remove_at(xs, n) requires 'n' to be in the bounds of the sequence");
            return remove_at_helper(static_cast<Xs &&>(xs), std::make_index_sequence<n>{},
                                    std::make_index_sequence<len - n - 1>{});
    }
};

template <std::size_t n>
struct remove_at_c_t {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const
    {
            return hana::remove_at(static_cast<Xs &&>(xs), hana::size_c<n>);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct remove_range_impl : remove_range_impl<S, when<true>> {};

struct remove_range_t {
    template <typename Xs, typename From, typename To>
    constexpr auto operator()(Xs &&xs, From const &from, To const &to) const;
};

constexpr remove_range_t remove_range{};

template <std::size_t from, std::size_t to>
struct remove_range_c_t;

template <std::size_t from, std::size_t to>
constexpr remove_range_c_t<from, to> remove_range_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename From, typename To>
constexpr auto remove_range_t::operator()(Xs &&xs, From const &from, To const &to) const
{
    using S = typename hana::tag_of<Xs>::type;
    using RemoveRange = ::std::conditional_t<(hana::Sequence<S>::value && hana::IntegralConstant<From>::value
                                              && hana::IntegralConstant<To>::value),
                                             remove_range_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::remove_range(xs, from, to) requires 'xs' to be a Sequence");

    static_assert(hana::IntegralConstant<From>::value,
                  "hana::remove_range(xs, from, to) requires 'from' to be an IntegralConstant");

    static_assert(hana::IntegralConstant<To>::value,
                  "hana::remove_range(xs, from, to) requires 'to' to be an IntegralConstant");

    return RemoveRange::apply(static_cast<Xs &&>(xs), from, to);
}

template <typename S, bool condition>
struct remove_range_impl<S, when<condition>> : default_ {
    template <std::size_t offset, typename Xs, std::size_t... before, std::size_t... after>
    static constexpr auto remove_range_helper(Xs &&xs, std::index_sequence<before...>, std::index_sequence<after...>)
    {
            return hana::make<S>(hana::at_c<before>(static_cast<Xs &&>(xs))...,
                                 hana::at_c<offset + after>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename From, typename To>
    static constexpr auto apply(Xs &&xs, From const &, To const &)
    {
            constexpr std::size_t from = From::value;
            constexpr std::size_t to = To::value;
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            constexpr std::size_t before = from == to ? len : from;
            constexpr std::size_t after = from == to ? 0 : len - to;

            static_assert(from <= to, "hana::remove_range(xs, from, to) requires '[from, to)' to be a "
                                      "valid interval, meaning that 'from <= to'");
            static_assert(from == to || from >= 0,
                          "hana::remove_range(xs, from, to) requires 'from' to be non-negative");
            static_assert(from == to || to <= len, "hana::remove_range(xs, from, to) requires 'to <= length(xs)'");

            return remove_range_helper<to>(static_cast<Xs &&>(xs), std::make_index_sequence<before>{},
                                           std::make_index_sequence<after>{});
    }
};

template <std::size_t from, std::size_t to>
struct remove_range_c_t {
    template <typename Xs>
    constexpr decltype(auto) operator()(Xs &&xs) const
    {
            return hana::remove_range(static_cast<Xs &&>(xs), hana::size_c<from>, hana::size_c<to>);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename N, typename = void>
struct repeat_impl : repeat_impl<N, when<true>> {};

struct repeat_t {
    template <typename N, typename F>
    constexpr void operator()(N const &n, F &&f) const;
};

constexpr repeat_t repeat{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
template <typename I, bool condition>
struct repeat_impl<I, when<condition>> : default_ {
    template <typename F, std::size_t... i>
    static constexpr void repeat_helper(F &&f, std::index_sequence<i...>)
    {
            using Swallow = std::size_t[];
            (void)Swallow{0, ((void)f(), i)...};
    }

    template <typename N, typename F>
    static constexpr auto apply(N const &, F &&f)
    {
            static_assert(N::value >= 0, "hana::repeat(n, f) requires 'n' to be non-negative");
            constexpr std::size_t n = N::value;
            repeat_helper(static_cast<F &&>(f), std::make_index_sequence<n>{});
    }
};

template <typename N, typename F>
constexpr void repeat_t::operator()(N const &n, F &&f) const
{
    using I = typename hana::tag_of<N>::type;
    using Repeat =
        ::std::conditional_t<(hana::IntegralConstant<I>::value), repeat_impl<I>, ::boost::hana::deleted_implementation>;

    static_assert(hana::IntegralConstant<I>::value, "hana::repeat(n, f) requires 'n' to be an IntegralConstant");

    return Repeat::apply(n, static_cast<F &&>(f));
}

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename = void>
struct replace_impl : replace_impl<Xs, when<true>> {};

struct replace_t {
    template <typename Xs, typename OldVal, typename NewVal>
    constexpr auto operator()(Xs &&xs, OldVal &&oldval, NewVal &&newval) const;
};

constexpr replace_t replace{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename = void>
struct replace_if_impl : replace_if_impl<Xs, when<true>> {};

struct replace_if_t {
    template <typename Xs, typename Pred, typename Value>
    constexpr auto operator()(Xs &&xs, Pred &&pred, Value &&value) const;
};

constexpr replace_if_t replace_if{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred, typename Value>
constexpr auto replace_if_t::operator()(Xs &&xs, Pred &&pred, Value &&value) const
{
    using S = typename hana::tag_of<Xs>::type;
    using ReplaceIf =
        ::std::conditional_t<(hana::Functor<S>::value), replace_if_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Functor<S>::value, "hana::replace_if(xs, pred, value) requires 'xs' to be a Functor");

    return ReplaceIf::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred), static_cast<Value &&>(value));
}

template <typename Fun, bool condition>
struct replace_if_impl<Fun, when<condition>> : default_ {
    template <typename Xs, typename Pred, typename Value>
    static constexpr auto apply(Xs &&xs, Pred &&pred, Value &&v)
    {
            return hana::adjust_if(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred),
                                   hana::always(static_cast<Value &&>(v)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename OldVal, typename NewVal>
constexpr auto replace_t::operator()(Xs &&xs, OldVal &&oldval, NewVal &&newval) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Replace =
        ::std::conditional_t<(hana::Functor<S>::value), replace_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Functor<S>::value, "hana::replace(xs, oldval, newval) requires 'xs' to be a Functor");

    return Replace::apply(static_cast<Xs &&>(xs), static_cast<OldVal &&>(oldval), static_cast<NewVal &&>(newval));
}

template <typename Fun, bool condition>
struct replace_impl<Fun, when<condition>> : default_ {
    template <typename Xs, typename OldVal, typename NewVal>
    static constexpr decltype(auto) apply(Xs &&xs, OldVal &&oldval, NewVal &&newval)
    {
            return hana::replace_if(static_cast<Xs &&>(xs), hana::equal.to(static_cast<OldVal &&>(oldval)),
                                    static_cast<NewVal &&>(newval));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct replicate_impl : replicate_impl<M, when<true>> {};

template <typename M>
struct replicate_t {
    template <typename X, typename N>
    constexpr auto operator()(X &&x, N const &n) const;
};

template <typename M>
constexpr replicate_t<M> replicate{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
template <typename X, typename N>
constexpr auto replicate_t<M>::operator()(X &&x, N const &n) const
{
    using Replicate = ::std::conditional_t<(hana::MonadPlus<M>::value && hana::IntegralConstant<N>::value),
                                           replicate_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::replicate<M>(x, n) requires 'M' to be a MonadPlus");

    static_assert(hana::IntegralConstant<N>::value, "hana::replicate<M>(x, n) requires 'n' to be an IntegralConstant");

    return Replicate::apply(static_cast<X &&>(x), n);
}

template <typename M, bool condition>
struct replicate_impl<M, when<condition>> : default_ {
    template <typename X, typename N>
    static constexpr auto apply(X &&x, N const &n)
    {
            return hana::cycle(hana::lift<M>(static_cast<X &&>(x)), n);
    }
};

template <typename S>
struct replicate_impl<S, when<Sequence<S>::value>> {
    template <typename X, std::size_t... i>
    static constexpr auto replicate_helper(X &&x, std::index_sequence<i...>)
    {
            return hana::make<S>(((void)i, x)...);
    }

    template <typename X, typename N>
    static constexpr auto apply(X &&x, N const &)
    {
            constexpr std::size_t n = N::value;
            return replicate_helper(static_cast<X &&>(x), std::make_index_sequence<n>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct reverse_impl : reverse_impl<S, when<true>> {};

struct reverse_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;
};

constexpr reverse_t reverse{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto reverse_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Reverse =
        ::std::conditional_t<(hana::Sequence<S>::value), reverse_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::reverse(xs) requires 'xs' to be a Sequence");

    return Reverse::apply(static_cast<Xs &&>(xs));
}

template <typename S, bool condition>
struct reverse_impl<S, when<condition>> : default_ {
    template <typename Xs, std::size_t... i>
    static constexpr auto reverse_helper(Xs &&xs, std::index_sequence<i...>)
    {
            return hana::make<S>(hana::at_c<sizeof...(i) - i - 1>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            constexpr std::size_t N = decltype(hana::length(xs))::value;
            return reverse_helper(static_cast<Xs &&>(xs), std::make_index_sequence<N>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

struct reverse_fold_t {
    template <typename Xs, typename S, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, S &&s, F &&f) const;

    template <typename Xs, typename F>
    constexpr decltype(auto) operator()(Xs &&xs, F &&f) const;
};

constexpr reverse_fold_t reverse_fold{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename S, typename F>
constexpr decltype(auto) reverse_fold_t::operator()(Xs &&xs, S &&s, F &&f) const
{
    return hana::fold_right(static_cast<Xs &&>(xs), static_cast<S &&>(s), hana::flip(static_cast<F &&>(f)));
}

template <typename Xs, typename F>
constexpr decltype(auto) reverse_fold_t::operator()(Xs &&xs, F &&f) const
{
    return hana::fold_right(static_cast<Xs &&>(xs), hana::flip(static_cast<F &&>(f)));
}

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct scan_left_impl : scan_left_impl<S, when<true>> {};

struct scan_left_t {
    template <typename Xs, typename State, typename F>
    constexpr auto operator()(Xs &&xs, State &&state, F const &f) const;

    template <typename Xs, typename F>
    constexpr auto operator()(Xs &&xs, F const &f) const;
};

constexpr scan_left_t scan_left{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename F>
constexpr auto scan_left_t::operator()(Xs &&xs, F const &f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using ScanLeft =
        ::std::conditional_t<(hana::Sequence<S>::value), scan_left_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::scan_left(xs, f) requires 'xs' to be a Sequence");

    return ScanLeft::apply(static_cast<Xs &&>(xs), f);
}

template <typename Xs, typename State, typename F>
constexpr auto scan_left_t::operator()(Xs &&xs, State &&state, F const &f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using ScanLeft =
        ::std::conditional_t<(hana::Sequence<S>::value), scan_left_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::scan_left(xs, state, f) requires 'xs' to be a Sequence");

    return ScanLeft::apply(static_cast<Xs &&>(xs), static_cast<State &&>(state), f);
}

template <typename S, bool condition>
struct scan_left_impl<S, when<condition>> : default_ {
    template <typename Xs, typename F, std::size_t n1, std::size_t n2, std::size_t... ns>
    static constexpr auto apply1_impl(Xs &&xs, F const &f, std::index_sequence<n1, n2, ns...>)
    {
            static_assert(n1 == 0, "logic error in Boost.Hana: file a bug report");

            return scan_left_impl::apply_impl(static_cast<Xs &&>(xs), hana::at_c<0>(static_cast<Xs &&>(xs)), f,
                                              std::index_sequence<n2, ns...>{});
    }

    template <typename Xs, typename F, std::size_t n>
    static constexpr auto apply1_impl(Xs &&xs, F const &, std::index_sequence<n>)
    {
            return hana::make<S>(hana::at_c<n>(static_cast<Xs &&>(xs)));
    }

    template <typename Xs, typename F>
    static constexpr auto apply1_impl(Xs &&, F const &, std::index_sequence<>)
    {
            return hana::empty<S>();
    }

    template <typename Xs, typename F>
    static constexpr auto apply(Xs &&xs, F const &f)
    {
            constexpr std::size_t Len = decltype(hana::length(xs))::value;
            return scan_left_impl::apply1_impl(static_cast<Xs &&>(xs), f, std::make_index_sequence<Len>{});
    }

    template <typename Xs, typename State, typename F, std::size_t n1, std::size_t n2, std::size_t... ns>
    static constexpr auto apply_impl(Xs &&xs, State &&state, F const &f, std::index_sequence<n1, n2, ns...>)
    {
            auto rest =
                scan_left_impl::apply_impl(static_cast<Xs &&>(xs), f(state, hana::at_c<n1>(static_cast<Xs &&>(xs))), f,
                                           std::index_sequence<n2, ns...>{});
            return hana::prepend(std::move(rest), static_cast<State &&>(state));
    }

    template <typename Xs, typename State, typename F, std::size_t n>
    static constexpr auto apply_impl(Xs &&xs, State &&state, F const &f, std::index_sequence<n>)
    {
            auto new_state = f(state, hana::at_c<n>(static_cast<Xs &&>(xs)));
            return hana::make<S>(static_cast<State &&>(state), std::move(new_state));
    }

    template <typename Xs, typename State, typename F>
    static constexpr auto apply_impl(Xs &&, State &&state, F const &, std::index_sequence<>)
    {
            return hana::make<S>(static_cast<State &&>(state));
    }

    template <typename Xs, typename State, typename F>
    static constexpr auto apply(Xs &&xs, State &&state, F const &f)
    {
            constexpr std::size_t Len = decltype(hana::length(xs))::value;
            return scan_left_impl::apply_impl(static_cast<Xs &&>(xs), static_cast<State &&>(state), f,
                                              std::make_index_sequence<Len>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct scan_right_impl : scan_right_impl<S, when<true>> {};

struct scan_right_t {
    template <typename Xs, typename State, typename F>
    constexpr auto operator()(Xs &&xs, State &&state, F const &f) const;

    template <typename Xs, typename F>
    constexpr auto operator()(Xs &&xs, F const &f) const;
};

constexpr scan_right_t scan_right{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename F>
constexpr auto scan_right_t::operator()(Xs &&xs, F const &f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using ScanRight =
        ::std::conditional_t<(hana::Sequence<S>::value), scan_right_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::scan_right(xs, f) requires 'xs' to be a Sequence");

    return ScanRight::apply(static_cast<Xs &&>(xs), f);
}

template <typename Xs, typename State, typename F>
constexpr auto scan_right_t::operator()(Xs &&xs, State &&state, F const &f) const
{
    using S = typename hana::tag_of<Xs>::type;
    using ScanRight =
        ::std::conditional_t<(hana::Sequence<S>::value), scan_right_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::scan_right(xs, state, f) requires 'xs' to be a Sequence");

    return ScanRight::apply(static_cast<Xs &&>(xs), static_cast<State &&>(state), f);
}

template <typename S, bool condition>
struct scan_right_impl<S, when<condition>> : default_ {
    template <typename Xs, typename F, std::size_t n1, std::size_t n2, std::size_t... ns>
    static constexpr auto apply1_impl(Xs &&xs, F const &f, std::index_sequence<n1, n2, ns...>)
    {
            auto rest = scan_right_impl::apply1_impl(static_cast<Xs &&>(xs), f, std::index_sequence<n2, ns...>{});
            auto element = f(hana::at_c<n1>(static_cast<Xs &&>(xs)), hana::front(rest));
            return hana::prepend(std::move(rest), std::move(element));
    }

    template <typename Xs, typename F, std::size_t n>
    static constexpr auto apply1_impl(Xs &&xs, F const &, std::index_sequence<n>)
    {
            return hana::make<S>(hana::at_c<n>(static_cast<Xs &&>(xs)));
    }

    template <typename Xs, typename F>
    static constexpr auto apply1_impl(Xs &&, F const &, std::index_sequence<>)
    {
            return hana::empty<S>();
    }

    template <typename Xs, typename F>
    static constexpr auto apply(Xs &&xs, F const &f)
    {
            constexpr std::size_t Len = decltype(hana::length(xs))::value;
            return scan_right_impl::apply1_impl(static_cast<Xs &&>(xs), f, std::make_index_sequence<Len>{});
    }

    template <typename Xs, typename State, typename F, std::size_t n1, std::size_t n2, std::size_t... ns>
    static constexpr auto apply_impl(Xs &&xs, State &&state, F const &f, std::index_sequence<n1, n2, ns...>)
    {
            auto rest = scan_right_impl::apply_impl(static_cast<Xs &&>(xs), static_cast<State &&>(state), f,
                                                    std::index_sequence<n2, ns...>{});
            auto element = f(hana::at_c<n1>(static_cast<Xs &&>(xs)), hana::front(rest));
            return hana::prepend(std::move(rest), std::move(element));
    }

    template <typename Xs, typename State, typename F, std::size_t n>
    static constexpr auto apply_impl(Xs &&xs, State &&state, F const &f, std::index_sequence<n>)
    {
            auto element = f(hana::at_c<n>(static_cast<Xs &&>(xs)), state);
            return hana::make<S>(std::move(element), static_cast<State &&>(state));
    }

    template <typename Xs, typename State, typename F>
    static constexpr auto apply_impl(Xs &&, State &&state, F const &, std::index_sequence<>)
    {
            return hana::make<S>(static_cast<State &&>(state));
    }

    template <typename Xs, typename State, typename F>
    static constexpr auto apply(Xs &&xs, State &&state, F const &f)
    {
            constexpr std::size_t Len = decltype(hana::length(xs))::value;
            return scan_right_impl::apply_impl(static_cast<Xs &&>(xs), static_cast<State &&>(state), f,
                                               std::make_index_sequence<Len>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename... Xs>
struct set;

struct set_tag {};

constexpr auto make_set = make<set_tag>;

constexpr auto to_set = to<set_tag>;

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename... Xs>
struct set final : detail::operators::adl<set<Xs...>>, detail::searchable_operators<set<Xs...>> {
    tuple<Xs...> storage;
    using hana_tag = set_tag;
    static constexpr std::size_t size = sizeof...(Xs);

    explicit constexpr set(tuple<Xs...> const &xs) : storage(xs) {}

    explicit constexpr set(tuple<Xs...> &&xs) : storage(static_cast<tuple<Xs...> &&>(xs)) {}

    constexpr set() = default;
    constexpr set(set const &other) = default;
    constexpr set(set &&other) = default;
};

namespace detail
{
template <>
struct comparable_operators<set_tag> {
    static constexpr bool value = true;
};
} // namespace detail

template <>
struct make_impl<set_tag> {
    template <typename... Xs>
    static constexpr auto apply(Xs &&...xs)
    {
            return set<typename detail::decay<Xs>::type...>{hana::make_tuple(static_cast<Xs &&>(xs)...)};
    }
};

template <>
struct equal_impl<set_tag, set_tag> {
    template <typename S1, typename S2>
    static constexpr auto equal_helper(S1 const &s1, S2 const &s2, hana::true_)
    {
            return hana::is_subset(s1, s2);
    }

    template <typename S1, typename S2>
    static constexpr auto equal_helper(S1 const &, S2 const &, hana::false_)
    {
            return hana::false_c;
    }

    template <typename S1, typename S2>
    static constexpr decltype(auto) apply(S1 &&s1, S2 &&s2)
    {
            return equal_impl::equal_helper(
                s1, s2,
                hana::bool_c<decltype(hana::length(s1.storage))::value == decltype(hana::length(s2.storage))::value>);
    }
};

template <>
struct unpack_impl<set_tag> {
    template <typename Set, typename F>
    static constexpr decltype(auto) apply(Set &&set, F &&f)
    {
            return hana::unpack(static_cast<Set &&>(set).storage, static_cast<F &&>(f));
    }
};

template <>
struct find_if_impl<set_tag> {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            return hana::find_if(static_cast<Xs &&>(xs).storage, static_cast<Pred &&>(pred));
    }
};

template <>
struct any_of_impl<set_tag> {
    template <typename Pred>
    struct any_of_helper {
            Pred const &pred;
            template <typename... X>
            constexpr auto operator()(X const &...x) const
            {
                return hana::or_(pred(x)...);
            }
            constexpr auto operator()() const
            {
                return hana::false_c;
            }
    };

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs const &xs, Pred const &pred)
    {
            return hana::unpack(xs.storage, any_of_helper<Pred>{pred});
    }
};

template <>
struct is_subset_impl<set_tag, set_tag> {
    template <typename Ys>
    struct all_contained {
            Ys const &ys;
            template <typename... X>
            constexpr auto operator()(X const &...x) const
            {
                return hana::bool_c<detail::fast_and<hana::value<decltype(hana::contains(ys, x))>()...>::value>;
            }
    };

    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs const &xs, Ys const &ys)
    {
            return hana::unpack(xs, all_contained<Ys>{ys});
    }
};

template <typename F>
struct to_impl<set_tag, F, when<hana::Foldable<F>::value>> {
    template <typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), hana::make_set(), hana::insert);
    }
};

template <>
struct insert_impl<set_tag> {
    template <typename Xs, typename X, typename Indices>
    static constexpr auto insert_helper(Xs &&xs, X &&, hana::true_, Indices)
    {
            return static_cast<Xs &&>(xs);
    }

    template <typename Xs, typename X, std::size_t... n>
    static constexpr auto insert_helper(Xs &&xs, X &&x, hana::false_, std::index_sequence<n...>)
    {
            return hana::make_set(hana::at_c<n>(static_cast<Xs &&>(xs).storage)..., static_cast<X &&>(x));
    }

    template <typename Xs, typename X>
    static constexpr auto apply(Xs &&xs, X &&x)
    {
            constexpr bool c = hana::value<decltype(hana::contains(xs, x))>();
            constexpr std::size_t size = std::remove_reference<Xs>::type::size;
            return insert_helper(static_cast<Xs &&>(xs), static_cast<X &&>(x), hana::bool_c<c>,
                                 std::make_index_sequence<size>{});
    }
};

template <>
struct erase_key_impl<set_tag> {
    template <typename Xs, typename X>
    static constexpr decltype(auto) apply(Xs &&xs, X &&x)
    {
            return hana::unpack(hana::remove(static_cast<Xs &&>(xs).storage, static_cast<X &&>(x)), hana::make_set);
    }
};

namespace detail
{
template <typename Ys>
struct set_insert_if_contains {
    Ys const &ys;

    template <typename Result, typename Key>
    static constexpr auto helper(Result &&result, Key &&key, hana::true_)
    {
            return hana::insert(static_cast<Result &&>(result), static_cast<Key &&>(key));
    }

    template <typename Result, typename Key>
    static constexpr auto helper(Result &&result, Key &&, hana::false_)
    {
            return static_cast<Result &&>(result);
    }

    template <typename Result, typename Key>
    constexpr auto operator()(Result &&result, Key &&key) const
    {
            constexpr bool keep = hana::value<decltype(hana::contains(ys, key))>();
            return set_insert_if_contains::helper(static_cast<Result &&>(result), static_cast<Key &&>(key),
                                                  hana::bool_c<keep>);
    }
};
} // namespace detail

template <>
struct intersection_impl<set_tag> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys const &ys)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), hana::make_set(), detail::set_insert_if_contains<Ys>{ys});
    }
};

template <>
struct union_impl<set_tag> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys &&ys)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys), hana::insert);
    }
};

template <>
struct difference_impl<set_tag> {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys &&ys)
    {
            return hana::fold_left(static_cast<Ys &&>(ys), static_cast<Xs &&>(xs), hana::erase_key);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

constexpr auto size = hana::length;
}
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct slice_impl : slice_impl<S, when<true>> {};

struct slice_t {
    template <typename Xs, typename Indices>
    constexpr auto operator()(Xs &&xs, Indices &&indices) const;
};

constexpr slice_t slice{};

template <std::size_t from, std::size_t to>
struct slice_c_t;

template <std::size_t from, std::size_t to>
constexpr slice_c_t<from, to> slice_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Indices>
constexpr auto slice_t::operator()(Xs &&xs, Indices &&indices) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Slice = ::std::conditional_t<(hana::Sequence<S>::value && hana::Foldable<Indices>::value), slice_impl<S>,
                                       ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::slice(xs, indices) requires 'xs' to be a Sequence");

    static_assert(hana::Foldable<Indices>::value, "hana::slice(xs, indices) requires 'indices' to be Foldable");

    return Slice::apply(static_cast<Xs &&>(xs), static_cast<Indices &&>(indices));
}

namespace detail
{
template <typename Xs>
struct take_arbitrary {
    Xs &xs;
    using S = typename hana::tag_of<Xs>::type;

    template <typename... N>
    constexpr auto operator()(N const &...) const
    {
            return hana::make<S>(hana::at_c<N::value>(xs)...);
    }
};
} // namespace detail

template <typename S, bool condition>
struct slice_impl<S, when<condition>> : default_ {
    template <std::size_t from, typename Xs, std::size_t... i>
    static constexpr auto from_offset(Xs &&xs, std::index_sequence<i...>)
    {
            return hana::make<S>(hana::at_c<from + i>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename T, T from, T to>
    static constexpr auto apply(Xs &&xs, hana::range<T, from, to> const &)
    {
            return slice_impl::from_offset<from>(static_cast<Xs &&>(xs), std::make_index_sequence<to - from>{});
    }

    template <typename Xs, typename Indices>
    static constexpr auto apply(Xs const &xs, Indices const &indices)
    {
            return hana::unpack(indices, detail::take_arbitrary<Xs const>{xs});
    }
};

template <std::size_t from, std::size_t to>
struct slice_c_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const
    {
            return hana::slice(static_cast<Xs &&>(xs), hana::range_c<std::size_t, from, to>);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct sort_impl : sort_impl<S, when<true>> {};

struct sort_t : detail::nested_by<sort_t> {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;

    template <typename Xs, typename Predicate>
    constexpr auto operator()(Xs &&xs, Predicate &&pred) const;
};

constexpr sort_t sort{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Predicate>
constexpr auto sort_t::operator()(Xs &&xs, Predicate &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Sort = ::std::conditional_t<(hana::Sequence<S>::value), sort_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::sort(xs, predicate) requires 'xs' to be a Sequence");

    return Sort::apply(static_cast<Xs &&>(xs), static_cast<Predicate &&>(pred));
}

template <typename Xs>
constexpr auto sort_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Sort = ::std::conditional_t<(hana::Sequence<S>::value), sort_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::sort(xs) requires 'xs' to be a Sequence");

    return Sort::apply(static_cast<Xs &&>(xs));
}

namespace detail
{
template <typename Xs, typename Pred>
struct sort_predicate {
    template <std::size_t I, std::size_t J>
    using apply = decltype(std::declval<Pred>()(hana::at_c<I>(std::declval<Xs>()), hana::at_c<J>(std::declval<Xs>())));
};

template <typename Left, typename Right>
struct concat;

template <std::size_t... l, std::size_t... r>
struct concat<std::index_sequence<l...>, std::index_sequence<r...>> {
    using type = std::index_sequence<l..., r...>;
};

template <typename Pred, bool PickRight, typename Left, typename Right>
struct merge;

template <typename Pred, std::size_t l0, std::size_t l1, std::size_t... l, std::size_t r0, std::size_t... r>
struct merge<Pred, false, std::index_sequence<l0, l1, l...>, std::index_sequence<r0, r...>> {
    using type =
        typename concat<std::index_sequence<l0>,
                        typename merge<Pred, (bool)Pred::template apply<r0, l1>::value, std::index_sequence<l1, l...>,
                                       std::index_sequence<r0, r...>>::type>::type;
};

template <typename Pred, std::size_t l0, std::size_t r0, std::size_t... r>
struct merge<Pred, false, std::index_sequence<l0>, std::index_sequence<r0, r...>> {
    using type = std::index_sequence<l0, r0, r...>;
};

template <typename Pred, std::size_t l0, std::size_t... l, std::size_t r0, std::size_t r1, std::size_t... r>
struct merge<Pred, true, std::index_sequence<l0, l...>, std::index_sequence<r0, r1, r...>> {
    using type =
        typename concat<std::index_sequence<r0>,
                        typename merge<Pred, (bool)Pred::template apply<r1, l0>::value, std::index_sequence<l0, l...>,
                                       std::index_sequence<r1, r...>>::type>::type;
};

template <typename Pred, std::size_t l0, std::size_t... l, std::size_t r0>
struct merge<Pred, true, std::index_sequence<l0, l...>, std::index_sequence<r0>> {
    using type = std::index_sequence<r0, l0, l...>;
};

template <typename Pred, typename Left, typename Right>
struct merge_helper;

template <typename Pred, std::size_t l0, std::size_t... l, std::size_t r0, std::size_t... r>
struct merge_helper<Pred, std::index_sequence<l0, l...>, std::index_sequence<r0, r...>> {
    using type = typename merge<Pred, (bool)Pred::template apply<r0, l0>::value, std::index_sequence<l0, l...>,
                                std::index_sequence<r0, r...>>::type;
};

template <std::size_t Nr, typename Left, typename Right, typename = void>
struct split;

template <std::size_t Nr, std::size_t... l, std::size_t... r, std::size_t r0>
struct split<Nr, std::index_sequence<l...>, std::index_sequence<r0, r...>, typename std::enable_if<Nr != 0>::type> {
    using sp = split<Nr - 1, std::index_sequence<l..., r0>, std::index_sequence<r...>>;
    using left = typename sp::left;
    using right = typename sp::right;
};

template <std::size_t... l, std::size_t... r>
struct split<0, std::index_sequence<l...>, std::index_sequence<r...>> {
    using left = std::index_sequence<l...>;
    using right = std::index_sequence<r...>;
};

template <typename Pred, typename Sequence>
struct merge_sort_impl;

template <typename Pred, std::size_t... seq>
struct merge_sort_impl<Pred, std::index_sequence<seq...>> {
    using sequence = std::index_sequence<seq...>;
    using sp = split<sequence::size() / 2, std::index_sequence<>, sequence>;
    using type = typename merge_helper<Pred, typename merge_sort_impl<Pred, typename sp::left>::type,
                                       typename merge_sort_impl<Pred, typename sp::right>::type>::type;
};

template <typename Pred, std::size_t x>
struct merge_sort_impl<Pred, std::index_sequence<x>> {
    using type = std::index_sequence<x>;
};

template <typename Pred>
struct merge_sort_impl<Pred, std::index_sequence<>> {
    using type = std::index_sequence<>;
};
} // namespace detail

template <typename S, bool condition>
struct sort_impl<S, when<condition>> : default_ {
    template <typename Xs, std::size_t... i>
    static constexpr auto apply_impl(Xs &&xs, std::index_sequence<i...>)
    {
            return hana::make<S>(hana::at_c<i>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred const &)
    {
            constexpr std::size_t Len = decltype(hana::length(xs))::value;
            using Indices = typename detail::merge_sort_impl<detail::sort_predicate<Xs &&, Pred>,
                                                             std::make_index_sequence<Len>>::type;

            return apply_impl(static_cast<Xs &&>(xs), Indices{});
    }

    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return sort_impl::apply(static_cast<Xs &&>(xs), hana::less);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct span_impl : span_impl<S, when<true>> {};

struct span_t : detail::nested_by<span_t> {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr span_t span{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto span_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Span = ::std::conditional_t<(hana::Sequence<S>::value), span_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::span(xs, pred) requires 'xs' to be a Sequence");

    return Span::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

template <typename S, bool condition>
struct span_impl<S, when<condition>> : default_ {
    template <typename Xs, std::size_t... before, std::size_t... after>
    static constexpr auto span_helper(Xs &&xs, std::index_sequence<before...>, std::index_sequence<after...>)
    {
            return hana::make_pair(hana::make<S>(hana::at_c<before>(static_cast<Xs &&>(xs))...),
                                   hana::make<S>(hana::at_c<sizeof...(before) + after>(static_cast<Xs &&>(xs))...));
    }

    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&)
    {
            using FirstUnsatisfied =
                decltype(hana::unpack(static_cast<Xs &&>(xs), detail::first_unsatisfied_index<Pred &&>{}));
            constexpr std::size_t breakpoint = FirstUnsatisfied::value;
            constexpr std::size_t N = decltype(hana::length(xs))::value;
            return span_helper(static_cast<Xs &&>(xs), std::make_index_sequence<breakpoint>{},
                               std::make_index_sequence<N - breakpoint>{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct suffix_impl : suffix_impl<M, when<true>> {};

struct suffix_t {
    template <typename Xs, typename Sfx>
    constexpr auto operator()(Xs &&xs, Sfx &&sfx) const;
};

constexpr suffix_t suffix{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Sfx>
constexpr auto suffix_t::operator()(Xs &&xs, Sfx &&sfx) const
{
    using M = typename hana::tag_of<Xs>::type;
    using Suffix =
        ::std::conditional_t<(hana::MonadPlus<M>::value), suffix_impl<M>, ::boost::hana::deleted_implementation>;

    static_assert(hana::MonadPlus<M>::value, "hana::suffix(xs, sfx) requires 'xs' to be a MonadPlus");

    return Suffix::apply(static_cast<Xs &&>(xs), static_cast<Sfx &&>(sfx));
}

template <typename M, bool condition>
struct suffix_impl<M, when<condition>> : default_ {
    template <typename Xs, typename Z>
    static constexpr auto apply(Xs &&xs, Z &&z)
    {
            return hana::chain(static_cast<Xs &&>(xs),
                               hana::partial(hana::prepend, hana::lift<M>(static_cast<Z &&>(z))));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
template <typename Xs>
constexpr decltype(auto) sum_t<M>::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Sum = ::std::conditional_t<(hana::Foldable<S>::value), sum_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Monoid<M>::value, "hana::sum<M> requires 'M' to be a Monoid");

    static_assert(hana::Foldable<S>::value, "hana::sum<M>(xs) requires 'xs' to be Foldable");

    return Sum::template apply<M>(static_cast<Xs &&>(xs));
}

template <typename T, bool condition>
struct sum_impl<T, when<condition>> : default_ {
    template <typename M, typename Xs>
    static constexpr decltype(auto) apply(Xs &&xs)
    {
            return hana::fold_left(static_cast<Xs &&>(xs), hana::zero<M>(), hana::plus);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct symmetric_difference_impl : symmetric_difference_impl<S, when<true>> {};

struct symmetric_difference_t {
    template <typename Xs, typename Ys>
    constexpr auto operator()(Xs &&, Ys &&) const;
};

constexpr symmetric_difference_t symmetric_difference{};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto union_t::operator()(Xs &&xs, Ys &&ys) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Union = ::std::conditional_t<(true), union_impl<S>, ::boost::hana::deleted_implementation>;

    return Union::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys));
}

template <typename S, bool condition>
struct union_impl<S, when<condition>> : default_ {
    template <typename... Args>
    static constexpr auto apply(Args &&...) = delete;
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Ys>
constexpr auto symmetric_difference_t::operator()(Xs &&xs, Ys &&ys) const
{
    using S = typename hana::tag_of<Xs>::type;
    using SymmetricDifference =
        ::std::conditional_t<(true), symmetric_difference_impl<S>, ::boost::hana::deleted_implementation>;

    return SymmetricDifference::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys));
}

template <typename S, bool condition>
struct symmetric_difference_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Ys>
    static constexpr auto apply(Xs &&xs, Ys &&ys)
    {
            return hana::union_(hana::difference(xs, ys), hana::difference(ys, xs));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct take_back_impl : take_back_impl<S, when<true>> {};

struct take_back_t {
    template <typename Xs, typename N>
    constexpr auto operator()(Xs &&xs, N const &n) const;
};

constexpr take_back_t take_back{};

template <std::size_t n>
struct take_back_c_t;

template <std::size_t n>
constexpr take_back_c_t<n> take_back_c{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename N>
constexpr auto take_back_t::operator()(Xs &&xs, N const &n) const
{
    using S = typename hana::tag_of<Xs>::type;
    using TakeBack = ::std::conditional_t<(hana::Sequence<S>::value && hana::IntegralConstant<N>::value),
                                          take_back_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::take_back(xs, n) requires 'xs' to be a Sequence");

    static_assert(hana::IntegralConstant<N>::value, "hana::take_back(xs, n) requires 'n' to be an IntegralConstant");

    return TakeBack::apply(static_cast<Xs &&>(xs), n);
}

template <typename S, bool condition>
struct take_back_impl<S, when<condition>> : default_ {
    template <std::size_t start, typename Xs, std::size_t... n>
    static constexpr auto take_back_helper(Xs &&xs, std::index_sequence<n...>)
    {
            return hana::make<S>(hana::at_c<start + n>(static_cast<Xs &&>(xs))...);
    }

    template <typename Xs, typename N>
    static constexpr auto apply(Xs &&xs, N const &)
    {
            constexpr std::size_t n = N::value;
            constexpr std::size_t len = decltype(hana::length(xs))::value;
            constexpr std::size_t start = n < len ? len - n : 0;
            return take_back_helper<start>(static_cast<Xs &&>(xs), std::make_index_sequence<(n < len ? n : len)>{});
    }
};

template <std::size_t n>
struct take_back_c_t {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const
    {
            return hana::take_back(static_cast<Xs &&>(xs), hana::size_c<n>);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct take_while_impl : take_while_impl<S, when<true>> {};

struct take_while_t {
    template <typename Xs, typename Pred>
    constexpr auto operator()(Xs &&xs, Pred &&pred) const;
};

constexpr take_while_t take_while{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename Pred>
constexpr auto take_while_t::operator()(Xs &&xs, Pred &&pred) const
{
    using S = typename hana::tag_of<Xs>::type;
    using TakeWhile =
        ::std::conditional_t<(hana::Sequence<S>::value), take_while_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::take_while(xs, pred) requires 'xs' to be a Sequence");

    return TakeWhile::apply(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred));
}

template <typename S, bool condition>
struct take_while_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&)
    {
            using FirstUnsatisfied =
                decltype(hana::unpack(static_cast<Xs &&>(xs), detail::first_unsatisfied_index<Pred &&>{}));
            return hana::take_front(static_cast<Xs &&>(xs), FirstUnsatisfied{});
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct tap_impl : tap_impl<M, when<true>> {};

template <typename M>
struct tap_t {
    template <typename F>
    constexpr auto operator()(F &&f) const;
};

template <typename M>
constexpr tap_t<M> tap{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M>
template <typename F>
constexpr auto tap_t<M>::operator()(F &&f) const
{
    static_assert(hana::Monad<M>::value, "hana::tap<M> requires 'M' to be a Monad");

    using Tap = ::std::conditional_t<(hana::Monad<M>::value), tap_impl<M>, ::boost::hana::deleted_implementation>;

    return Tap::apply(static_cast<F &&>(f));
}

namespace detail
{
template <typename M>
struct tap_helper {
    template <typename F, typename X>
    constexpr auto operator()(F &&f, X &&x) const
    {
            (void)static_cast<F &&>(f)(x);
            return hana::lift<M>(static_cast<X &&>(x));
    }
};
} // namespace detail

template <typename M, bool condition>
struct tap_impl<M, when<condition>> : default_ {
    template <typename F>
    static constexpr auto apply(F &&f)
    {
            return hana::partial(detail::tap_helper<M>{}, static_cast<F &&>(f));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename M, typename = void>
struct then_impl : then_impl<M, when<true>> {};

struct then_t {
    template <typename Before, typename Xs>
    constexpr decltype(auto) operator()(Before &&before, Xs &&xs) const;
};

constexpr then_t then{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Before, typename Xs>
constexpr decltype(auto) then_t::operator()(Before &&before, Xs &&xs) const
{
    using M = typename hana::tag_of<Before>::type;
    using Then = ::std::conditional_t<(hana::Monad<M>::value && hana::Monad<Xs>::value), then_impl<M>,
                                      ::boost::hana::deleted_implementation>;

    static_assert(hana::Monad<M>::value, "hana::then(before, xs) requires 'before' to be a Monad");

    static_assert(hana::Monad<Xs>::value, "hana::then(before, xs) requires 'xs' to be a Monad");

    return Then::apply(static_cast<Before &&>(before), static_cast<Xs &&>(xs));
}

template <typename M, bool condition>
struct then_impl<M, when<condition>> : default_ {
    template <typename Xs, typename Ys>
    static constexpr decltype(auto) apply(Xs &&xs, Ys &&ys)
    {
            return hana::chain(static_cast<Xs &&>(xs), hana::always(static_cast<Ys &&>(ys)));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{
namespace traits
{
namespace detail
{

template <template <typename...> class F>
struct hana_trait {
    template <typename... T>
    constexpr auto operator()(T const &...) const
    {
            using Result = typename F<typename T::type...>::type;
            return hana::integral_c<typename Result::value_type, Result::value>;
    }
};
} // namespace detail

constexpr auto is_void = detail::hana_trait<std::is_void>{};
constexpr auto is_null_pointer = detail::hana_trait<std::is_null_pointer>{};
constexpr auto is_integral = detail::hana_trait<std::is_integral>{};
constexpr auto is_floating_point = detail::hana_trait<std::is_floating_point>{};
constexpr auto is_array = detail::hana_trait<std::is_array>{};
constexpr auto is_enum = detail::hana_trait<std::is_enum>{};
constexpr auto is_union = detail::hana_trait<std::is_union>{};
constexpr auto is_class = detail::hana_trait<std::is_class>{};
constexpr auto is_function = detail::hana_trait<std::is_function>{};
constexpr auto is_pointer = detail::hana_trait<std::is_pointer>{};
constexpr auto is_lvalue_reference = detail::hana_trait<std::is_lvalue_reference>{};
constexpr auto is_rvalue_reference = detail::hana_trait<std::is_rvalue_reference>{};
constexpr auto is_member_object_pointer = detail::hana_trait<std::is_member_object_pointer>{};
constexpr auto is_member_function_pointer = detail::hana_trait<std::is_member_function_pointer>{};

constexpr auto is_fundamental = detail::hana_trait<std::is_fundamental>{};
constexpr auto is_arithmetic = detail::hana_trait<std::is_arithmetic>{};
constexpr auto is_scalar = detail::hana_trait<std::is_scalar>{};
constexpr auto is_object = detail::hana_trait<std::is_object>{};
constexpr auto is_compound = detail::hana_trait<std::is_compound>{};
constexpr auto is_reference = detail::hana_trait<std::is_reference>{};
constexpr auto is_member_pointer = detail::hana_trait<std::is_member_pointer>{};

constexpr auto is_const = detail::hana_trait<std::is_const>{};
constexpr auto is_volatile = detail::hana_trait<std::is_volatile>{};
constexpr auto is_trivial = detail::hana_trait<std::is_trivial>{};
constexpr auto is_trivially_copyable = detail::hana_trait<std::is_trivially_copyable>{};
constexpr auto is_standard_layout = detail::hana_trait<std::is_standard_layout>{};

constexpr auto is_pod = detail::hana_trait<std::is_pod>{};

constexpr auto is_literal_type = detail::hana_trait<std::is_literal_type>{};

constexpr auto is_empty = detail::hana_trait<std::is_empty>{};
constexpr auto is_polymorphic = detail::hana_trait<std::is_polymorphic>{};
constexpr auto is_abstract = detail::hana_trait<std::is_abstract>{};
constexpr auto is_signed = detail::hana_trait<std::is_signed>{};
constexpr auto is_unsigned = detail::hana_trait<std::is_unsigned>{};

constexpr auto is_constructible = detail::hana_trait<std::is_constructible>{};
constexpr auto is_trivially_constructible = detail::hana_trait<std::is_trivially_constructible>{};
constexpr auto is_nothrow_constructible = detail::hana_trait<std::is_nothrow_constructible>{};

constexpr auto is_default_constructible = detail::hana_trait<std::is_default_constructible>{};
constexpr auto is_trivially_default_constructible = detail::hana_trait<std::is_trivially_default_constructible>{};
constexpr auto is_nothrow_default_constructible = detail::hana_trait<std::is_nothrow_default_constructible>{};

constexpr auto is_copy_constructible = detail::hana_trait<std::is_copy_constructible>{};
constexpr auto is_trivially_copy_constructible = detail::hana_trait<std::is_trivially_copy_constructible>{};
constexpr auto is_nothrow_copy_constructible = detail::hana_trait<std::is_nothrow_copy_constructible>{};

constexpr auto is_move_constructible = detail::hana_trait<std::is_move_constructible>{};
constexpr auto is_trivially_move_constructible = detail::hana_trait<std::is_trivially_move_constructible>{};
constexpr auto is_nothrow_move_constructible = detail::hana_trait<std::is_nothrow_move_constructible>{};

constexpr auto is_assignable = detail::hana_trait<std::is_assignable>{};
constexpr auto is_trivially_assignable = detail::hana_trait<std::is_trivially_assignable>{};
constexpr auto is_nothrow_assignable = detail::hana_trait<std::is_nothrow_assignable>{};

constexpr auto is_copy_assignable = detail::hana_trait<std::is_copy_assignable>{};
constexpr auto is_trivially_copy_assignable = detail::hana_trait<std::is_trivially_copy_assignable>{};
constexpr auto is_nothrow_copy_assignable = detail::hana_trait<std::is_nothrow_copy_assignable>{};

constexpr auto is_move_assignable = detail::hana_trait<std::is_move_assignable>{};
constexpr auto is_trivially_move_assignable = detail::hana_trait<std::is_trivially_move_assignable>{};
constexpr auto is_nothrow_move_assignable = detail::hana_trait<std::is_nothrow_move_assignable>{};

constexpr auto is_destructible = detail::hana_trait<std::is_destructible>{};
constexpr auto is_trivially_destructible = detail::hana_trait<std::is_trivially_destructible>{};
constexpr auto is_nothrow_destructible = detail::hana_trait<std::is_nothrow_destructible>{};

constexpr auto has_virtual_destructor = detail::hana_trait<std::has_virtual_destructor>{};

constexpr auto alignment_of = detail::hana_trait<std::alignment_of>{};
constexpr auto rank = detail::hana_trait<std::rank>{};
constexpr struct extent_t {
    template <typename T, typename N>
    constexpr auto operator()(T const &, N const &) const
    {
            constexpr unsigned n = N::value;
            using Result = typename std::extent<typename T::type, n>::type;
            return hana::integral_c<typename Result::value_type, Result::value>;
    }

    template <typename T>
    constexpr auto operator()(T const &t) const
    {
            return (*this)(t, hana::uint_c<0>);
    }
} extent{};

constexpr auto is_same = detail::hana_trait<std::is_same>{};
constexpr auto is_base_of = detail::hana_trait<std::is_base_of>{};
constexpr auto is_convertible = detail::hana_trait<std::is_convertible>{};

constexpr auto remove_cv = metafunction<std::remove_cv>;
constexpr auto remove_const = metafunction<std::remove_const>;
constexpr auto remove_volatile = metafunction<std::remove_volatile>;

constexpr auto add_cv = metafunction<std::add_cv>;
constexpr auto add_const = metafunction<std::add_const>;
constexpr auto add_volatile = metafunction<std::add_volatile>;

constexpr auto remove_reference = metafunction<std::remove_reference>;
constexpr auto add_lvalue_reference = metafunction<std::add_lvalue_reference>;
constexpr auto add_rvalue_reference = metafunction<std::add_rvalue_reference>;

constexpr auto remove_pointer = metafunction<std::remove_pointer>;
constexpr auto add_pointer = metafunction<std::add_pointer>;

constexpr auto make_signed = metafunction<std::make_signed>;
constexpr auto make_unsigned = metafunction<std::make_unsigned>;

constexpr auto remove_extent = metafunction<std::remove_extent>;
constexpr auto remove_all_extents = metafunction<std::remove_all_extents>;

constexpr struct aligned_storage_t {
    template <typename Len, typename Align>
    constexpr auto operator()(Len const &, Align const &) const
    {
            constexpr std::size_t len = Len::value;
            constexpr std::size_t align = Align::value;
            using Result = typename std::aligned_storage<len, align>::type;
            return hana::type_c<Result>;
    }

    template <typename Len>
    constexpr auto operator()(Len const &) const
    {
            constexpr std::size_t len = Len::value;
            using Result = typename std::aligned_storage<len>::type;
            return hana::type_c<Result>;
    }
} aligned_storage{};

constexpr struct aligned_union_t {
    template <typename Len, typename... T>
    constexpr auto operator()(Len const &, T const &...) const
    {
            constexpr std::size_t len = Len::value;
            using Result = typename std::aligned_union<len, typename T::type...>::type;
            return hana::type_c<Result>;
    }
} aligned_union{};

constexpr auto decay = metafunction<std::decay>;

constexpr auto common_type = metafunction<std::common_type>;
constexpr auto underlying_type = metafunction<std::underlying_type>;

struct declval_t {
    template <typename T>
    typename std::add_rvalue_reference<typename T::type>::type operator()(T const &) const;
};

constexpr declval_t declval{};
} // namespace traits
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct unfold_left_impl : unfold_left_impl<S, when<true>> {};

template <typename S>
struct unfold_left_t;

template <typename S>
constexpr unfold_left_t<S> unfold_left{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S>
struct unfold_left_t {
    static_assert(hana::Sequence<S>::value, "hana::unfold_left<S> requires 'S' to be a Sequence");

    template <typename State, typename F>
    constexpr auto operator()(State &&state, F &&f) const
    {
            return unfold_left_impl<S>::apply(static_cast<State &&>(state), static_cast<F &&>(f));
    }
};

template <typename S, bool condition>
struct unfold_left_impl<S, when<condition>> : default_ {
    struct unfold_left_helper {
            template <typename F, typename P>
            constexpr auto operator()(F &&f, P &&p) const
            {
                return hana::append(unfold_left_impl::apply(hana::first(static_cast<P &&>(p)), static_cast<F &&>(f)),
                                    hana::second(static_cast<P &&>(p)));
            }
    };

    template <typename Init, typename F>
    static constexpr auto apply(Init &&init, F &&f)
    {
            decltype(auto) elt = f(static_cast<Init &&>(init));
            return hana::maybe(empty<S>(), hana::partial(unfold_left_helper{}, static_cast<F &&>(f)),
                               static_cast<decltype(elt) &&>(elt));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct unfold_right_impl : unfold_right_impl<S, when<true>> {};

template <typename S>
struct unfold_right_t;

template <typename S>
constexpr unfold_right_t<S> unfold_right{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S>
struct unfold_right_t {
    static_assert(hana::Sequence<S>::value, "hana::unfold_right<S> requires 'S' to be a Sequence");

    template <typename State, typename F>
    constexpr auto operator()(State &&state, F &&f) const
    {
            return unfold_right_impl<S>::apply(static_cast<State &&>(state), static_cast<F &&>(f));
    }
};

template <typename S, bool condition>
struct unfold_right_impl<S, when<condition>> : default_ {
    struct unfold_right_helper {
            template <typename F, typename P>
            constexpr auto operator()(F &&f, P &&p) const
            {
                return hana::prepend(unfold_right_impl::apply(hana::second(static_cast<P &&>(p)), static_cast<F &&>(f)),
                                     hana::first(static_cast<P &&>(p)));
            }
    };

    template <typename Init, typename F>
    static constexpr auto apply(Init &&init, F &&f)
    {
            decltype(auto) elt = f(static_cast<Init &&>(init));
            return hana::maybe(hana::empty<S>(), hana::partial(unfold_right_helper{}, static_cast<F &&>(f)),
                               static_cast<decltype(elt) &&>(elt));
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct unique_impl : unique_impl<S, when<true>> {};

struct unique_t : detail::nested_by<unique_t> {
    template <typename Xs>
    constexpr auto operator()(Xs &&xs) const;

    template <typename Xs, typename Predicate>
    constexpr auto operator()(Xs &&xs, Predicate &&predicate) const;
};

constexpr unique_t unique{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs>
constexpr auto unique_t::operator()(Xs &&xs) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Unique =
        ::std::conditional_t<(hana::Sequence<S>::value), unique_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::unique(xs) requires 'xs' to be a Sequence");

    return Unique::apply(static_cast<Xs &&>(xs));
}

template <typename Xs, typename Predicate>
constexpr auto unique_t::operator()(Xs &&xs, Predicate &&predicate) const
{
    using S = typename hana::tag_of<Xs>::type;
    using Unique =
        ::std::conditional_t<(hana::Sequence<S>::value), unique_impl<S>, ::boost::hana::deleted_implementation>;

    static_assert(hana::Sequence<S>::value, "hana::unique(xs, predicate) requires 'xs' to be a Sequence");

    return Unique::apply(static_cast<Xs &&>(xs), static_cast<Predicate &&>(predicate));
}

template <typename S, bool condition>
struct unique_impl<S, when<condition>> : default_ {
    template <typename Xs, typename Pred>
    static constexpr auto apply(Xs &&xs, Pred &&pred)
    {
            return hana::transform(hana::group(static_cast<Xs &&>(xs), static_cast<Pred &&>(pred)), hana::front);
    }

    template <typename Xs>
    static constexpr auto apply(Xs &&xs)
    {
            return unique_impl::apply(static_cast<Xs &&>(xs), hana::equal);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct zip_impl : zip_impl<S, when<true>> {};

struct zip_t {
    template <typename Xs, typename... Ys>
    constexpr auto operator()(Xs &&xs, Ys &&...ys) const;
};

constexpr zip_t zip{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct zip_with_impl : zip_with_impl<S, when<true>> {};

struct zip_with_t {
    template <typename F, typename Xs, typename... Ys>
    constexpr auto operator()(F &&f, Xs &&xs, Ys &&...ys) const;
};

constexpr zip_with_t zip_with{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F, typename Xs, typename... Ys>
constexpr auto zip_with_t::operator()(F &&f, Xs &&xs, Ys &&...ys) const
{
    static_assert(detail::fast_and<hana::Sequence<Xs>::value, hana::Sequence<Ys>::value...>::value,
                  "hana::zip_with(f, xs, ys...) requires 'xs' and 'ys...' to be Sequences");

    return zip_with_impl<typename hana::tag_of<Xs>::type>::apply(static_cast<F &&>(f), static_cast<Xs &&>(xs),
                                                                 static_cast<Ys &&>(ys)...);
}

template <typename S>
struct zip_with_impl<S, when<Sequence<S>::value>> {
    template <std::size_t N, typename F, typename... Xs>
    static constexpr decltype(auto) transverse(F &&f, Xs &&...xs)
    {
            return static_cast<F &&>(f)(hana::at_c<N>(static_cast<Xs &&>(xs))...);
    }

    template <std::size_t... N, typename F, typename... Xs>
    static constexpr auto zip_helper(std::index_sequence<N...>, F &&f, Xs &&...xs)
    {
            return hana::make<S>(transverse<N>(f, xs...)...);
    }

    template <typename F, typename X, typename... Xs>
    static constexpr auto apply(F &&f, X &&x, Xs &&...xs)
    {
            constexpr std::size_t N = decltype(hana::length(x))::value;
            return zip_helper(std::make_index_sequence<N>{}, static_cast<F &&>(f), static_cast<X &&>(x),
                              static_cast<Xs &&>(xs)...);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename... Ys>
constexpr auto zip_t::operator()(Xs &&xs, Ys &&...ys) const
{
    static_assert(detail::fast_and<hana::Sequence<Xs>::value, hana::Sequence<Ys>::value...>::value,
                  "hana::zip(xs, ys...) requires 'xs' and 'ys...' to be Sequences");

    return zip_impl<typename hana::tag_of<Xs>::type>::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys)...);
}

template <typename S, bool condition>
struct zip_impl<S, when<condition>> : default_ {
    template <typename... Xs>
    static constexpr decltype(auto) apply(Xs &&...xs)
    {
            return hana::zip_with(hana::make_tuple, static_cast<Xs &&>(xs)...);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct zip_shortest_impl : zip_shortest_impl<S, when<true>> {};

struct zip_shortest_t {
    template <typename Xs, typename... Ys>
    constexpr auto operator()(Xs &&xs, Ys &&...ys) const;
};

constexpr zip_shortest_t zip_shortest{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename S, typename = void>
struct zip_shortest_with_impl : zip_shortest_with_impl<S, when<true>> {};

struct zip_shortest_with_t {
    template <typename F, typename Xs, typename... Ys>
    constexpr auto operator()(F &&f, Xs &&xs, Ys &&...ys) const;
};

constexpr zip_shortest_with_t zip_shortest_with{};

} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename F, typename Xs, typename... Ys>
constexpr auto zip_shortest_with_t::operator()(F &&f, Xs &&xs, Ys &&...ys) const
{
    static_assert(detail::fast_and<hana::Sequence<Xs>::value, hana::Sequence<Ys>::value...>::value,
                  "hana::zip_shortest_with(f, xs, ys...) requires 'xs' and 'ys...' to be Sequences");

    return zip_shortest_with_impl<typename hana::tag_of<Xs>::type>::apply(static_cast<F &&>(f), static_cast<Xs &&>(xs),
                                                                          static_cast<Ys &&>(ys)...);
}

template <typename S, bool condition>
struct zip_shortest_with_impl<S, when<condition>> : default_ {
    template <typename F, typename... Xs>
    static constexpr decltype(auto) apply(F &&f, Xs &&...xs)
    {
            constexpr std::size_t lengths[] = {decltype(hana::length(xs))::value...};
            constexpr std::size_t min_len = *detail::min_element(lengths, lengths + sizeof...(xs));
            return hana::zip_with(static_cast<F &&>(f),
                                  hana::take_front(static_cast<Xs &&>(xs), hana::size_c<min_len>)...);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{
namespace hana
{

template <typename Xs, typename... Ys>
constexpr auto zip_shortest_t::operator()(Xs &&xs, Ys &&...ys) const
{
    static_assert(detail::fast_and<hana::Sequence<Xs>::value, hana::Sequence<Ys>::value...>::value,
                  "hana::zip_shortest(xs, ys...) requires 'xs' and 'ys...' to be Sequences");

    return zip_shortest_impl<typename hana::tag_of<Xs>::type>::apply(static_cast<Xs &&>(xs), static_cast<Ys &&>(ys)...);
}

template <typename S, bool condition>
struct zip_shortest_impl<S, when<condition>> : default_ {
    template <typename... Xs>
    static constexpr decltype(auto) apply(Xs &&...xs)
    {
            return hana::zip_shortest_with(hana::make_tuple, static_cast<Xs &&>(xs)...);
    }
};
} // namespace hana
} // namespace boost

namespace boost
{

__extension__ typedef long long long_long_type;
__extension__ typedef unsigned long long ulong_long_type;

} // namespace boost

namespace boost
{

__extension__ typedef __int128 int128_type;
__extension__ typedef unsigned __int128 uint128_type;

} // namespace boost

namespace mpl_
{
namespace aux
{
}
} // namespace mpl_
namespace boost
{
namespace mpl
{
using namespace mpl_;
namespace aux
{
using namespace mpl_::aux;
}
} // namespace mpl
} // namespace boost

namespace mpl_
{

template <bool C_>
struct bool_;

typedef bool_<true> true_;
typedef bool_<false> false_;

} // namespace mpl_

namespace boost
{
namespace mpl
{
using ::mpl_::bool_;
}
} // namespace boost
namespace boost
{
namespace mpl
{
using ::mpl_::true_;
}
} // namespace boost
namespace boost
{
namespace mpl
{
using ::mpl_::false_;
}
} // namespace boost

namespace mpl_
{
struct integral_c_tag {
    static const int value = 0;
};
} // namespace mpl_
namespace boost
{
namespace mpl
{
using ::mpl_::integral_c_tag;
}
} // namespace boost

namespace mpl_
{

template <bool C_>
struct bool_ {
    static const bool value = C_;
    typedef integral_c_tag tag;
    typedef bool_ type;
    typedef bool value_type;
    constexpr operator bool() const
    {
            return this->value;
    }
};

template <bool C_>
bool const bool_<C_>::value;

} // namespace mpl_

namespace boost
{
namespace mpl
{
namespace aux
{
template <typename T>
struct nested_type_wknd : T::type {};
} // namespace aux
} // namespace mpl
} // namespace boost

namespace mpl_
{

struct void_;

}
namespace boost
{
namespace mpl
{
using ::mpl_::void_;
}
} // namespace boost

namespace mpl_
{

struct na {
    typedef na type;
    enum { value = 0 };
};

} // namespace mpl_
namespace boost
{
namespace mpl
{
using ::mpl_::na;
}
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename T>
struct is_na : false_ {};

template <>
struct is_na<na> : true_ {};

template <typename T>
struct is_not_na : true_ {};

template <>
struct is_not_na<na> : false_ {};

template <typename T, typename U>
struct if_na {
    typedef T type;
};

template <typename U>
struct if_na<na, U> {
    typedef U type;
};

} // namespace mpl
} // namespace boost

namespace mpl_
{

template <int N>
struct int_;

}
namespace boost
{
namespace mpl
{
using ::mpl_::int_;
}
} // namespace boost

namespace mpl_
{

template <int N>
struct int_ {
    static const int value = N;

    typedef int_ type;

    typedef int value_type;
    typedef integral_c_tag tag;

    typedef mpl_::int_<static_cast<int>((value + 1))> next;
    typedef mpl_::int_<static_cast<int>((value - 1))> prior;

    constexpr operator int() const
    {
            return static_cast<int>(this->value);
    }
};

template <int N>
int const mpl_::int_<N>::value;

} // namespace mpl_

namespace boost
{
namespace mpl
{
namespace aux
{

template <typename F>
struct template_arity;

}
} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename T = na, typename Tag = void_, typename Arity = int_<aux::template_arity<T>::value>

          >
struct lambda;

}
} // namespace boost

namespace boost
{
namespace mpl
{

namespace aux
{

template <bool C_, typename T1, typename T2, typename T3, typename T4>
struct or_impl : true_ {};

template <typename T1, typename T2, typename T3, typename T4>
struct or_impl<false, T1, T2, T3, T4> : or_impl<::boost::mpl::aux::nested_type_wknd<T1>::value, T2, T3, T4, false_> {};

template <>
struct or_impl<false, false_, false_, false_, false_> : false_ {};

} // namespace aux

template <typename T1 = na, typename T2 = na, typename T3 = false_, typename T4 = false_, typename T5 = false_>
struct or_

    : aux::or_impl<::boost::mpl::aux::nested_type_wknd<T1>::value, T2, T3, T4, T5>

{};

template <>
struct or_<na, na> {
    template <typename T1, typename T2, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : or_<T1, T2> {};
};
template <typename Tag>
struct lambda<or_<na, na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef or_<na, na> result_;
    typedef or_<na, na> type;
};
namespace aux
{
template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct template_arity<or_<T1, T2, T3, T4, T5>> : int_<5> {};
template <>
struct template_arity<or_<na, na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace mpl_
{

template <bool B>
struct bool_;
template <class I, I val>
struct integral_c;
struct integral_c_tag;
} // namespace mpl_

namespace boost
{
namespace mpl
{
using ::mpl_::bool_;
using ::mpl_::integral_c;
using ::mpl_::integral_c_tag;
} // namespace mpl
} // namespace boost

namespace boost
{
template <class T, T val>
struct integral_constant {
    typedef mpl::integral_c_tag tag;
    typedef T value_type;
    typedef integral_constant<T, val> type;
    static const T value = val;

    operator const mpl::integral_c<T, val> &() const
    {
            static const char data[sizeof(long)] = {0};
            static const void *pdata = data;
            return *(reinterpret_cast<const mpl::integral_c<T, val> *>(pdata));
    }
    constexpr operator T() const
    {
            return val;
    }
};

template <class T, T val>
T const integral_constant<T, val>::value;

template <bool val>
struct integral_constant<bool, val> {
    typedef mpl::integral_c_tag tag;
    typedef bool value_type;
    typedef integral_constant<bool, val> type;
    static const bool value = val;

    operator const mpl::bool_<val> &() const
    {
            static const char data[sizeof(long)] = {0};
            static const void *pdata = data;
            return *(reinterpret_cast<const mpl::bool_<val> *>(pdata));
    }
    constexpr operator bool() const
    {
            return val;
    }
};

template <bool val>
bool const integral_constant<bool, val>::value;

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

} // namespace boost

namespace boost
{

template <class T>
struct is_void : public false_type {};

template <>
struct is_void<void> : public true_type {};
template <>
struct is_void<const void> : public true_type {};
template <>
struct is_void<const volatile void> : public true_type {};
template <>
struct is_void<volatile void> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct is_lvalue_reference : public false_type {};
template <class T>
struct is_lvalue_reference<T &> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct is_rvalue_reference : public false_type {};

template <class T>
struct is_rvalue_reference<T &&> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct is_reference
    : public integral_constant<bool, ::boost::is_lvalue_reference<T>::value || ::boost::is_rvalue_reference<T>::value> {
};

} // namespace boost

namespace boost
{

namespace type_traits_detail
{

template <typename T, bool b>
struct add_rvalue_reference_helper {
    typedef T type;
};

template <typename T>
struct add_rvalue_reference_helper<T, true> {
    typedef T &&type;
};

template <typename T>
struct add_rvalue_reference_imp {
    typedef typename boost::type_traits_detail::add_rvalue_reference_helper<
        T, (is_void<T>::value == false && is_reference<T>::value == false)>::type type;
};

} // namespace type_traits_detail

template <class T>
struct add_rvalue_reference {
    typedef typename boost::type_traits_detail::add_rvalue_reference_imp<T>::type type;
};

template <class T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

} // namespace boost

namespace boost
{

template <typename T>
typename add_rvalue_reference<T>::type declval() noexcept;

}

namespace boost
{

namespace detail
{

template <class T>
struct remove_rvalue_ref {
    typedef T type;
};

template <class T>
struct remove_rvalue_ref<T &&> {
    typedef T type;
};

} // namespace detail

template <class T>
struct remove_reference {
    typedef typename boost::detail::remove_rvalue_ref<T>::type type;
};
template <class T>
struct remove_reference<T &> {
    typedef T type;
};

template <class T>
using remove_reference_t = typename remove_reference<T>::type;

} // namespace boost

namespace boost
{

template <class T>
struct is_function : public false_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...)> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...)> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) const> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) const> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) const &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) const &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile &> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) &&> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) &&> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) const &&> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) const &&> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile &&> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile &&> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &&> : public true_type {};

template <class Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile &&> : public true_type {};

} // namespace boost

namespace boost
{
namespace type_traits
{

typedef char yes_type;
struct no_type {
    char padding[8];
};

} // namespace type_traits
} // namespace boost

namespace boost
{

namespace detail
{

template <std::size_t N>
struct ok_tag {
    double d;
    char c[N];
};

template <class T>
ok_tag<sizeof(T)> check_is_complete(int);
template <class T>
char check_is_complete(...);
} // namespace detail

template <class T>
struct is_complete
    : public integral_constant<bool, ::boost::is_function<typename boost::remove_reference<T>::type>::value
                                         || (sizeof(boost::detail::check_is_complete<T>(0)) != sizeof(char))> {};

} // namespace boost

namespace boost
{

template <class T>
struct is_array : public false_type {};

template <class T, std::size_t N>
struct is_array<T[N]> : public true_type {};
template <class T, std::size_t N>
struct is_array<T const[N]> : public true_type {};
template <class T, std::size_t N>
struct is_array<T volatile[N]> : public true_type {};
template <class T, std::size_t N>
struct is_array<T const volatile[N]> : public true_type {};

template <class T>
struct is_array<T[]> : public true_type {};
template <class T>
struct is_array<T const[]> : public true_type{};
template <class T>
struct is_array<T const volatile[]> : public true_type{};
template <class T>
struct is_array<T volatile[]> : public true_type{};

} // namespace boost

namespace boost
{

template <class From, class To>
struct is_convertible : public integral_constant<bool, __is_convertible_to(From, To)> {
    static_assert(boost::is_complete<To>::value || boost::is_void<To>::value || boost::is_array<To>::value,
                  "Destination argument type to is_convertible must be a complete type");
    static_assert(boost::is_complete<From>::value || boost::is_void<From>::value || boost::is_array<From>::value,
                  "From argument type to is_convertible must be a complete type");
};

} // namespace boost

namespace boost
{
namespace iterators
{

template <typename A, typename B>
struct is_interoperable

    : mpl::or_<is_convertible<A, B>, is_convertible<B, A>>

{};

} // namespace iterators

using iterators::is_interoperable;

} // namespace boost

namespace boost
{
namespace iterators
{

template <class Iterator>
struct iterator_value {
    typedef typename std::iterator_traits<Iterator>::value_type type;
};

template <class Iterator>
struct iterator_reference {
    typedef typename std::iterator_traits<Iterator>::reference type;
};

template <class Iterator>
struct iterator_pointer {
    typedef typename std::iterator_traits<Iterator>::pointer type;
};

template <class Iterator>
struct iterator_difference {
    typedef typename std::iterator_traits<Iterator>::difference_type type;
};

template <class Iterator>
struct iterator_category {
    typedef typename std::iterator_traits<Iterator>::iterator_category type;
};

} // namespace iterators

using iterators::iterator_category;
using iterators::iterator_difference;
using iterators::iterator_pointer;
using iterators::iterator_reference;
using iterators::iterator_value;

} // namespace boost

namespace boost
{
namespace mpl
{
namespace aux
{

template <typename T>
struct value_type_wknd {
    typedef typename T::value_type type;
};

} // namespace aux
} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <bool C, typename T1, typename T2>
struct if_c {
    typedef T1 type;
};

template <typename T1, typename T2>
struct if_c<false, T1, T2> {
    typedef T2 type;
};

template <typename T1 = na, typename T2 = na, typename T3 = na>
struct if_ {
  private:
    typedef if_c<

        static_cast<bool>(T1::value)

            ,
        T2, T3>
        almost_type_;

  public:
    typedef typename almost_type_::type type;
};

template <>
struct if_<na, na, na> {
    template <typename T1, typename T2, typename T3, typename T4 = na, typename T5 = na>
    struct apply : if_<T1, T2, T3> {};
};
template <typename Tag>
struct lambda<if_<na, na, na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef if_<na, na, na> result_;
    typedef if_<na, na, na> type;
};
namespace aux
{
template <typename T1, typename T2, typename T3>
struct template_arity<if_<T1, T2, T3>> : int_<3> {};
template <>
struct template_arity<if_<na, na, na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename C = na, typename F1 = na, typename F2 = na>
struct eval_if

{
    typedef typename if_<C, F1, F2>::type f_;
    typedef typename f_::type type;
};

template <bool C, typename F1, typename F2>
struct eval_if_c

{
    typedef typename if_c<C, F1, F2>::type f_;
    typedef typename f_::type type;
};

template <>
struct eval_if<na, na, na> {
    template <typename T1, typename T2, typename T3, typename T4 = na, typename T5 = na>
    struct apply : eval_if<T1, T2, T3> {};
};
template <typename Tag>
struct lambda<eval_if<na, na, na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef eval_if<na, na, na> result_;
    typedef eval_if<na, na, na> type;
};
namespace aux
{
template <typename T1, typename T2, typename T3>
struct template_arity<eval_if<T1, T2, T3>> : int_<3> {};
template <>
struct template_arity<eval_if<na, na, na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename T = na>
struct identity {
    typedef T type;
};

template <typename T = na>
struct make_identity {
    typedef identity<T> type;
};

template <>
struct identity<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : identity<T1> {};
};
template <typename Tag>
struct lambda<identity<na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef identity<na> result_;
    typedef identity<na> type;
};
namespace aux
{
template <typename T1>
struct template_arity<identity<T1>> : int_<1> {};
template <>
struct template_arity<identity<na>> : int_<-1> {};
} // namespace aux
template <>
struct make_identity<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : make_identity<T1> {};
};
template <typename Tag>
struct lambda<make_identity<na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef make_identity<na> result_;
    typedef make_identity<na> type;
};
namespace aux
{
template <typename T1>
struct template_arity<make_identity<T1>> : int_<1> {};
template <>
struct template_arity<make_identity<na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace mpl_
{

template <int N>
struct arg;

}
namespace boost
{
namespace mpl
{
using ::mpl_::arg;
}
} // namespace boost

namespace boost
{
namespace mpl
{

namespace aux
{

template <long C_>
struct not_impl : bool_<!C_> {};

} // namespace aux

template <typename T = na>
struct not_ : aux::not_impl<::boost::mpl::aux::nested_type_wknd<T>::value> {};

template <>
struct not_<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : not_<T1> {};
};
template <typename Tag>
struct lambda<not_<na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef not_<na> result_;
    typedef not_<na> type;
};
namespace aux
{
template <typename T1>
struct template_arity<not_<T1>> : int_<1> {};
template <>
struct template_arity<not_<na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{
namespace aux
{

typedef char (&no_tag)[1];
typedef char (&yes_tag)[2];

template <bool C_>
struct yes_no_tag {
    typedef no_tag type;
};

template <>
struct yes_no_tag<true> {
    typedef yes_tag type;
};

template <std::size_t n>
struct weighted_tag {
    typedef char (&type)[n];
};

} // namespace aux
} // namespace mpl
} // namespace boost

namespace mpl_
{

struct failed {};

template <bool C>
struct assert {
    typedef void *type;
};
template <>
struct assert<false> {
    typedef assert type;
};

template <bool C>
int assertion_failed(typename assert<C>::type);

template <bool C>
struct assertion {
    static int failed(assert<false>);
};

template <>
struct assertion<true> {
    static int failed(void *);
};

struct assert_ {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na>
    struct types {};

    static assert_ const arg;
    enum relations { equal = 1, not_equal, greater, greater_equal, less, less_equal };
};

boost::mpl::aux::weighted_tag<1>::type operator==(assert_, assert_);
boost::mpl::aux::weighted_tag<2>::type operator!=(assert_, assert_);
boost::mpl::aux::weighted_tag<3>::type operator>(assert_, assert_);
boost::mpl::aux::weighted_tag<4>::type operator>=(assert_, assert_);
boost::mpl::aux::weighted_tag<5>::type operator<(assert_, assert_);
boost::mpl::aux::weighted_tag<6>::type operator<=(assert_, assert_);

template <assert_::relations r, long x, long y>
struct assert_relation {};

template <bool>
struct assert_arg_pred_impl {
    typedef int type;
};
template <>
struct assert_arg_pred_impl<true> {
    typedef void *type;
};

template <typename P>
struct assert_arg_pred {
    typedef typename P::type p_type;
    typedef typename assert_arg_pred_impl<p_type::value>::type type;
};

template <typename P>
struct assert_arg_pred_not {
    typedef typename P::type p_type;
    enum { p = !p_type::value };
    typedef typename assert_arg_pred_impl<p>::type type;
};

template <typename Pred>
failed ************(Pred::************assert_arg(void (*)(Pred), typename assert_arg_pred<Pred>::type));

template <typename Pred>
failed ************(boost::mpl::not_<Pred>::************assert_not_arg(void (*)(Pred),
                                                                       typename assert_arg_pred_not<Pred>::type));

template <typename Pred>
assert<false> assert_arg(void (*)(Pred), typename assert_arg_pred_not<Pred>::type);

template <typename Pred>
assert<false> assert_not_arg(void (*)(Pred), typename assert_arg_pred<Pred>::type);

} // namespace mpl_

namespace mpl_
{
template <>
struct arg<-1> {
    static const int value = -1;

    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
            typedef U1 type;
            enum {
                mpl_assertion_in_line_27 = sizeof(boost::mpl::assertion_failed<false>(
                    boost::mpl::assert_not_arg((void (*)(boost::mpl::is_na<type>))0, 1)))
            };
    };
};

template <>
struct arg<1> {
    static const int value = 1;
    typedef arg<2> next;

    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
            typedef U1 type;
            enum {
                mpl_assertion_in_line_45 = sizeof(boost::mpl::assertion_failed<false>(
                    boost::mpl::assert_not_arg((void (*)(boost::mpl::is_na<type>))0, 1)))
            };
    };
};

template <>
struct arg<2> {
    static const int value = 2;
    typedef arg<3> next;

    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
            typedef U2 type;
            enum {
                mpl_assertion_in_line_63 = sizeof(boost::mpl::assertion_failed<false>(
                    boost::mpl::assert_not_arg((void (*)(boost::mpl::is_na<type>))0, 1)))
            };
    };
};

template <>
struct arg<3> {
    static const int value = 3;
    typedef arg<4> next;

    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
            typedef U3 type;
            enum {
                mpl_assertion_in_line_81 = sizeof(boost::mpl::assertion_failed<false>(
                    boost::mpl::assert_not_arg((void (*)(boost::mpl::is_na<type>))0, 1)))
            };
    };
};

template <>
struct arg<4> {
    static const int value = 4;
    typedef arg<5> next;

    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
            typedef U4 type;
            enum {
                mpl_assertion_in_line_99 = sizeof(boost::mpl::assertion_failed<false>(
                    boost::mpl::assert_not_arg((void (*)(boost::mpl::is_na<type>))0, 1)))
            };
    };
};

template <>
struct arg<5> {
    static const int value = 5;
    typedef arg<6> next;

    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
            typedef U5 type;
            enum {
                mpl_assertion_in_line_117 = sizeof(boost::mpl::assertion_failed<false>(
                    boost::mpl::assert_not_arg((void (*)(boost::mpl::is_na<type>))0, 1)))
            };
    };
};

} // namespace mpl_

namespace mpl_
{
typedef arg<-1> _;
}
namespace boost
{
namespace mpl
{

using ::mpl_::_;

namespace placeholders
{
using mpl_::_;
}

} // namespace mpl
} // namespace boost

namespace mpl_
{
typedef arg<1> _1;

}
namespace boost
{
namespace mpl
{

using ::mpl_::_1;

namespace placeholders
{
using mpl_::_1;
}

} // namespace mpl
} // namespace boost
namespace mpl_
{
typedef arg<2> _2;

}
namespace boost
{
namespace mpl
{

using ::mpl_::_2;

namespace placeholders
{
using mpl_::_2;
}

} // namespace mpl
} // namespace boost
namespace mpl_
{
typedef arg<3> _3;

}
namespace boost
{
namespace mpl
{

using ::mpl_::_3;

namespace placeholders
{
using mpl_::_3;
}

} // namespace mpl
} // namespace boost
namespace mpl_
{
typedef arg<4> _4;

}
namespace boost
{
namespace mpl
{

using ::mpl_::_4;

namespace placeholders
{
using mpl_::_4;
}

} // namespace mpl
} // namespace boost
namespace mpl_
{
typedef arg<5> _5;

}
namespace boost
{
namespace mpl
{

using ::mpl_::_5;

namespace placeholders
{
using mpl_::_5;
}

} // namespace mpl
} // namespace boost
namespace mpl_
{
typedef arg<6> _6;

}
namespace boost
{
namespace mpl
{

using ::mpl_::_6;

namespace placeholders
{
using mpl_::_6;
}

} // namespace mpl
} // namespace boost

namespace boost
{
namespace iterators
{

struct no_traversal_tag {};

struct incrementable_traversal_tag : no_traversal_tag {};

struct single_pass_traversal_tag : incrementable_traversal_tag {};

struct forward_traversal_tag : single_pass_traversal_tag {};

struct bidirectional_traversal_tag : forward_traversal_tag {};

struct random_access_traversal_tag : bidirectional_traversal_tag {};

namespace detail
{

template <class Cat>
struct old_category_to_traversal
    : mpl::eval_if<
          is_convertible<Cat, std::random_access_iterator_tag>, mpl::identity<random_access_traversal_tag>,
          mpl::eval_if<
              is_convertible<Cat, std::bidirectional_iterator_tag>, mpl::identity<bidirectional_traversal_tag>,
              mpl::eval_if<
                  is_convertible<Cat, std::forward_iterator_tag>, mpl::identity<forward_traversal_tag>,
                  mpl::eval_if<is_convertible<Cat, std::input_iterator_tag>, mpl::identity<single_pass_traversal_tag>,
                               mpl::eval_if<is_convertible<Cat, std::output_iterator_tag>,
                                            mpl::identity<incrementable_traversal_tag>, void>>>>> {};

} // namespace detail

template <class Cat>
struct iterator_category_to_traversal
    : mpl::eval_if<is_convertible<Cat, incrementable_traversal_tag>, mpl::identity<Cat>,
                   boost::iterators::detail::old_category_to_traversal<Cat>> {};

template <class Iterator = mpl::_1>
struct iterator_traversal : iterator_category_to_traversal<typename std::iterator_traits<Iterator>::iterator_category> {
};

template <class Traversal>
struct pure_traversal_tag
    : mpl::eval_if<
          is_convertible<Traversal, random_access_traversal_tag>, mpl::identity<random_access_traversal_tag>,
          mpl::eval_if<
              is_convertible<Traversal, bidirectional_traversal_tag>, mpl::identity<bidirectional_traversal_tag>,
              mpl::eval_if<is_convertible<Traversal, forward_traversal_tag>, mpl::identity<forward_traversal_tag>,
                           mpl::eval_if<is_convertible<Traversal, single_pass_traversal_tag>,
                                        mpl::identity<single_pass_traversal_tag>,
                                        mpl::eval_if<is_convertible<Traversal, incrementable_traversal_tag>,
                                                     mpl::identity<incrementable_traversal_tag>, void>>>>> {};

template <class Iterator = mpl::_1>
struct pure_iterator_traversal : pure_traversal_tag<typename iterator_traversal<Iterator>::type> {};

} // namespace iterators

using iterators::bidirectional_traversal_tag;
using iterators::forward_traversal_tag;
using iterators::incrementable_traversal_tag;
using iterators::iterator_category_to_traversal;
using iterators::iterator_traversal;
using iterators::no_traversal_tag;
using iterators::random_access_traversal_tag;
using iterators::single_pass_traversal_tag;

namespace detail
{
using iterators::pure_traversal_tag;
}

} // namespace boost

namespace boost
{

struct use_default {};

} // namespace boost

namespace boost
{
namespace mpl
{

namespace aux
{

template <bool C_, typename T1, typename T2, typename T3, typename T4>
struct and_impl : false_ {};

template <typename T1, typename T2, typename T3, typename T4>
struct and_impl<true, T1, T2, T3, T4> : and_impl<::boost::mpl::aux::nested_type_wknd<T1>::value, T2, T3, T4, true_> {};

template <>
struct and_impl<true, true_, true_, true_, true_> : true_ {};

} // namespace aux

template <typename T1 = na, typename T2 = na, typename T3 = true_, typename T4 = true_, typename T5 = true_>
struct and_

    : aux::and_impl<::boost::mpl::aux::nested_type_wknd<T1>::value, T2, T3, T4, T5>

{};

template <>
struct and_<na, na> {
    template <typename T1, typename T2, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : and_<T1, T2> {};
};
template <typename Tag>
struct lambda<and_<na, na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef and_<na, na> result_;
    typedef and_<na, na> type;
};
namespace aux
{
template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct template_arity<and_<T1, T2, T3, T4, T5>> : int_<5> {};
template <>
struct template_arity<and_<na, na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{

template <class T, class U>
struct is_same : public false_type {};
template <class T>
struct is_same<T, T> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct is_const : public false_type {};
template <class T>
struct is_const<T const> : public true_type {};
template <class T, std::size_t N>
struct is_const<T const[N]> : public true_type {};
template <class T>
struct is_const<T const[]> : public true_type{};

} // namespace boost

namespace boost
{

template <class T>
struct is_pointer : public false_type {};
template <class T>
struct is_pointer<T *> : public true_type {};
template <class T>
struct is_pointer<T *const> : public true_type {};
template <class T>
struct is_pointer<T *const volatile> : public true_type {};
template <class T>
struct is_pointer<T *volatile> : public true_type {};

} // namespace boost

namespace boost
{

namespace detail
{

template <typename T>
struct is_class_impl {
    static const bool value = __is_class(T);
};

} // namespace detail

template <class T>
struct is_class : public integral_constant<bool, ::boost::detail::is_class_impl<T>::value> {};

} // namespace boost

namespace boost
{

template <class T>
struct is_volatile : public false_type {};
template <class T>
struct is_volatile<T volatile> : public true_type {};
template <class T, std::size_t N>
struct is_volatile<T volatile[N]> : public true_type {};
template <class T>
struct is_volatile<T volatile[]> : public true_type{};

} // namespace boost

namespace boost
{

template <class T>
struct is_member_function_pointer : public false_type {};
template <class T>
struct is_member_function_pointer<T const> : public is_member_function_pointer<T> {};
template <class T>
struct is_member_function_pointer<T volatile> : public is_member_function_pointer<T> {};
template <class T>
struct is_member_function_pointer<T const volatile> : public is_member_function_pointer<T> {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...)> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...)> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) const> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) const> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) volatile> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) volatile> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) const volatile> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) const volatile> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) &> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) &> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) const &> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) const &> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) volatile &> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) volatile &> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) const volatile &> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) const volatile &> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) &&> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) &&> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) const &&> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) const &&> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) volatile &&> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) volatile &&> : public true_type {};

template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args...) const volatile &&> : public true_type {};
template <class Ret, class C, class... Args>
struct is_member_function_pointer<Ret (C::*)(Args..., ...) const volatile &&> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct is_member_pointer : public integral_constant<bool, ::boost::is_member_function_pointer<T>::value> {};
template <class T, class U>
struct is_member_pointer<U T::*> : public true_type {};

template <class T, class U>
struct is_member_pointer<U T::*const> : public true_type {};
template <class T, class U>
struct is_member_pointer<U T::*const volatile> : public true_type {};
template <class T, class U>
struct is_member_pointer<U T::*volatile> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct remove_cv {
    typedef T type;
};
template <class T>
struct remove_cv<T const> {
    typedef T type;
};
template <class T>
struct remove_cv<T volatile> {
    typedef T type;
};
template <class T>
struct remove_cv<T const volatile> {
    typedef T type;
};

template <class T, std::size_t N>
struct remove_cv<T const [N]> {
    typedef T type[N];
};
template <class T, std::size_t N>
struct remove_cv<T const volatile [N]> {
    typedef T type[N];
};
template <class T, std::size_t N>
struct remove_cv<T volatile [N]> {
    typedef T type[N];
};

template <class T>
struct remove_cv<T const[]>
{
    typedef T type[];
};
template <class T>
struct remove_cv<T const volatile[]>
{
    typedef T type[];
};
template <class T>
struct remove_cv<T volatile[]>
{
    typedef T type[];
};

template <class T>
using remove_cv_t = typename remove_cv<T>::type;

} // namespace boost

namespace boost
{

template <class T>
struct remove_pointer {
    typedef T type;
};
template <class T>
struct remove_pointer<T *> {
    typedef T type;
};
template <class T>
struct remove_pointer<T *const> {
    typedef T type;
};
template <class T>
struct remove_pointer<T *volatile> {
    typedef T type;
};
template <class T>
struct remove_pointer<T *const volatile> {
    typedef T type;
};

template <class T>
using remove_pointer_t = typename remove_pointer<T>::type;

} // namespace boost

namespace boost
{
namespace detail
{

template <bool b>
struct if_true {
    template <class T, class F>
    struct then {
            typedef T type;
    };
};

template <>
struct if_true<false> {
    template <class T, class F>
    struct then {
            typedef F type;
    };
};
} // namespace detail
} // namespace boost

namespace boost
{
namespace detail
{

namespace indirect_traits
{

template <class T>
struct is_reference_to_const : boost::false_type {};

template <class T>
struct is_reference_to_const<T const &> : boost::true_type {};

template <class T>
struct is_reference_to_function : boost::false_type {};

template <class T>
struct is_reference_to_function<T &> : is_function<T> {};

template <class T>
struct is_pointer_to_function : boost::false_type {};

template <class T>
struct is_pointer_to_function<T *> : is_function<T> {};

template <class T>
struct is_reference_to_member_function_pointer_impl : boost::false_type {};

template <class T>
struct is_reference_to_member_function_pointer_impl<T &> : is_member_function_pointer<typename remove_cv<T>::type> {};

template <class T>
struct is_reference_to_member_function_pointer : is_reference_to_member_function_pointer_impl<T> {};

template <class T>
struct is_reference_to_function_pointer_aux
    : boost::integral_constant<
          bool, is_reference<T>::value
                    && is_pointer_to_function<typename remove_cv<typename remove_reference<T>::type>::type>::value> {};

template <class T>
struct is_reference_to_function_pointer : boost::detail::if_true<is_reference_to_function<T>::value>::template then<
                                              boost::false_type, is_reference_to_function_pointer_aux<T>>::type {};

template <class T>
struct is_reference_to_non_const
    : boost::integral_constant<bool, is_reference<T>::value && !is_reference_to_const<T>::value> {};

template <class T>
struct is_reference_to_volatile : boost::false_type {};

template <class T>
struct is_reference_to_volatile<T volatile &> : boost::true_type {};

template <class T>
struct is_reference_to_pointer : boost::false_type {};

template <class T>
struct is_reference_to_pointer<T *&> : boost::true_type {};

template <class T>
struct is_reference_to_pointer<T *const &> : boost::true_type {};

template <class T>
struct is_reference_to_pointer<T *volatile &> : boost::true_type {};

template <class T>
struct is_reference_to_pointer<T *const volatile &> : boost::true_type {};

template <class T>
struct is_reference_to_class
    : boost::integral_constant<bool,
                               is_reference<T>::value
                                   && is_class<typename remove_cv<typename remove_reference<T>::type>::type>::value> {};

template <class T>
struct is_pointer_to_class
    : boost::integral_constant<
          bool, is_pointer<T>::value && is_class<typename remove_cv<typename remove_pointer<T>::type>::type>::value> {};

} // namespace indirect_traits
} // namespace detail
} // namespace boost

namespace boost
{
namespace iterators
{

using boost::use_default;

namespace detail
{

struct input_output_iterator_tag : std::input_iterator_tag {
    operator std::output_iterator_tag() const
    {
            return std::output_iterator_tag();
    }
};

template <class ValueParam, class Reference>
struct iterator_writability_disabled

    : mpl::or_<is_const<Reference>, boost::detail::indirect_traits::is_reference_to_const<Reference>,
               is_const<ValueParam>>

{};

template <class Traversal, class ValueParam, class Reference>
struct iterator_facade_default_category
    : mpl::eval_if<mpl::and_<is_reference<Reference>, is_convertible<Traversal, forward_traversal_tag>>,
                   mpl::eval_if<is_convertible<Traversal, random_access_traversal_tag>,
                                mpl::identity<std::random_access_iterator_tag>,
                                mpl::if_<is_convertible<Traversal, bidirectional_traversal_tag>,
                                         std::bidirectional_iterator_tag, std::forward_iterator_tag>>,
                   typename mpl::eval_if<mpl::and_<is_convertible<Traversal, single_pass_traversal_tag>

                                                   ,
                                                   is_convertible<Reference, ValueParam>>,
                                         mpl::identity<std::input_iterator_tag>, mpl::identity<Traversal>>> {};

template <class T>
struct is_iterator_category
    : mpl::or_<is_convertible<T, std::input_iterator_tag>, is_convertible<T, std::output_iterator_tag>> {};

template <class T>
struct is_iterator_traversal : is_convertible<T, incrementable_traversal_tag> {};

template <class Category, class Traversal>
struct iterator_category_with_traversal : Category, Traversal {
    enum {
        mpl_assertion_in_line_146 = sizeof(boost::mpl::assertion_failed<false>(boost::mpl::assert_not_arg(
            (void (*)(is_convertible<typename iterator_category_to_traversal<Category>::type, Traversal>))0, 1)))
    };

    enum {
        mpl_assertion_in_line_148 = sizeof(
            boost::mpl::assertion_failed<false>(boost::mpl::assert_arg((void (*)(is_iterator_category<Category>))0, 1)))
    };
    enum {
        mpl_assertion_in_line_149 = sizeof(boost::mpl::assertion_failed<false>(
            boost::mpl::assert_not_arg((void (*)(is_iterator_category<Traversal>))0, 1)))
    };
    enum {
        mpl_assertion_in_line_150 = sizeof(boost::mpl::assertion_failed<false>(
            boost::mpl::assert_not_arg((void (*)(is_iterator_traversal<Category>))0, 1)))
    };

    enum {
        mpl_assertion_in_line_152 = sizeof(boost::mpl::assertion_failed<false>(
            boost::mpl::assert_arg((void (*)(is_iterator_traversal<Traversal>))0, 1)))
    };
};

template <class Traversal, class ValueParam, class Reference>
struct facade_iterator_category_impl {
    enum {
        mpl_assertion_in_line_161 = sizeof(boost::mpl::assertion_failed<false>(
            boost::mpl::assert_not_arg((void (*)(is_iterator_category<Traversal>))0, 1)))
    };

    typedef typename iterator_facade_default_category<Traversal, ValueParam, Reference>::type category;

    typedef typename mpl::if_<is_same<Traversal, typename iterator_category_to_traversal<category>::type>, category,
                              iterator_category_with_traversal<category, Traversal>>::type type;
};

template <class CategoryOrTraversal, class ValueParam, class Reference>
struct facade_iterator_category
    : mpl::eval_if<is_iterator_category<CategoryOrTraversal>, mpl::identity<CategoryOrTraversal>,
                   facade_iterator_category_impl<CategoryOrTraversal, ValueParam, Reference>> {};

} // namespace detail
} // namespace iterators
} // namespace boost

namespace boost
{

namespace iterators
{

template <bool>
struct enabled {
    template <typename T>
    struct base {
            typedef T type;
    };
};

template <>
struct enabled<false> {
    template <typename T>
    struct base {};
};

template <class Cond, class Return>
struct enable_if

    : enabled<(Cond::value)>::template base<Return>

{};

} // namespace iterators

} // namespace boost

namespace boost
{

template <class T>
constexpr inline T *addressof(T &o) noexcept
{
    return __builtin_addressof(o);
}

} // namespace boost

namespace boost
{

template <class T>
const T *addressof(const T &&) = delete;

}

namespace boost
{

template <class T>
struct add_const {
    typedef T const type;
};

template <class T>
struct add_const<T &> {
    typedef T &type;
};

template <class T>
using add_const_t = typename add_const<T>::type;

} // namespace boost

namespace boost
{

template <typename T>
struct add_pointer {
    typedef typename remove_reference<T>::type no_ref_type;
    typedef no_ref_type *type;
};

template <class T>
using add_pointer_t = typename add_pointer<T>::type;

} // namespace boost

namespace boost
{

namespace detail
{

template <typename T>
struct add_reference_impl {
    typedef T &type;
};

template <typename T>
struct add_reference_impl<T &&> {
    typedef T &&type;
};

} // namespace detail

template <class T>
struct add_reference {
    typedef typename boost::detail::add_reference_impl<T>::type type;
};
template <class T>
struct add_reference<T &> {
    typedef T &type;
};

template <>
struct add_reference<void> {
    typedef void type;
};

template <>
struct add_reference<const void> {
    typedef const void type;
};
template <>
struct add_reference<const volatile void> {
    typedef const volatile void type;
};
template <>
struct add_reference<volatile void> {
    typedef volatile void type;
};

template <class T>
using add_reference_t = typename add_reference<T>::type;

} // namespace boost

namespace boost
{

template <class T>
struct add_lvalue_reference {
    typedef typename boost::add_reference<T>::type type;
};

template <class T>
struct add_lvalue_reference<T &&> {
    typedef T &type;
};

template <class T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

} // namespace boost

namespace boost
{

template <class T>
struct remove_const {
    typedef T type;
};
template <class T>
struct remove_const<T const> {
    typedef T type;
};

template <class T, std::size_t N>
struct remove_const<T const [N]> {
    typedef T type[N];
};

template <class T>
struct remove_const<T const[]>
{
    typedef T type[];
};

template <class T>
using remove_const_t = typename remove_const<T>::type;

} // namespace boost

namespace boost
{

template <class T>
struct is_integral : public false_type {};
template <class T>
struct is_integral<const T> : public is_integral<T> {};
template <class T>
struct is_integral<volatile const T> : public is_integral<T> {};
template <class T>
struct is_integral<volatile T> : public is_integral<T> {};

template <>
struct is_integral<unsigned char> : public true_type {};
template <>
struct is_integral<unsigned short> : public true_type {};
template <>
struct is_integral<unsigned int> : public true_type {};
template <>
struct is_integral<unsigned long> : public true_type {};

template <>
struct is_integral<signed char> : public true_type {};
template <>
struct is_integral<short> : public true_type {};
template <>
struct is_integral<int> : public true_type {};
template <>
struct is_integral<long> : public true_type {};

template <>
struct is_integral<char> : public true_type {};
template <>
struct is_integral<bool> : public true_type {};

template <>
struct is_integral<wchar_t> : public true_type {};

template <>
struct is_integral<::boost::ulong_long_type> : public true_type {};
template <>
struct is_integral<::boost::long_long_type> : public true_type {};

template <>
struct is_integral<boost::int128_type> : public true_type {};
template <>
struct is_integral<boost::uint128_type> : public true_type {};

template <>
struct is_integral<char16_t> : public true_type {};

template <>
struct is_integral<char32_t> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct is_floating_point : public false_type {};
template <class T>
struct is_floating_point<const T> : public is_floating_point<T> {};
template <class T>
struct is_floating_point<volatile const T> : public is_floating_point<T> {};
template <class T>
struct is_floating_point<volatile T> : public is_floating_point<T> {};
template <>
struct is_floating_point<float> : public true_type {};
template <>
struct is_floating_point<double> : public true_type {};
template <>
struct is_floating_point<long double> : public true_type {};

} // namespace boost

namespace boost
{

template <class T>
struct is_arithmetic : public integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value> {};

} // namespace boost

namespace boost
{

template <class T>
struct is_enum : public integral_constant<bool, __is_enum(T)> {};

} // namespace boost

namespace boost
{

template <typename T>
struct is_scalar
    : public integral_constant<bool, ::boost::is_arithmetic<T>::value || ::boost::is_enum<T>::value
                                         || ::boost::is_pointer<T>::value || ::boost::is_member_pointer<T>::value> {};

} // namespace boost

namespace boost
{

template <typename T>
struct is_POD;

template <typename T>
struct is_pod
    : public integral_constant<bool, ::boost::is_scalar<T>::value || ::boost::is_void<T>::value || __is_pod(T)> {};

template <typename T, std::size_t sz>
struct is_pod<T[sz]> : public is_pod<T> {};

template <>
struct is_pod<void> : public true_type {};

template <>
struct is_pod<void const> : public true_type {};
template <>
struct is_pod<void const volatile> : public true_type {};
template <>
struct is_pod<void volatile> : public true_type {};

template <class T>
struct is_POD : public is_pod<T> {};

} // namespace boost

namespace boost
{
namespace mpl
{

template <typename Value>
struct always {
    template <typename T1 = na, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply {
            typedef Value type;
    };
};

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename F, typename T1 = na, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
struct apply;

template <typename F>
struct apply0;

template <typename F, typename T1>
struct apply1;

template <typename F, typename T1, typename T2>
struct apply2;

template <typename F, typename T1, typename T2, typename T3>
struct apply3;

template <typename F, typename T1, typename T2, typename T3, typename T4>
struct apply4;

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
struct apply5;

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{
namespace aux
{

template <typename T>
struct type_wrapper {
    typedef T type;
};

template <typename T>
struct wrapped_type;

template <typename T>
struct wrapped_type<type_wrapper<T>> {
    typedef T type;
};

} // namespace aux
} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{
namespace aux
{

template <typename T, typename fallback_ = boost::mpl::bool_<false>>
struct has_apply {
    struct gcc_3_2_wknd {
            template <typename U>
            static boost::mpl::aux::yes_tag test(boost::mpl::aux::type_wrapper<U> const volatile *,
                                                 boost::mpl::aux::type_wrapper<typename U::apply> * = 0);
            static boost::mpl::aux::no_tag test(...);
    };
    typedef boost::mpl::aux::type_wrapper<T> t_;
    static const bool value = sizeof(gcc_3_2_wknd::test(static_cast<t_ *>(0))) == sizeof(boost::mpl::aux::yes_tag);
    typedef boost::mpl::bool_<value> type;
};

} // namespace aux
} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename F

          ,
          typename has_apply_ = typename aux::has_apply<F>::type

          >
struct apply_wrap0

    : F::template apply<> {};

template <typename F>
struct apply_wrap0<F, true_> : F::apply {};

template <typename F, typename T1

          >
struct apply_wrap1

    : F::template apply<T1> {};

template <typename F, typename T1, typename T2

          >
struct apply_wrap2

    : F::template apply<T1, T2> {};

template <typename F, typename T1, typename T2, typename T3

          >
struct apply_wrap3

    : F::template apply<T1, T2, T3> {};

template <typename F, typename T1, typename T2, typename T3, typename T4

          >
struct apply_wrap4

    : F::template apply<T1, T2, T3, T4> {};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5

          >
struct apply_wrap5

    : F::template apply<T1, T2, T3, T4, T5> {};

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename F, typename T1 = na, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
struct bind;

template <typename F>
struct bind0;

template <typename F, typename T1>
struct bind1;

template <typename F, typename T1, typename T2>
struct bind2;

template <typename F, typename T1, typename T2, typename T3>
struct bind3;

template <typename F, typename T1, typename T2, typename T3, typename T4>
struct bind4;

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
struct bind5;

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename T = na>
struct next {
    typedef typename T::next type;
};

template <typename T = na>
struct prior {
    typedef typename T::prior type;
};

template <>
struct next<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : next<T1> {};
};
template <typename Tag>
struct lambda<next<na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef next<na> result_;
    typedef next<na> type;
};
namespace aux
{
template <typename T1>
struct template_arity<next<T1>> : int_<1> {};
template <>
struct template_arity<next<na>> : int_<-1> {};
} // namespace aux
template <>
struct prior<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : prior<T1> {};
};
template <typename Tag>
struct lambda<prior<na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef prior<na> result_;
    typedef prior<na> type;
};
namespace aux
{
template <typename T1>
struct template_arity<prior<T1>> : int_<1> {};
template <>
struct template_arity<prior<na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename T = na, int not_le_ = 0>
struct protect : T {
    typedef protect type;
};

template <>
struct protect<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : protect<T1> {};
};

namespace aux
{
template <typename T1>
struct template_arity<protect<T1>> : int_<1> {};
template <>
struct template_arity<protect<na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

namespace aux
{

template <typename T, typename U1, typename U2, typename U3, typename U4, typename U5>
struct resolve_bind_arg {
    typedef T type;
};

template <typename T, typename Arg>
struct replace_unnamed_arg {
    typedef Arg next;
    typedef T type;
};

template <typename Arg>
struct replace_unnamed_arg<arg<-1>, Arg> {
    typedef typename Arg::next next;
    typedef Arg type;
};

template <int N, typename U1, typename U2, typename U3, typename U4, typename U5>
struct resolve_bind_arg<arg<N>, U1, U2, U3, U4, U5> {
    typedef typename apply_wrap5<mpl::arg<N>, U1, U2, U3, U4, U5>::type type;
};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename U1, typename U2,
          typename U3, typename U4, typename U5>
struct resolve_bind_arg<bind<F, T1, T2, T3, T4, T5>, U1, U2, U3, U4, U5> {
    typedef bind<F, T1, T2, T3, T4, T5> f_;
    typedef typename apply_wrap5<f_, U1, U2, U3, U4, U5>::type type;
};

} // namespace aux

template <typename F>
struct bind0 {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef aux::replace_unnamed_arg<F, mpl::arg<1>> r0;
            typedef typename r0::type a0;
            typedef typename r0::next n1;
            typedef typename aux::resolve_bind_arg<a0, U1, U2, U3, U4, U5>::type f_;

          public:
            typedef typename apply_wrap0<f_>::type type;
    };
};

namespace aux
{

template <typename F, typename U1, typename U2, typename U3, typename U4, typename U5>
struct resolve_bind_arg<bind0<F>, U1, U2, U3, U4, U5> {
    typedef bind0<F> f_;
    typedef typename apply_wrap5<f_, U1, U2, U3, U4, U5>::type type;
};

} // namespace aux

namespace aux
{
template <typename T1>
struct template_arity<bind0<T1>> : int_<1> {};
} // namespace aux

template <typename F>
struct bind<F, na, na, na, na, na> : bind0<F> {};

template <typename F, typename T1>
struct bind1 {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef aux::replace_unnamed_arg<F, mpl::arg<1>> r0;
            typedef typename r0::type a0;
            typedef typename r0::next n1;
            typedef typename aux::resolve_bind_arg<a0, U1, U2, U3, U4, U5>::type f_;

            typedef aux::replace_unnamed_arg<T1, n1> r1;
            typedef typename r1::type a1;
            typedef typename r1::next n2;
            typedef aux::resolve_bind_arg<a1, U1, U2, U3, U4, U5> t1;

          public:
            typedef typename apply_wrap1<f_, typename t1::type>::type type;
    };
};

namespace aux
{

template <typename F, typename T1, typename U1, typename U2, typename U3, typename U4, typename U5>
struct resolve_bind_arg<bind1<F, T1>, U1, U2, U3, U4, U5> {
    typedef bind1<F, T1> f_;
    typedef typename apply_wrap5<f_, U1, U2, U3, U4, U5>::type type;
};

} // namespace aux

namespace aux
{
template <typename T1, typename T2>
struct template_arity<bind1<T1, T2>> : int_<2> {};
} // namespace aux

template <typename F, typename T1>
struct bind<F, T1, na, na, na, na> : bind1<F, T1> {};

template <typename F, typename T1, typename T2>
struct bind2 {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef aux::replace_unnamed_arg<F, mpl::arg<1>> r0;
            typedef typename r0::type a0;
            typedef typename r0::next n1;
            typedef typename aux::resolve_bind_arg<a0, U1, U2, U3, U4, U5>::type f_;

            typedef aux::replace_unnamed_arg<T1, n1> r1;
            typedef typename r1::type a1;
            typedef typename r1::next n2;
            typedef aux::resolve_bind_arg<a1, U1, U2, U3, U4, U5> t1;

            typedef aux::replace_unnamed_arg<T2, n2> r2;
            typedef typename r2::type a2;
            typedef typename r2::next n3;
            typedef aux::resolve_bind_arg<a2, U1, U2, U3, U4, U5> t2;

          public:
            typedef typename apply_wrap2<f_, typename t1::type, typename t2::type>::type type;
    };
};

namespace aux
{

template <typename F, typename T1, typename T2, typename U1, typename U2, typename U3, typename U4, typename U5>
struct resolve_bind_arg<bind2<F, T1, T2>, U1, U2, U3, U4, U5> {
    typedef bind2<F, T1, T2> f_;
    typedef typename apply_wrap5<f_, U1, U2, U3, U4, U5>::type type;
};

} // namespace aux

namespace aux
{
template <typename T1, typename T2, typename T3>
struct template_arity<bind2<T1, T2, T3>> : int_<3> {};
} // namespace aux

template <typename F, typename T1, typename T2>
struct bind<F, T1, T2, na, na, na> : bind2<F, T1, T2> {};

template <typename F, typename T1, typename T2, typename T3>
struct bind3 {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef aux::replace_unnamed_arg<F, mpl::arg<1>> r0;
            typedef typename r0::type a0;
            typedef typename r0::next n1;
            typedef typename aux::resolve_bind_arg<a0, U1, U2, U3, U4, U5>::type f_;

            typedef aux::replace_unnamed_arg<T1, n1> r1;
            typedef typename r1::type a1;
            typedef typename r1::next n2;
            typedef aux::resolve_bind_arg<a1, U1, U2, U3, U4, U5> t1;

            typedef aux::replace_unnamed_arg<T2, n2> r2;
            typedef typename r2::type a2;
            typedef typename r2::next n3;
            typedef aux::resolve_bind_arg<a2, U1, U2, U3, U4, U5> t2;

            typedef aux::replace_unnamed_arg<T3, n3> r3;
            typedef typename r3::type a3;
            typedef typename r3::next n4;
            typedef aux::resolve_bind_arg<a3, U1, U2, U3, U4, U5> t3;

          public:
            typedef typename apply_wrap3<f_, typename t1::type, typename t2::type, typename t3::type>::type type;
    };
};

namespace aux
{

template <typename F, typename T1, typename T2, typename T3, typename U1, typename U2, typename U3, typename U4,
          typename U5>
struct resolve_bind_arg<bind3<F, T1, T2, T3>, U1, U2, U3, U4, U5> {
    typedef bind3<F, T1, T2, T3> f_;
    typedef typename apply_wrap5<f_, U1, U2, U3, U4, U5>::type type;
};

} // namespace aux

namespace aux
{
template <typename T1, typename T2, typename T3, typename T4>
struct template_arity<bind3<T1, T2, T3, T4>> : int_<4> {};
} // namespace aux

template <typename F, typename T1, typename T2, typename T3>
struct bind<F, T1, T2, T3, na, na> : bind3<F, T1, T2, T3> {};

template <typename F, typename T1, typename T2, typename T3, typename T4>
struct bind4 {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef aux::replace_unnamed_arg<F, mpl::arg<1>> r0;
            typedef typename r0::type a0;
            typedef typename r0::next n1;
            typedef typename aux::resolve_bind_arg<a0, U1, U2, U3, U4, U5>::type f_;

            typedef aux::replace_unnamed_arg<T1, n1> r1;
            typedef typename r1::type a1;
            typedef typename r1::next n2;
            typedef aux::resolve_bind_arg<a1, U1, U2, U3, U4, U5> t1;

            typedef aux::replace_unnamed_arg<T2, n2> r2;
            typedef typename r2::type a2;
            typedef typename r2::next n3;
            typedef aux::resolve_bind_arg<a2, U1, U2, U3, U4, U5> t2;

            typedef aux::replace_unnamed_arg<T3, n3> r3;
            typedef typename r3::type a3;
            typedef typename r3::next n4;
            typedef aux::resolve_bind_arg<a3, U1, U2, U3, U4, U5> t3;

            typedef aux::replace_unnamed_arg<T4, n4> r4;
            typedef typename r4::type a4;
            typedef typename r4::next n5;
            typedef aux::resolve_bind_arg<a4, U1, U2, U3, U4, U5> t4;

          public:
            typedef typename apply_wrap4<f_, typename t1::type, typename t2::type, typename t3::type,
                                         typename t4::type>::type type;
    };
};

namespace aux
{

template <typename F, typename T1, typename T2, typename T3, typename T4, typename U1, typename U2, typename U3,
          typename U4, typename U5>
struct resolve_bind_arg<bind4<F, T1, T2, T3, T4>, U1, U2, U3, U4, U5> {
    typedef bind4<F, T1, T2, T3, T4> f_;
    typedef typename apply_wrap5<f_, U1, U2, U3, U4, U5>::type type;
};

} // namespace aux

namespace aux
{
template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct template_arity<bind4<T1, T2, T3, T4, T5>> : int_<5> {};
} // namespace aux

template <typename F, typename T1, typename T2, typename T3, typename T4>
struct bind<F, T1, T2, T3, T4, na> : bind4<F, T1, T2, T3, T4> {};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
struct bind5 {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef aux::replace_unnamed_arg<F, mpl::arg<1>> r0;
            typedef typename r0::type a0;
            typedef typename r0::next n1;
            typedef typename aux::resolve_bind_arg<a0, U1, U2, U3, U4, U5>::type f_;

            typedef aux::replace_unnamed_arg<T1, n1> r1;
            typedef typename r1::type a1;
            typedef typename r1::next n2;
            typedef aux::resolve_bind_arg<a1, U1, U2, U3, U4, U5> t1;

            typedef aux::replace_unnamed_arg<T2, n2> r2;
            typedef typename r2::type a2;
            typedef typename r2::next n3;
            typedef aux::resolve_bind_arg<a2, U1, U2, U3, U4, U5> t2;

            typedef aux::replace_unnamed_arg<T3, n3> r3;
            typedef typename r3::type a3;
            typedef typename r3::next n4;
            typedef aux::resolve_bind_arg<a3, U1, U2, U3, U4, U5> t3;

            typedef aux::replace_unnamed_arg<T4, n4> r4;
            typedef typename r4::type a4;
            typedef typename r4::next n5;
            typedef aux::resolve_bind_arg<a4, U1, U2, U3, U4, U5> t4;

            typedef aux::replace_unnamed_arg<T5, n5> r5;
            typedef typename r5::type a5;
            typedef typename r5::next n6;
            typedef aux::resolve_bind_arg<a5, U1, U2, U3, U4, U5> t5;

          public:
            typedef typename apply_wrap5<f_, typename t1::type, typename t2::type, typename t3::type, typename t4::type,
                                         typename t5::type>::type type;
    };
};

namespace aux
{

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename U1, typename U2,
          typename U3, typename U4, typename U5>
struct resolve_bind_arg<bind5<F, T1, T2, T3, T4, T5>, U1, U2, U3, U4, U5> {
    typedef bind5<F, T1, T2, T3, T4, T5> f_;
    typedef typename apply_wrap5<f_, U1, U2, U3, U4, U5>::type type;
};

} // namespace aux

namespace aux
{
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
struct template_arity<bind5<T1, T2, T3, T4, T5, T6>> : int_<6> {};
} // namespace aux

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
struct bind : bind5<F, T1, T2, T3, T4, T5> {};

template <template <typename T1, typename T2, typename T3> class F, typename Tag>
struct quote3;

template <typename T1, typename T2, typename T3>
struct if_;

template <typename Tag, typename T1, typename T2, typename T3>
struct bind3<quote3<if_, Tag>, T1, T2, T3> {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef mpl::arg<1> n1;
            typedef aux::replace_unnamed_arg<T1, n1> r1;
            typedef typename r1::type a1;
            typedef typename r1::next n2;
            typedef aux::resolve_bind_arg<a1, U1, U2, U3, U4, U5> t1;

            typedef aux::replace_unnamed_arg<T2, n2> r2;
            typedef typename r2::type a2;
            typedef typename r2::next n3;
            typedef aux::resolve_bind_arg<a2, U1, U2, U3, U4, U5> t2;

            typedef aux::replace_unnamed_arg<T3, n3> r3;
            typedef typename r3::type a3;
            typedef typename r3::next n4;
            typedef aux::resolve_bind_arg<a3, U1, U2, U3, U4, U5> t3;

            typedef typename if_<typename t1::type, t2, t3>::type f_;

          public:
            typedef typename f_::type type;
    };
};

template <template <typename T1, typename T2, typename T3> class F, typename Tag>
struct quote3;

template <typename T1, typename T2, typename T3>
struct eval_if;

template <typename Tag, typename T1, typename T2, typename T3>
struct bind3<quote3<eval_if, Tag>, T1, T2, T3> {
    template <typename U1 = na, typename U2 = na, typename U3 = na, typename U4 = na, typename U5 = na>
    struct apply {
          private:
            typedef mpl::arg<1> n1;
            typedef aux::replace_unnamed_arg<T1, n1> r1;
            typedef typename r1::type a1;
            typedef typename r1::next n2;
            typedef aux::resolve_bind_arg<a1, U1, U2, U3, U4, U5> t1;

            typedef aux::replace_unnamed_arg<T2, n2> r2;
            typedef typename r2::type a2;
            typedef typename r2::next n3;
            typedef aux::resolve_bind_arg<a2, U1, U2, U3, U4, U5> t2;

            typedef aux::replace_unnamed_arg<T3, n3> r3;
            typedef typename r3::type a3;
            typedef typename r3::next n4;
            typedef aux::resolve_bind_arg<a3, U1, U2, U3, U4, U5> t3;

            typedef typename eval_if<typename t1::type, t2, t3>::type f_;

          public:
            typedef typename f_::type type;
    };
};

} // namespace mpl
} // namespace boost

namespace mpl_
{

struct void_ {
    typedef void_ type;
};

} // namespace mpl_

namespace boost
{
namespace mpl
{

template <typename T>
struct is_void_ : false_ {};

template <>
struct is_void_<void_> : true_ {};

template <typename T>
struct is_not_void_ : true_ {};

template <>
struct is_not_void_<void_> : false_ {};

template <>
struct is_void_<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : is_void_<T1> {};
};
template <typename Tag>
struct lambda<is_void_<na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef is_void_<na> result_;
    typedef is_void_<na> type;
};
namespace aux
{
template <typename T1>
struct template_arity<is_void_<T1>> : int_<1> {};
template <>
struct template_arity<is_void_<na>> : int_<-1> {};
} // namespace aux
template <>
struct is_not_void_<na> {
    template <typename T1, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : is_not_void_<T1> {};
};
template <typename Tag>
struct lambda<is_not_void_<na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef is_not_void_<na> result_;
    typedef is_not_void_<na> type;
};
namespace aux
{
template <typename T1>
struct template_arity<is_not_void_<T1>> : int_<1> {};
template <>
struct template_arity<is_not_void_<na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{
namespace aux
{
template <typename T, typename fallback_ = boost::mpl::bool_<true>>
struct has_type {
    struct gcc_3_2_wknd {
            template <typename U>
            static boost::mpl::aux::yes_tag test(boost::mpl::aux::type_wrapper<U> const volatile *,
                                                 boost::mpl::aux::type_wrapper<typename U::type> * = 0);
            static boost::mpl::aux::no_tag test(...);
    };
    typedef boost::mpl::aux::type_wrapper<T> t_;
    static const bool value = sizeof(gcc_3_2_wknd::test(static_cast<t_ *>(0))) == sizeof(boost::mpl::aux::yes_tag);
    typedef boost::mpl::bool_<value> type;
};
} // namespace aux
} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename T, bool has_type_>
struct quote_impl {
    typedef typename T::type type;
};

template <typename T>
struct quote_impl<T, false> {
    typedef T type;
};

template <template <typename P1> class F, typename Tag = void_>
struct quote1 {
    template <typename U1>
    struct apply

        : quote_impl<F<U1>, aux::has_type<F<U1>>::value>

    {};
};

template <template <typename P1, typename P2> class F, typename Tag = void_>
struct quote2 {
    template <typename U1, typename U2>
    struct apply

        : quote_impl<F<U1, U2>, aux::has_type<F<U1, U2>>::value>

    {};
};

template <template <typename P1, typename P2, typename P3> class F, typename Tag = void_>
struct quote3 {
    template <typename U1, typename U2, typename U3>
    struct apply

        : quote_impl<F<U1, U2, U3>, aux::has_type<F<U1, U2, U3>>::value>

    {};
};

template <template <typename P1, typename P2, typename P3, typename P4> class F, typename Tag = void_>
struct quote4 {
    template <typename U1, typename U2, typename U3, typename U4>
    struct apply

        : quote_impl<F<U1, U2, U3, U4>, aux::has_type<F<U1, U2, U3, U4>>::value>

    {};
};

template <template <typename P1, typename P2, typename P3, typename P4, typename P5> class F, typename Tag = void_>
struct quote5 {
    template <typename U1, typename U2, typename U3, typename U4, typename U5>
    struct apply

        : quote_impl<F<U1, U2, U3, U4, U5>, aux::has_type<F<U1, U2, U3, U4, U5>>::value>

    {};
};

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{
namespace aux
{
template <int N>
struct arity_tag {
    typedef char (&type)[(unsigned)N + 1];
};

template <int C1, int C2, int C3, int C4, int C5, int C6>
struct max_arity {
    static const int value =
        (C6 > 0 ? C6 : (C5 > 0 ? C5 : (C4 > 0 ? C4 : (C3 > 0 ? C3 : (C2 > 0 ? C2 : (C1 > 0 ? C1 : -1))))));
};

arity_tag<0>::type arity_helper(...);

template <template <typename P1> class F, typename T1>
typename arity_tag<1>::type arity_helper(type_wrapper<F<T1>>, arity_tag<1>);

template <template <typename P1, typename P2> class F, typename T1, typename T2>
typename arity_tag<2>::type arity_helper(type_wrapper<F<T1, T2>>, arity_tag<2>);

template <template <typename P1, typename P2, typename P3> class F, typename T1, typename T2, typename T3>
typename arity_tag<3>::type arity_helper(type_wrapper<F<T1, T2, T3>>, arity_tag<3>);

template <template <typename P1, typename P2, typename P3, typename P4> class F, typename T1, typename T2, typename T3,
          typename T4>
typename arity_tag<4>::type arity_helper(type_wrapper<F<T1, T2, T3, T4>>, arity_tag<4>);

template <template <typename P1, typename P2, typename P3, typename P4, typename P5> class F, typename T1, typename T2,
          typename T3, typename T4, typename T5>
typename arity_tag<5>::type arity_helper(type_wrapper<F<T1, T2, T3, T4, T5>>, arity_tag<5>);

template <template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6> class F, typename T1,
          typename T2, typename T3, typename T4, typename T5, typename T6>
typename arity_tag<6>::type arity_helper(type_wrapper<F<T1, T2, T3, T4, T5, T6>>, arity_tag<6>);
template <typename F, int N>
struct template_arity_impl {
    static const int value = sizeof(::boost::mpl::aux::arity_helper(type_wrapper<F>(), arity_tag<N>())) - 1;
};

template <typename F>
struct template_arity {
    static const int value = (max_arity<template_arity_impl<F, 1>::value, template_arity_impl<F, 2>::value,
                                        template_arity_impl<F, 3>::value, template_arity_impl<F, 4>::value,
                                        template_arity_impl<F, 5>::value, template_arity_impl<F, 6>::value>::value);

    typedef mpl::int_<value> type;
};

} // namespace aux
} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

namespace aux
{

template <bool C1 = false, bool C2 = false, bool C3 = false, bool C4 = false, bool C5 = false>
struct lambda_or : true_ {};

template <>
struct lambda_or<false, false, false, false, false> : false_ {};

} // namespace aux

template <typename T, typename Tag, typename Arity>
struct lambda {
    typedef false_ is_le;
    typedef T result_;
    typedef T type;
};

template <typename T>
struct is_lambda_expression : lambda<T>::is_le {};

template <int N, typename Tag>
struct lambda<arg<N>, Tag, int_<-1>> {
    typedef true_ is_le;
    typedef mpl::arg<N> result_;
    typedef mpl::protect<result_> type;
};

template <typename F, typename Tag>
struct lambda<bind0<F>, Tag, int_<1>> {
    typedef false_ is_le;
    typedef bind0<F> result_;

    typedef result_ type;
};

namespace aux
{

template <typename IsLE, typename Tag, template <typename P1> class F, typename L1>
struct le_result1 {
    typedef F<typename L1::type> result_;

    typedef result_ type;
};

template <typename Tag, template <typename P1> class F, typename L1>
struct le_result1<true_, Tag, F, L1> {
    typedef bind1<quote1<F, Tag>, typename L1::result_> result_;

    typedef mpl::protect<result_> type;
};

} // namespace aux

template <template <typename P1> class F, typename T1, typename Tag>
struct lambda<F<T1>, Tag, int_<1>> {
    typedef lambda<T1, Tag> l1;
    typedef typename l1::is_le is_le1;
    typedef typename aux::lambda_or<is_le1::value>::type is_le;

    typedef aux::le_result1<is_le, Tag, F, l1> le_result_;

    typedef typename le_result_::result_ result_;
    typedef typename le_result_::type type;
};

template <typename F, typename T1, typename Tag>
struct lambda<bind1<F, T1>, Tag, int_<2>> {
    typedef false_ is_le;
    typedef bind1<F, T1> result_;

    typedef result_ type;
};

namespace aux
{

template <typename IsLE, typename Tag, template <typename P1, typename P2> class F, typename L1, typename L2>
struct le_result2 {
    typedef F<typename L1::type, typename L2::type> result_;

    typedef result_ type;
};

template <typename Tag, template <typename P1, typename P2> class F, typename L1, typename L2>
struct le_result2<true_, Tag, F, L1, L2> {
    typedef bind2<quote2<F, Tag>, typename L1::result_, typename L2::result_> result_;

    typedef mpl::protect<result_> type;
};

} // namespace aux

template <template <typename P1, typename P2> class F, typename T1, typename T2, typename Tag>
struct lambda<F<T1, T2>, Tag, int_<2>> {
    typedef lambda<T1, Tag> l1;
    typedef lambda<T2, Tag> l2;

    typedef typename l1::is_le is_le1;
    typedef typename l2::is_le is_le2;

    typedef typename aux::lambda_or<is_le1::value, is_le2::value>::type is_le;

    typedef aux::le_result2<is_le, Tag, F, l1, l2> le_result_;

    typedef typename le_result_::result_ result_;
    typedef typename le_result_::type type;
};

template <typename F, typename T1, typename T2, typename Tag>
struct lambda<bind2<F, T1, T2>, Tag, int_<3>> {
    typedef false_ is_le;
    typedef bind2<F, T1, T2> result_;

    typedef result_ type;
};

namespace aux
{

template <typename IsLE, typename Tag, template <typename P1, typename P2, typename P3> class F, typename L1,
          typename L2, typename L3>
struct le_result3 {
    typedef F<typename L1::type, typename L2::type, typename L3::type> result_;

    typedef result_ type;
};

template <typename Tag, template <typename P1, typename P2, typename P3> class F, typename L1, typename L2, typename L3>
struct le_result3<true_, Tag, F, L1, L2, L3> {
    typedef bind3<quote3<F, Tag>, typename L1::result_, typename L2::result_, typename L3::result_> result_;

    typedef mpl::protect<result_> type;
};

} // namespace aux

template <template <typename P1, typename P2, typename P3> class F, typename T1, typename T2, typename T3, typename Tag>
struct lambda<F<T1, T2, T3>, Tag, int_<3>> {
    typedef lambda<T1, Tag> l1;
    typedef lambda<T2, Tag> l2;
    typedef lambda<T3, Tag> l3;

    typedef typename l1::is_le is_le1;
    typedef typename l2::is_le is_le2;
    typedef typename l3::is_le is_le3;

    typedef typename aux::lambda_or<is_le1::value, is_le2::value, is_le3::value>::type is_le;

    typedef aux::le_result3<is_le, Tag, F, l1, l2, l3> le_result_;

    typedef typename le_result_::result_ result_;
    typedef typename le_result_::type type;
};

template <typename F, typename T1, typename T2, typename T3, typename Tag>
struct lambda<bind3<F, T1, T2, T3>, Tag, int_<4>> {
    typedef false_ is_le;
    typedef bind3<F, T1, T2, T3> result_;

    typedef result_ type;
};

namespace aux
{
template <typename IsLE, typename Tag, template <typename P1, typename P2, typename P3, typename P4> class F,
          typename L1, typename L2, typename L3, typename L4>
struct le_result4 {
    typedef F<typename L1::type, typename L2::type, typename L3::type, typename L4::type> result_;

    typedef result_ type;
};

template <typename Tag, template <typename P1, typename P2, typename P3, typename P4> class F, typename L1, typename L2,
          typename L3, typename L4>
struct le_result4<true_, Tag, F, L1, L2, L3, L4> {
    typedef bind4<quote4<F, Tag>, typename L1::result_, typename L2::result_, typename L3::result_,
                  typename L4::result_>
        result_;

    typedef mpl::protect<result_> type;
};
} // namespace aux

template <template <typename P1, typename P2, typename P3, typename P4> class F, typename T1, typename T2, typename T3,
          typename T4, typename Tag>
struct lambda<F<T1, T2, T3, T4>, Tag, int_<4>> {
    typedef lambda<T1, Tag> l1;
    typedef lambda<T2, Tag> l2;
    typedef lambda<T3, Tag> l3;
    typedef lambda<T4, Tag> l4;

    typedef typename l1::is_le is_le1;
    typedef typename l2::is_le is_le2;
    typedef typename l3::is_le is_le3;
    typedef typename l4::is_le is_le4;

    typedef typename aux::lambda_or<is_le1::value, is_le2::value, is_le3::value, is_le4::value>::type is_le;

    typedef aux::le_result4<is_le, Tag, F, l1, l2, l3, l4> le_result_;

    typedef typename le_result_::result_ result_;
    typedef typename le_result_::type type;
};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename Tag>
struct lambda<bind4<F, T1, T2, T3, T4>, Tag, int_<5>> {
    typedef false_ is_le;
    typedef bind4<F, T1, T2, T3, T4> result_;

    typedef result_ type;
};

namespace aux
{

template <typename IsLE, typename Tag,
          template <typename P1, typename P2, typename P3, typename P4, typename P5> class F, typename L1, typename L2,
          typename L3, typename L4, typename L5>
struct le_result5 {
    typedef F<typename L1::type, typename L2::type, typename L3::type, typename L4::type, typename L5::type> result_;

    typedef result_ type;
};

template <typename Tag, template <typename P1, typename P2, typename P3, typename P4, typename P5> class F, typename L1,
          typename L2, typename L3, typename L4, typename L5>
struct le_result5<true_, Tag, F, L1, L2, L3, L4, L5> {
    typedef bind5<quote5<F, Tag>, typename L1::result_, typename L2::result_, typename L3::result_,
                  typename L4::result_, typename L5::result_>
        result_;

    typedef mpl::protect<result_> type;
};

} // namespace aux

template <template <typename P1, typename P2, typename P3, typename P4, typename P5> class F, typename T1, typename T2,
          typename T3, typename T4, typename T5, typename Tag>
struct lambda<F<T1, T2, T3, T4, T5>, Tag, int_<5>> {
    typedef lambda<T1, Tag> l1;
    typedef lambda<T2, Tag> l2;
    typedef lambda<T3, Tag> l3;
    typedef lambda<T4, Tag> l4;
    typedef lambda<T5, Tag> l5;

    typedef typename l1::is_le is_le1;
    typedef typename l2::is_le is_le2;
    typedef typename l3::is_le is_le3;
    typedef typename l4::is_le is_le4;
    typedef typename l5::is_le is_le5;

    typedef
        typename aux::lambda_or<is_le1::value, is_le2::value, is_le3::value, is_le4::value, is_le5::value>::type is_le;

    typedef aux::le_result5<is_le, Tag, F, l1, l2, l3, l4, l5> le_result_;

    typedef typename le_result_::result_ result_;
    typedef typename le_result_::type type;
};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename Tag>
struct lambda<bind5<F, T1, T2, T3, T4, T5>, Tag, int_<6>> {
    typedef false_ is_le;
    typedef bind5<F, T1, T2, T3, T4, T5> result_;

    typedef result_ type;
};

template <typename T, typename Tag>
struct lambda<mpl::protect<T>, Tag, int_<1>> {
    typedef false_ is_le;
    typedef mpl::protect<T> result_;
    typedef result_ type;
};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename Tag>
struct lambda<bind<F, T1, T2, T3, T4, T5>, Tag, int_<6>> {
    typedef false_ is_le;
    typedef bind<F, T1, T2, T3, T4, T5> result_;
    typedef result_ type;
};

template <typename F, typename Tag1, typename Tag2, typename Arity>
struct lambda<lambda<F, Tag1, Arity>, Tag2, int_<3>> {
    typedef lambda<F, Tag2> l1;
    typedef lambda<Tag1, Tag2> l2;
    typedef typename l1::is_le is_le;
    typedef bind1<quote1<aux::template_arity>, typename l1::result_> arity_;
    typedef lambda<typename if_<is_le, arity_, Arity>::type, Tag2> l3;
    typedef aux::le_result3<is_le, Tag2, mpl::lambda, l1, l2, l3> le_result_;
    typedef typename le_result_::result_ result_;
    typedef typename le_result_::type type;
};

template <>
struct lambda<na, na> {
    template <typename T1, typename T2, typename T3 = na, typename T4 = na, typename T5 = na>
    struct apply : lambda<T1, T2> {};
};
template <typename Tag>
struct lambda<lambda<na, na>, Tag, int_<-1>> {
    typedef false_ is_le;
    typedef lambda<na, na> result_;
    typedef lambda<na, na> type;
};
namespace aux
{
template <typename T1, typename T2, typename T3>
struct template_arity<lambda<T1, T2, T3>> : int_<3> {};
template <>
struct template_arity<lambda<na, na>> : int_<-1> {};
} // namespace aux

} // namespace mpl
} // namespace boost

namespace boost
{
namespace mpl
{

template <typename F>
struct apply0

    : apply_wrap0<typename lambda<F>::type

                  > {};

template <typename F>
struct apply<F, na, na, na, na, na> : apply0<F> {};

template <typename F, typename T1>
struct apply1

    : apply_wrap1<typename lambda<F>::type, T1> {};

template <typename F, typename T1>
struct apply<F, T1, na, na, na, na> : apply1<F, T1> {};

template <typename F, typename T1, typename T2>
struct apply2

    : apply_wrap2<typename lambda<F>::type, T1, T2> {};

template <typename F, typename T1, typename T2>
struct apply<F, T1, T2, na, na, na> : apply2<F, T1, T2> {};

template <typename F, typename T1, typename T2, typename T3>
struct apply3

    : apply_wrap3<typename lambda<F>::type, T1, T2, T3> {};

template <typename F, typename T1, typename T2, typename T3>
struct apply<F, T1, T2, T3, na, na> : apply3<F, T1, T2, T3> {};

template <typename F, typename T1, typename T2, typename T3, typename T4>
struct apply4

    : apply_wrap4<typename lambda<F>::type, T1, T2, T3, T4> {};

template <typename F, typename T1, typename T2, typename T3, typename T4>
struct apply<F, T1, T2, T3, T4, na> : apply4<F, T1, T2, T3, T4> {};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
struct apply5

    : apply_wrap5<typename lambda<F>::type, T1, T2, T3, T4, T5> {};

template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
struct apply : apply5<F, T1, T2, T3, T4, T5> {};

} // namespace mpl
} // namespace boost

namespace boost
{
namespace iterators
{

template <class I, class V, class TC, class R, class D>
class iterator_facade;

namespace detail
{

struct always_bool2 {
    template <class T, class U>
    struct apply {
            typedef bool type;
    };
};

template <typename CategoryOrTraversal, typename Required>
struct is_traversal_at_least
    : public boost::is_convertible<typename iterator_category_to_traversal<CategoryOrTraversal>::type, Required> {};

template <class Facade1, class Facade2, class Return>
struct enable_if_interoperable : public boost::iterators::enable_if<is_interoperable<Facade1, Facade2>, Return> {};

template <class Facade1, class Facade2, class Return>
struct enable_if_interoperable_and_random_access_traversal
    : public boost::iterators::enable_if<
          mpl::and_<is_interoperable<Facade1, Facade2>,
                    is_traversal_at_least<typename iterator_category<Facade1>::type, random_access_traversal_tag>,
                    is_traversal_at_least<typename iterator_category<Facade2>::type, random_access_traversal_tag>>,
          Return> {};

template <class ValueParam, class CategoryOrTraversal, class Reference, class Difference>
struct iterator_facade_types {
    typedef typename facade_iterator_category<CategoryOrTraversal, ValueParam, Reference>::type iterator_category;

    typedef typename remove_const<ValueParam>::type value_type;

    typedef typename mpl::eval_if<boost::iterators::detail::iterator_writability_disabled<ValueParam, Reference>,
                                  add_pointer<const value_type>, add_pointer<value_type>>::type pointer;
};

template <class Iterator>
class postfix_increment_proxy {
    typedef typename iterator_value<Iterator>::type value_type;

  public:
    explicit postfix_increment_proxy(Iterator const &x) : stored_value(*x) {}

    value_type &operator*() const
    {
            return this->stored_value;
    }

  private:
    mutable value_type stored_value;
};

template <class Iterator>
class writable_postfix_increment_proxy {
    typedef typename iterator_value<Iterator>::type value_type;

  public:
    explicit writable_postfix_increment_proxy(Iterator const &x) : stored_value(*x), stored_iterator(x) {}

    writable_postfix_increment_proxy const &operator*() const
    {
            return *this;
    }

    operator value_type &() const
    {
            return stored_value;
    }

    template <class T>
    T const &operator=(T const &x) const
    {
            *this->stored_iterator = x;
            return x;
    }

    template <class T>
    T &operator=(T &x) const
    {
            *this->stored_iterator = x;
            return x;
    }

    operator Iterator const &() const
    {
            return stored_iterator;
    }

  private:
    mutable value_type stored_value;
    Iterator stored_iterator;
};

template <class Reference, class Value>
struct is_non_proxy_reference
    : is_convertible<typename remove_reference<Reference>::type const volatile *, Value const volatile *> {};

template <class Iterator, class Value, class Reference, class CategoryOrTraversal>
struct postfix_increment_result
    : mpl::eval_if<mpl::and_<

                       is_convertible<Reference

                                      ,
                                      typename add_lvalue_reference<Value const>::type>

                       ,
                       mpl::not_<is_convertible<typename iterator_category_to_traversal<CategoryOrTraversal>::type,
                                                forward_traversal_tag>>>,
                   mpl::if_<is_non_proxy_reference<Reference, Value>, postfix_increment_proxy<Iterator>,
                            writable_postfix_increment_proxy<Iterator>>,
                   mpl::identity<Iterator>> {};

template <class Reference, class Pointer>
struct operator_arrow_dispatch {
    struct proxy {
            explicit proxy(Reference const &x) : m_ref(x) {}
            Reference *operator->()
            {
                return boost::addressof(m_ref);
            }

            operator Reference *()
            {
                return boost::addressof(m_ref);
            }
            Reference m_ref;
    };
    typedef proxy result_type;
    static result_type apply(Reference const &x)
    {
            return result_type(x);
    }
};

template <class T, class Pointer>
struct operator_arrow_dispatch<T &, Pointer> {
    typedef Pointer result_type;
    static result_type apply(T &x)
    {
            return boost::addressof(x);
    }
};

template <class Iterator>
class operator_brackets_proxy {
    typedef typename Iterator::reference reference;
    typedef typename Iterator::value_type value_type;

  public:
    operator_brackets_proxy(Iterator const &iter) : m_iter(iter) {}

    operator reference() const
    {
            return *m_iter;
    }

    operator_brackets_proxy &operator=(value_type const &val)
    {
            *m_iter = val;
            return *this;
    }

  private:
    Iterator m_iter;
};

template <class ValueType, class Reference>
struct use_operator_brackets_proxy
    : mpl::not_<mpl::and_<

          boost::is_POD<ValueType>, iterator_writability_disabled<ValueType, Reference>>> {};

template <class Iterator, class Value, class Reference>
struct operator_brackets_result {
    typedef
        typename mpl::if_<use_operator_brackets_proxy<Value, Reference>, operator_brackets_proxy<Iterator>, Value>::type
            type;
};

template <class Iterator>
operator_brackets_proxy<Iterator> make_operator_brackets_result(Iterator const &iter, mpl::true_)
{
    return operator_brackets_proxy<Iterator>(iter);
}

template <class Iterator>
typename Iterator::value_type make_operator_brackets_result(Iterator const &iter, mpl::false_)
{
    return *iter;
}

struct choose_difference_type {
    template <class I1, class I2>
    struct apply :

        mpl::eval_if<is_convertible<I2, I1>, iterator_difference<I1>, iterator_difference<I2>>

    {};
};

template <class Derived, class Value, class CategoryOrTraversal, class Reference, class Difference,
          bool IsBidirectionalTraversal, bool IsRandomAccessTraversal>
class iterator_facade_base;

} // namespace detail

class iterator_core_access {
    template <class I, class V, class TC, class R, class D>
    friend class iterator_facade;
    template <class I, class V, class TC, class R, class D, bool IsBidirectionalTraversal, bool IsRandomAccessTraversal>
    friend class detail::iterator_facade_base;

    template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2,
              class TC2, class Reference2, class Difference2>
    friend typename boost::iterators::detail::enable_if_interoperable<
        Derived1, Derived2,
        typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
    operator==(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
               iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs);
    template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2,
              class TC2, class Reference2, class Difference2>
    friend typename boost::iterators::detail::enable_if_interoperable<
        Derived1, Derived2,
        typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
    operator!=(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
               iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs);

    template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2,
              class TC2, class Reference2, class Difference2>
    friend typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
        Derived1, Derived2,
        typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
    operator<(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
              iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs);
    template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2,
              class TC2, class Reference2, class Difference2>
    friend typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
        Derived1, Derived2,
        typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
    operator>(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
              iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs);
    template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2,
              class TC2, class Reference2, class Difference2>
    friend typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
        Derived1, Derived2,
        typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
    operator<=(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
               iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs);
    template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2,
              class TC2, class Reference2, class Difference2>
    friend typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
        Derived1, Derived2,
        typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
    operator>=(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
               iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs);

    template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2,
              class TC2, class Reference2, class Difference2>
    friend typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
        Derived1, Derived2,
        typename mpl::apply2<boost::iterators::detail::choose_difference_type, Derived1, Derived2>::type>::type
    operator-(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
              iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)

        ;

    template <class Derived, class V, class TC, class R, class D>
    friend inline typename boost::iterators::enable_if<
        boost::iterators::detail::is_traversal_at_least<TC, boost::iterators::random_access_traversal_tag>,
        Derived>::type
    operator+(iterator_facade<Derived, V, TC, R, D> const &, typename Derived::difference_type)

        ;

    template <class Derived, class V, class TC, class R, class D>
    friend inline typename boost::iterators::enable_if<
        boost::iterators::detail::is_traversal_at_least<TC, boost::iterators::random_access_traversal_tag>,
        Derived>::type
    operator+(typename Derived::difference_type, iterator_facade<Derived, V, TC, R, D> const &)

        ;

    template <class Facade>
    static typename Facade::reference dereference(Facade const &f)
    {
            return f.dereference();
    }

    template <class Facade>
    static void increment(Facade &f)
    {
            f.increment();
    }

    template <class Facade>
    static void decrement(Facade &f)
    {
            f.decrement();
    }

    template <class Facade1, class Facade2>
    static bool equal(Facade1 const &f1, Facade2 const &f2, mpl::true_)
    {
            return f1.equal(f2);
    }

    template <class Facade1, class Facade2>
    static bool equal(Facade1 const &f1, Facade2 const &f2, mpl::false_)
    {
            return f2.equal(f1);
    }

    template <class Facade>
    static void advance(Facade &f, typename Facade::difference_type n)
    {
            f.advance(n);
    }

    template <class Facade1, class Facade2>
    static typename Facade1::difference_type distance_from(Facade1 const &f1, Facade2 const &f2, mpl::true_)
    {
            return -f1.distance_to(f2);
    }

    template <class Facade1, class Facade2>
    static typename Facade2::difference_type distance_from(Facade1 const &f1, Facade2 const &f2, mpl::false_)
    {
            return f2.distance_to(f1);
    }

    template <class I, class V, class TC, class R, class D>
    static I &derived(iterator_facade<I, V, TC, R, D> &facade)
    {
            return *static_cast<I *>(&facade);
    }

    template <class I, class V, class TC, class R, class D>
    static I const &derived(iterator_facade<I, V, TC, R, D> const &facade)
    {
            return *static_cast<I const *>(&facade);
    }

    iterator_core_access() = delete;
};

namespace detail
{

template <class Derived, class Value, class CategoryOrTraversal, class Reference, class Difference>
class iterator_facade_base<Derived, Value, CategoryOrTraversal, Reference, Difference, false, false>

{
  private:
    typedef boost::iterators::detail::iterator_facade_types<Value, CategoryOrTraversal, Reference, Difference>
        associated_types;

    typedef boost::iterators::detail::operator_arrow_dispatch<Reference, typename associated_types::pointer>
        operator_arrow_dispatch_;

  public:
    typedef typename associated_types::value_type value_type;
    typedef Reference reference;
    typedef Difference difference_type;

    typedef typename operator_arrow_dispatch_::result_type pointer;

    typedef typename associated_types::iterator_category iterator_category;

  public:
    reference operator*() const
    {
            return iterator_core_access::dereference(this->derived());
    }

    pointer operator->() const
    {
            return operator_arrow_dispatch_::apply(*this->derived());
    }

    Derived &operator++()
    {
            iterator_core_access::increment(this->derived());
            return this->derived();
    }

  protected:
    Derived &derived()
    {
            return *static_cast<Derived *>(this);
    }

    Derived const &derived() const
    {
            return *static_cast<Derived const *>(this);
    }
};

template <class Derived, class Value, class CategoryOrTraversal, class Reference, class Difference>
class iterator_facade_base<Derived, Value, CategoryOrTraversal, Reference, Difference, true, false>
    : public iterator_facade_base<Derived, Value, CategoryOrTraversal, Reference, Difference, false, false> {
  public:
    Derived &operator--()
    {
            iterator_core_access::decrement(this->derived());
            return this->derived();
    }

    Derived operator--(int)
    {
            Derived tmp(this->derived());
            --*this;
            return tmp;
    }
};

template <class Derived, class Value, class CategoryOrTraversal, class Reference, class Difference>
class iterator_facade_base<Derived, Value, CategoryOrTraversal, Reference, Difference, true, true>
    : public iterator_facade_base<Derived, Value, CategoryOrTraversal, Reference, Difference, true, false> {
  private:
    typedef iterator_facade_base<Derived, Value, CategoryOrTraversal, Reference, Difference, true, false> base_type;

  public:
    typedef typename base_type::reference reference;
    typedef typename base_type::difference_type difference_type;

  public:
    typename boost::iterators::detail::operator_brackets_result<Derived, Value, reference>::type operator[](
        difference_type n) const
    {
            typedef boost::iterators::detail::use_operator_brackets_proxy<Value, Reference> use_proxy;

            return boost::iterators::detail::make_operator_brackets_result<Derived>(this->derived() + n, use_proxy());
    }

    Derived &operator+=(difference_type n)
    {
            iterator_core_access::advance(this->derived(), n);
            return this->derived();
    }

    Derived &operator-=(difference_type n)
    {
            iterator_core_access::advance(this->derived(), -n);
            return this->derived();
    }

    Derived operator-(difference_type x) const
    {
            Derived result(this->derived());
            return result -= x;
    }
};

} // namespace detail

template <class Derived, class Value, class CategoryOrTraversal, class Reference = Value &,
          class Difference = std::ptrdiff_t>
class iterator_facade : public detail::iterator_facade_base<
                            Derived, Value, CategoryOrTraversal, Reference, Difference,
                            detail::is_traversal_at_least<CategoryOrTraversal, bidirectional_traversal_tag>::value,
                            detail::is_traversal_at_least<CategoryOrTraversal, random_access_traversal_tag>::value> {
  protected:
    typedef iterator_facade<Derived, Value, CategoryOrTraversal, Reference, Difference> iterator_facade_;
};

template <class I, class V, class TC, class R, class D>
inline typename boost::iterators::detail::postfix_increment_result<I, V, R, TC>::type operator++(
    iterator_facade<I, V, TC, R, D> &i, int)
{
    typename boost::iterators::detail::postfix_increment_result<I, V, R, TC>::type tmp(*static_cast<I *>(&i));

    ++i;

    return tmp;
}

template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2, class TC2,
          class Reference2, class Difference2>
inline typename boost::iterators::detail::enable_if_interoperable<
    Derived1, Derived2, typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
operator==(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
           iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)
{
    static_assert((is_interoperable<Derived1, Derived2>::value), "( is_interoperable< Derived1, Derived2 >::value )");
    return iterator_core_access::equal(*static_cast<Derived1 const *>(&lhs), *static_cast<Derived2 const *>(&rhs),
                                       is_convertible<Derived2, Derived1>());
}
template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2, class TC2,
          class Reference2, class Difference2>
inline typename boost::iterators::detail::enable_if_interoperable<
    Derived1, Derived2, typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
operator!=(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
           iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)
{
    static_assert((is_interoperable<Derived1, Derived2>::value), "( is_interoperable< Derived1, Derived2 >::value )");
    return !iterator_core_access::equal(*static_cast<Derived1 const *>(&lhs), *static_cast<Derived2 const *>(&rhs),
                                        is_convertible<Derived2, Derived1>());
}

template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2, class TC2,
          class Reference2, class Difference2>
inline typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
    Derived1, Derived2, typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
operator<(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
          iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)
{
    static_assert((is_interoperable<Derived1, Derived2>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived1>::type,
                                                                      random_access_traversal_tag>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived2>::type,
                                                                      random_access_traversal_tag>::value),
                  "( is_interoperable< Derived1, Derived2 >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived1 "
                  ">::type, random_access_traversal_tag >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived2 "
                  ">::type, random_access_traversal_tag >::value )");
    return 0 > iterator_core_access::distance_from(*static_cast<Derived1 const *>(&lhs),
                                                   *static_cast<Derived2 const *>(&rhs),
                                                   is_convertible<Derived2, Derived1>());
}
template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2, class TC2,
          class Reference2, class Difference2>
inline typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
    Derived1, Derived2, typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
operator>(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
          iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)
{
    static_assert((is_interoperable<Derived1, Derived2>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived1>::type,
                                                                      random_access_traversal_tag>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived2>::type,
                                                                      random_access_traversal_tag>::value),
                  "( is_interoperable< Derived1, Derived2 >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived1 "
                  ">::type, random_access_traversal_tag >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived2 "
                  ">::type, random_access_traversal_tag >::value )");
    return 0 < iterator_core_access::distance_from(*static_cast<Derived1 const *>(&lhs),
                                                   *static_cast<Derived2 const *>(&rhs),
                                                   is_convertible<Derived2, Derived1>());
}
template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2, class TC2,
          class Reference2, class Difference2>
inline typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
    Derived1, Derived2, typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
operator<=(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
           iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)
{
    static_assert((is_interoperable<Derived1, Derived2>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived1>::type,
                                                                      random_access_traversal_tag>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived2>::type,
                                                                      random_access_traversal_tag>::value),
                  "( is_interoperable< Derived1, Derived2 >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived1 "
                  ">::type, random_access_traversal_tag >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived2 "
                  ">::type, random_access_traversal_tag >::value )");
    return 0 >= iterator_core_access::distance_from(*static_cast<Derived1 const *>(&lhs),
                                                    *static_cast<Derived2 const *>(&rhs),
                                                    is_convertible<Derived2, Derived1>());
}
template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2, class TC2,
          class Reference2, class Difference2>
inline typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
    Derived1, Derived2, typename mpl::apply2<boost::iterators::detail::always_bool2, Derived1, Derived2>::type>::type
operator>=(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
           iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)
{
    static_assert((is_interoperable<Derived1, Derived2>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived1>::type,
                                                                      random_access_traversal_tag>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived2>::type,
                                                                      random_access_traversal_tag>::value),
                  "( is_interoperable< Derived1, Derived2 >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived1 "
                  ">::type, random_access_traversal_tag >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived2 "
                  ">::type, random_access_traversal_tag >::value )");
    return 0 <= iterator_core_access::distance_from(*static_cast<Derived1 const *>(&lhs),
                                                    *static_cast<Derived2 const *>(&rhs),
                                                    is_convertible<Derived2, Derived1>());
}

template <class Derived1, class V1, class TC1, class Reference1, class Difference1, class Derived2, class V2, class TC2,
          class Reference2, class Difference2>
inline typename boost::iterators::detail::enable_if_interoperable_and_random_access_traversal<
    Derived1, Derived2,
    typename mpl::apply2<boost::iterators::detail::choose_difference_type, Derived1, Derived2>::type>::type
operator-(iterator_facade<Derived1, V1, TC1, Reference1, Difference1> const &lhs,
          iterator_facade<Derived2, V2, TC2, Reference2, Difference2> const &rhs)
{
    static_assert((is_interoperable<Derived1, Derived2>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived1>::type,
                                                                      random_access_traversal_tag>::value
                   && boost::iterators::detail::is_traversal_at_least<typename iterator_category<Derived2>::type,
                                                                      random_access_traversal_tag>::value),
                  "( is_interoperable< Derived1, Derived2 >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived1 "
                  ">::type, random_access_traversal_tag >::value && "
                  "boost::iterators::detail::is_traversal_at_least< typename iterator_category< Derived2 "
                  ">::type, random_access_traversal_tag >::value )");
    return iterator_core_access::distance_from(*static_cast<Derived1 const *>(&lhs),
                                               *static_cast<Derived2 const *>(&rhs),
                                               is_convertible<Derived2, Derived1>());
}

template <class Derived, class V, class TC, class R, class D>
inline typename boost::iterators::enable_if<
    boost::iterators::detail::is_traversal_at_least<TC, boost::iterators::random_access_traversal_tag>, Derived>::type
operator+(iterator_facade<Derived, V, TC, R, D> const &i, typename Derived::difference_type n)
{
    Derived tmp(static_cast<Derived const &>(i));
    return tmp += n;
}

template <class Derived, class V, class TC, class R, class D>
inline typename boost::iterators::enable_if<
    boost::iterators::detail::is_traversal_at_least<TC, boost::iterators::random_access_traversal_tag>, Derived>::type
operator+(typename Derived::difference_type n, iterator_facade<Derived, V, TC, R, D> const &i)
{
    Derived tmp(static_cast<Derived const &>(i));
    return tmp += n;
}

} // namespace iterators

using iterators::iterator_core_access;
using iterators::iterator_facade;

} // namespace boost

template <typename T_Value, typename T_IteratorsTpl>
class chain_iterator
    : public boost::iterator_facade<chain_iterator<T_Value, T_IteratorsTpl>, T_Value, boost::forward_traversal_tag> {
    using chain_iterator_type = chain_iterator<T_Value, T_IteratorsTpl>;

  public:
    chain_iterator(const T_IteratorsTpl &beginTpl)
        : m_iteratorsTpl(beginTpl),
          m_section(0),
          m_maxSection(boost::hana::minus(boost::hana::size(beginTpl), boost::hana::size(boost::hana::make_tuple(0))))
    {
    }

    chain_iterator_type &begin()
    {
            return *this;
    }

    chain_iterator_type end()
    {
            auto tmp = *this;
            tmp.m_section = tmp.m_maxSection;

            size_t sectionI = 0;
            boost::hana::for_each(tmp.m_iteratorsTpl, [&tmp, &sectionI](auto &iterators) {
                auto &begin = boost::hana::at_c<0>(iterators);
                auto &end = boost::hana::at_c<1>(iterators);

                if (sectionI != tmp.m_section) {
                    sectionI++;
                    return;
                }

                begin = end;
            });

            return tmp;
    }

  private:
    void increment()
    {
            size_t sectionI = 0;
            bool incrementSection = false;
            boost::hana::for_each(m_iteratorsTpl, [this, &sectionI, &incrementSection](auto &iterators) {
                auto &begin = boost::hana::at_c<0>(iterators);
                auto &end = boost::hana::at_c<1>(iterators);

                if (sectionI != m_section) {
                    sectionI++;
                    return;
                }

                begin++;
                sectionI++;

                if (m_section == m_maxSection && begin == end) {
                    return;
                }

                if (begin == end) {
                    incrementSection = true;
                }
            });

            if (incrementSection) {
                m_section++;
            }
    }

    bool equal(const chain_iterator_type &other) const
    {
            bool isEqual = false;

            callOnCurrentIterator([&other, &isEqual](auto &sectionI, auto &begin, auto &end) {
                if (other.m_section == sectionI) {
                    auto &otherBegin = boost::hana::at_c<0>(boost::hana::at(other.m_iteratorsTpl, sectionI));
                    auto &otherEnd = boost::hana::at_c<0>(boost::hana::at(other.m_iteratorsTpl, sectionI));

                    isEqual = begin == otherBegin && end == otherEnd;
                }
            });

            return isEqual;
    }

    typename std::decay<T_Value>::type &dereference() const
    {
            const typename std::decay<T_Value>::type *value;
            callOnCurrentIterator([&value](auto, auto &begin, auto &) {
                value = &(*begin);
            });

            return const_cast<typename std::decay<T_Value>::type &>(*value);
    }

    template <class T_Callable>
    void callOnCurrentIterator(const T_Callable &callable) const
    {
            boost::hana::fold_left(m_iteratorsTpl, m_iteratorsTpl, [this, &callable](auto state, auto &iterators) {
                auto sectionI = boost::hana::minus(boost::hana::size(m_iteratorsTpl), boost::hana::size(state));

                auto isCurrentSection = m_section == sectionI;

                if (isCurrentSection) {
                    callable(sectionI, boost::hana::at_c<0>(iterators), boost::hana::at_c<1>(iterators));
                }
                return boost::hana::drop_front(state);
            });
    }

  private:
    T_IteratorsTpl m_iteratorsTpl;
    std::size_t m_section;
    std::size_t m_maxSection;

    friend class boost::iterator_core_access;
};

template <typename... T_Ranges>
struct FirstRange {
    using type = decltype(*(boost::hana::front(boost::hana::tuple<T_Ranges...>{}).begin()));
};

template <typename... T_Ranges>
auto make_chain_iterator(const T_Ranges &...ranges) -> auto
{
    auto iteratorsTpl = boost::hana::transform(boost::hana::make_tuple(std::ref(ranges)...), [](const auto &range) {
        return boost::hana::make_tuple(range.get().begin(), range.get().end());
    });

    return chain_iterator<typename FirstRange<T_Ranges...>::type, decltype(iteratorsTpl)>{iteratorsTpl};
}

#include <array>
#include <iostream>
#include <list>
#include <vector>

int main()
{
    std::vector<int> a{1, 2};
    std::vector<int> b{3, 4};
    std::array<int, 2> c{5, 6};
    std::list<int> d{7, 8};

    for (auto const &element : make_chain_iterator(a, b, c, d)) {
            std::cout << element << std::endl;
    }

    return 0;
}
