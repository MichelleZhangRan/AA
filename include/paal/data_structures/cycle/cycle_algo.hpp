/**
 * @file cycle_algo.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#ifndef CYCLE_ALGO
#define CYCLE_ALGO

#include "paal/data_structures/cycle/cycle_traits.hpp"
#include "paal/data_structures/vertex_to_edge_iterator.hpp"

#include <algorithm>
#include <numeric>
#include <string>

namespace paal {
namespace simple_algo {

/**
  * @brief computes length of the cycle
  *
  * @tparam Metric
  * @tparam Cycle
  * @param m
  * @param cm
  *
  * @return
  */
template <typename Metric, typename Cycle>
typename Metric::DistanceType get_length(const Metric &m, const Cycle &cm) {
    typedef typename data_structures::cycle_traits<Cycle>::CycleElem El;
    typedef typename Metric::DistanceType Dist;

    auto ebegin =
        data_structures::make_vertex_to_edge_iterator(cm.vbegin(), cm.vend());
    auto eend =
        data_structures::make_vertex_to_edge_iterator(cm.vend(), cm.vend());
    return std::accumulate(ebegin, eend, Dist(),
                           [&m](Dist a, const std::pair<El, El> & p)->Dist{
        return a + m(p.first, p.second);
    });
}

/// pints cycle to std out
template <typename Cycle, typename Stream>
void print(const Cycle &cm, Stream &o, const std::string &endl = "\n") {
    auto ebegin =
        data_structures::make_vertex_to_edge_iterator(cm.vbegin(), cm.vend());
    auto eend =
        data_structures::make_vertex_to_edge_iterator(cm.vend(), cm.vend());
    typedef typename data_structures::cycle_traits<Cycle>::CycleElem El;

    for (const std::pair<El, El> &p :
         boost::make_iterator_range(ebegin, eend)) {
        o << "(" << p.first << "," << p.second << ")->";
    }

    o << endl;
}

} // paal
} // simple_algo

#endif // CYCLE_ALGO
