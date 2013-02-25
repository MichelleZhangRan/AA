/**
 * @file local_search_multi_solution/trivial_neighbor.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef __LOCAL_SEARCH_MULTI_SOLUTION__
#define __LOCAL_SEARCH_MULTI_SOLUTION__

#include <utility>
#include <algorithm>
#include <functional>

#include "local_search_multi_solution_concepts.hpp"
#include "trivial_stop_condition_multi_solution.hpp"
#include "paal/helpers/iterator_helpers.hpp"

namespace paal {
namespace local_search {

namespace search_strategies {
    class ChooseFirstBetter;
    class SteepestSlope;
}


/**
 * @class LocalSearchStepMultiSolution
 * @brief General class for local search on the multi solution. Note there is no Update type here because it can be deduced.
 *
 * @tparam Solution
 * @tparam NeighborhoodGetter
 * @tparam ImproveChecker
 * @tparam SolutionUpdater
 * @tparam StopCondition
 * @tparam SearchStrategy
           Search strategy descibes LS search strategy. For ow we are planning two strategies: 
           <ul>
           <li>ChooseFirstBetter -> The algorithm chooses the first update with the positive gain
           <li>SteepestSlope     -> The algorithm chooses the update with the largest gain and update if positive.
           </ul>
 */

template <typename Solution, 
          typename MultiSearchComponents,
          typename SearchStrategy = search_strategies::ChooseFirstBetter>
class LocalSearchStepMultiSolution {
      BOOST_CONCEPT_ASSERT((local_search_concepts::MultiSearchComponents<MultiSearchComponents, Solution>));
      
      static_assert(std::is_same<SearchStrategy, search_strategies::ChooseFirstBetter>::value || 
                    std::is_same<SearchStrategy, search_strategies::SteepestSlope>::value, "Wrong search strategy");
    
    typedef typename helpers::SolToElem<Solution>::type SolutionElement;
    typedef typename MultiUpdate<MultiSearchComponents, Solution>::type Update;
    
public:

    /**
     * @brief costam 
     *
     * @param solution
     * @param ng
     * @param check
     * @param solutionUpdater
     * @param sc
     */
    LocalSearchStepMultiSolution(Solution solution, MultiSearchComponents sc) : 
        m_solution(std::move(solution)), m_searchComponents(std::move(sc)) {}

    /**
     * @brief tata 
     *
     * @return 
     */
    bool search() {

        for(const SolutionElement & r : m_solution) {
            auto adjustmentSet = m_searchComponents.getNeighborhoodGetter().get(m_solution, r);
            for(const Update & update : helpers::make_range(adjustmentSet)) {
                if(m_searchComponents.getImproveChecker().gain(m_solution, r, update) > 0) {
                    m_searchComponents.getSolutionUpdater().update(m_solution, r, update);
                    return true;
                } else if(m_searchComponents.getStopCondition().stop(m_solution, r, update)) {
                    return false;
                }
            }
        }
          
        return false;
    }

    MultiSearchComponents & getSearchComponents() {
        return m_searchComponents;
    }

    // TODO it is not optional :)
    typename std::enable_if<helpers::has_get<Solution>::value, decltype(std::declval<Solution>().get()) &>::type getSolution() {
        return m_solution.get();
    }
    
    
    //TODO doesn't work
/*    std::enable_if<!has_get<Solution>::value, Solution &> getSolution() {
        return m_solution;
    }*/

private:

    Solution m_solution;
    MultiSearchComponents m_searchComponents;
};

template <typename Solution, typename SearchComponents> 
class LocalSearchStepMultiSolution<Solution, SearchComponents, search_strategies::SteepestSlope> {
    //TODO implement
};

} // local_search
} // paal

#endif // __LOCAL_SEARCH_MULTI_SOLUTION__
