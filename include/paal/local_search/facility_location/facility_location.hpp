/**
 * @file facility_location.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

#include "paal/data_structures/facility_location/facility_location_solution.hpp"
#include "paal/local_search/multi_solution/local_search_multi_solution.hpp"

#include "facility_location_solution_adapter.hpp"
#include "facility_location_add.hpp"
#include "facility_location_remove.hpp"
#include "facility_location_swap.hpp"

namespace paal {
namespace local_search {
namespace facility_location {

/**
 * @class DefaultFLComponents 
 * @brief Model of MultiSearchComponents with default multi search components for facility location.
 *
 * @tparam VertexType
 */
template <typename VertexType> 
struct DefaultRemoveFLComponents {
    typedef MultiSearchComponents<
                FacilityLocationGetMovesRemove<VertexType>,
                FacilityLocationCheckerRemove        <VertexType>,
                FacilityLocationCommitRemove        <VertexType>> type;
};

template <typename VertexType> 
struct DefaultAddFLComponents {
    typedef MultiSearchComponents<
                FacilityLocationGetMovesAdd<VertexType>,
                FacilityLocationCheckerAdd        <VertexType>,
                FacilityLocationCommitAdd        <VertexType>> type;
};

template <typename VertexType> 
struct DefaultSwapFLComponents {
    typedef MultiSearchComponents<
                FacilityLocationGetMovesSwap<VertexType>,
                FacilityLocationCheckerSwap        <VertexType>,
                FacilityLocationCommitSwap        <VertexType>> type;
};

/**
 * @class FacilityLocationLocalSearchStep
 * @brief this is model of LocalSearchStepMultiSolution concept. See \ref local_search.<br>
 * The Move is facility_location::Move. <br>
 * The Solution is adapted data_structures::FacilityLocationSolution. <br>
 * The SolutionElement is facility_location::Facility  <br>
 * Use DefaultFLComponents for default search components.
 *
 * The FacilityLocationLocalSearchStep takes as constructor parameter  data_structures::FacilityLocationSolution.
 * <b> WARNING </b>
 * getSolution of the FacilityLocationLocalSearchStep returns type ObjectWithCopy<FacilityLocationSolution>.
 * If you want to perform search, then change the solution object and continue local search you should perform all the operations on ObjectWithCopy. <br>
 * example: 
    \snippet facility_location_example.cpp FL Search Example
 *
 * full example is facility_location_example.cpp
 *
 * @tparam Voronoi
 * @tparam FacilityCost
 * @tparam MultiSearchComponents
 */
template <typename SearchStrategy = search_strategies::ChooseFirstBetter,
          typename PostSearchAction,
          typename GlobalStopCondition,
          typename FacilityLocationSolution,
          typename... Components>
bool facility_location_local_search(
            FacilityLocationSolution & fls,
            PostSearchAction psa,
            GlobalStopCondition gsc,
            Components... components) {
    typedef FacilityLocationSolutionAdapter<FacilityLocationSolution> FLSA;
    FLSA flsa(fls);
    return local_search_multi_solution(flsa, std::move(psa), std::move(gsc), std::move(components)...);
}

template <typename SearchStrategy = search_strategies::ChooseFirstBetter, 
          typename FacilityLocationSolution, 
          typename... Components>
bool facility_location_local_search_simple(FacilityLocationSolution & fls, Components... components) {
    return facility_location_local_search(fls, utils::SkipFunctor(), utils::ReturnFalseFunctor(), std::move(components)...);
}


}
}
}
