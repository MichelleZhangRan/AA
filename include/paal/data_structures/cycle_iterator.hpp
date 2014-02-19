/**
 * @file cycle_iterator.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-03-11
 */
#ifndef CYCLE_ITERATOR_HPP
#define CYCLE_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>
#include "paal/utils/type_functions.hpp"

namespace paal {
namespace data_structures {

// could have been done by simple boost::join
// minor TODO class can specialized for randomaccess iterators

/**
 * @class CycleIterator
 * @brief For given collection (begin -> end) and start iterator pointing to an element inside
 *        collection (begin -> ... -> start -> ... ->end), returns new collection created by shifting the old collection to start.
 *
 *        example:
 *        WE are given collection of 5 elemeents and start points to the third one:
 *        1 -> 2 -> 3 (start) -> 4 -> 5 -> end
 *
 *        The collection
 *        ( CycleIterator(start, begin, end), CycleIterator() )
 *        describes collection
 *        3 -> 4 -> 5 -> 1 -> 2 -> end
 *
 * @tparam Iter type of iterator
 */
template <typename Iter>
class CycleIterator :
    public boost::iterator_facade<
        CycleIterator<Iter>,
        typename std::iterator_traits<Iter>::value_type,
        typename boost::forward_traversal_tag,
        typename std::iterator_traits<Iter>::reference,
        typename std::iterator_traits<Iter>::difference_type
        > {

    typedef std::iterator_traits<Iter> IT;
    typedef typename IT::reference ref;

public:

    /**
     * @brief constructing of CycleIterator
     *
     * @param start new start
     * @param begin old start
     * @param end   old end
     */
    CycleIterator(Iter start, Iter begin, Iter end) :
        m_curr(start), m_start(start), m_begin(begin), m_end(end), m_isEnd(false) {}

    /**
     * @brief Points to end of the collection
     */
    CycleIterator() : m_isEnd(true) {}

private:
    friend class boost::iterator_core_access;
    /**
     *  Standard iterator facade implementation:
     */

    void increment() {
        ++m_curr;

        if(m_curr == m_end) {
            m_curr = m_begin;
        }

        if(m_curr == m_start) {
            m_isEnd = true;
            m_curr = m_end;
        }
    }

    bool equal(CycleIterator ei) const {
        return (m_isEnd && ei.m_isEnd) || m_curr == ei.m_curr;
    }

    ref dereference() const {
        return *m_curr;
    }

    Iter m_curr;
    Iter m_start;
    Iter m_begin;
    Iter m_end;
    bool m_isEnd = false;
};

}
}
#endif /* CYCLE_ITERATOR_HPP */
