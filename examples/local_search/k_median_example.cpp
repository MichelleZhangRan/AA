/**
 * @file k_median_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include "test/utils/sample_graph.hpp"

#include "paal/local_search/k_median/k_median.hpp"
#include "paal/data_structures/facility_location/fl_algo.hpp"

#include <iostream>

int main() {
    //! [K Median Search Example]
    // sample data
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_small();

    // define voronoi and solution
    const int k = 2;
    typedef paal::data_structures::voronoi<decltype(gm)> VorType;
    typedef paal::data_structures::k_median_solution<VorType> Sol;
    typedef paal::data_structures::voronoi_traits<VorType> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    typedef typename Sol::UnchosenFacilitiesSet USet;

    // create voronoi and solution
    VorType voronoi(GSet{ SGM::B, SGM::D },
                    VSet{ SGM::A, SGM::B, SGM::C, SGM::D, SGM::E }, gm);
    Sol sol(std::move(voronoi), USet{ SGM::A, SGM::C }, k);

    // create facility location local search step
    paal::local_search::k_median::default_k_median_components::type swap;

    // search
    paal::local_search::facility_location::facility_location_first_improving(
        sol, swap);

    // print result
    auto const &ch = sol.get_chosen_facilities();
    std::cout << "Solution:" << std::endl;
    std::copy(ch.begin(), ch.end(), std::ostream_iterator<int>(std::cout, ","));
    std::cout << std::endl << "Cost " << paal::simple_algo::get_km_cost(gm, sol)
              << std::endl;

    std::cout << std::endl;
    //! [K Median Search Example]

    return 0;
}
