#pragma once

#include <initializer_list>

#include <array>
#include <cstdint>
#include <type_traits>
#include <vector>
 
namespace clientmap {
//=======================================================
// #include <initializer_list>
//=======================================================
// Accept any number of parameters of any types, and do nothing with them.
// Useful for providing a context for parameter pack expansion where a legal
// expansion context is not otherwise available.
//
// See https://github.com/mapbox/cpp/blob/master/C%2B%2B%20Structural%20Metaprogramming.md
// for more details.
//
template <class... Ts> void ignore(Ts&&...) {}

// std::initializer_list overload, for situations where you need sequenced
// modifications.
//
template <class T> void ignore(const std::initializer_list<T>&) {}

// Handle the zero-argument case.
inline void ignore(const std::initializer_list<int>&) {}
//============================================================
//============================================================
namespace non_copyable_
{
    class NonCopyable
    {
    public:
        NonCopyable(NonCopyable const&) = delete;
        NonCopyable& operator=(NonCopyable const&) = delete;

    protected:
        constexpr NonCopyable() = default;
        ~NonCopyable() = default;
    };
} // namespace non_copyable_
using NonCopyable = non_copyable_::NonCopyable;
//============================================================ 
//#include <array>
//#include <cstdint>
//#include <type_traits>
//#include <vector>
//============================================================
template<typename T>
constexpr auto underlying_type(T t) -> typename std::underlying_type_t<T> {
    // If T is a complete enumeration (enum) type, provides a member 
    // typedef type that names the underlying type of T.
    // std::underlying_type模板用于获取枚举类型T的基础类型。 
    return typename std::underlying_type_t<T>(t);
}

template <typename T> struct is_utf16char_like : std::false_type {};
template <typename T> struct is_utf16char_like<const T> : is_utf16char_like<T> {};
template <> struct is_utf16char_like<char16_t> : std::true_type {};
template <> struct is_utf16char_like<wchar_t> : std::true_type {};
template <> struct is_utf16char_like<uint16_t> : std::true_type {};

template <typename T> using is_utf16char_like_pointer = std::integral_constant<bool, std::is_pointer<T>::value
    && is_utf16char_like<typename std::remove_pointer<T>::type>::value>;

template <class OutPointer, class InChar>
typename std::enable_if<is_utf16char_like<InChar>::value &&
    is_utf16char_like_pointer<OutPointer>::value,
    OutPointer
>::type
utf16char_cast(InChar *in)
{
    return reinterpret_cast<OutPointer>(in);
}

template <typename T>
struct is_linear_container : std::false_type {};

template <typename T, std::size_t N>
struct is_linear_container<std::array<T, N>> : std::true_type {};
template <typename... Ts>
struct is_linear_container<std::vector<Ts...>> : std::true_type {};
//============================================================ 
//#include <functional>
// hash.hpp
//============================================================
template <class T>
void hash_combine(std::size_t& seed, const T& v)
{
    seed ^= std::hash<T>()(v) + 0x9e3779b9
        + (seed << 6) + (seed >> 2);
}

template <class... Args>
std::size_t util_hash(Args&&... args)
{
    std::size_t seed = 0;
    ignore({ (hash_combine(seed, args), 0)... });
    return seed;
}

} // namespace MdU 
 
