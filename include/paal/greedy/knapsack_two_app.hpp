/**
 * @file knapsack_two_app.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-10-07
 */
#ifndef KNAPSACK_TWO_APP_HPP
#define KNAPSACK_TWO_APP_HPP

#include "paal/utils/functors.hpp"
#include "paal/utils/type_functions.hpp"
#include "paal/greedy/knapsack/knapsack_general.hpp"

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <type_traits>
#include <utility>

namespace paal {

namespace detail {
template <typename ObjectsIterIter, typename ObjectSizeFunctor,
          typename ObjectValueFunctor>
std::tuple<FunctorOnIteratorPValue<
               ObjectValueFunctor,
               typename std::iterator_traits<ObjectsIterIter>::value_type>,
           FunctorOnIteratorPValue<
               ObjectValueFunctor,
               typename std::iterator_traits<ObjectsIterIter>::value_type>,
           std::pair<ObjectsIterIter, unsigned>>
get_greedy_fill(
    ObjectsIterIter oBegin, ObjectsIterIter oEnd,
    FunctorOnIteratorPValue<
        ObjectSizeFunctor,
        typename std::iterator_traits<ObjectsIterIter>::value_type> capacity,
    ObjectValueFunctor value, ObjectSizeFunctor size, no_zero_one_tag) {
    typedef typename std::iterator_traits<ObjectsIterIter>::value_type
        ObjectsIter;
    typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> ValueType;
    typedef FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter> SizeType;

    auto starValue = utils::make_lift_iterator_functor(value);
    auto starSize = utils::make_lift_iterator_functor(size);
    auto density = make_Density(starValue, starSize);
    auto compare = utils::make_functor_to_comparator(density);
    auto mostDenseIter = std::max_element(oBegin, oEnd, compare);

    unsigned nr = capacity / size(**mostDenseIter);
    ValueType valueSum = ValueType(nr) * value(**mostDenseIter);
    SizeType sizeSum = SizeType(nr) * size(**mostDenseIter);
    return std::make_tuple(valueSum, sizeSum,
                           std::make_pair(mostDenseIter, nr));
}

template <typename ObjectsIterAndNr, typename OutputIter>
void greedy_to_output(ObjectsIterAndNr mostDenseIterAndNr, OutputIter out,
                      no_zero_one_tag) {
    auto nr = mostDenseIterAndNr.second;
    auto mostDenseIter = mostDenseIterAndNr.first;
    for (unsigned i = 0; i < nr; ++i) {
        *out = **mostDenseIter;
        ++out;
    }
}

} // detail

template <typename OutputIterator, typename ObjectsIter,
          typename ObjectSizeFunctor, typename ObjectValueFunctor>
std::pair<detail::FunctorOnIteratorPValue<ObjectValueFunctor, ObjectsIter>,
          detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter>>
knapsack_two_app(
    ObjectsIter oBegin, ObjectsIter oEnd,
    detail::FunctorOnIteratorPValue<ObjectSizeFunctor, ObjectsIter> capacity,
    OutputIterator out, ObjectValueFunctor value, ObjectSizeFunctor size) {
    return detail::knapsack_general_two_app(oBegin, oEnd, capacity, out, value,
                                            size, detail::no_zero_one_tag());
}
}
#endif /* KNAPSACK_TWO_APP_HPP */
