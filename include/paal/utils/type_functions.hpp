/**
 * @file type_functions.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef TYPE_FUNCTIONS_HPP
#define TYPE_FUNCTIONS_HPP

#include <boost/range.hpp>

#include <type_traits>
#include <utility>
#include <tuple>

namespace paal {

/// for given expression returns its type with removed const and reference
#define puretype(t) typename std::decay<decltype(t)>::type

/// for given range returns type of its reference
template <typename Range>
using range_to_ref_t = typename boost::range_reference<Range>::type;

/// for given range returns type of its element
template <typename Range>
using range_to_elem_t = typename boost::range_value<Range>::type;

namespace detail {

    template <typename T, int k>
    struct k_tuple {
        using type = decltype(
            std::tuple_cat(std::declval<std::tuple<T>>(),
                       std::declval<typename k_tuple<T, k - 1>::type>()));
    };

    template <typename T>
    struct k_tuple<T, 1> {using type = std::tuple<T>;};
}

/// returns tuple consisting of k times type T
template <typename T, int k>
using k_tuple_t = typename detail::k_tuple<T, k>::type;

/// return pure type of function (decays const and reference)
template <class F>
using pure_result_of_t = typename std::decay<typename std::result_of<F>::type>::type;

/// return type after promotion with double
template <typename T>
using promote_with_double_t = puretype(std::declval<T>() + 0.0);

} //!paal

#endif // TYPE_FUNCTIONS_HPP
