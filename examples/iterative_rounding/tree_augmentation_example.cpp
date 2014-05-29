/**
 * @file tree_augmentation_example.cpp
 * @brief
 * @author Attila Bernath, Piotr Godlewski
 * @version 1.0
 * @date 2013-10-17
 */

#include "paal/iterative_rounding/treeaug/tree_augmentation.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <iostream>
#include <vector>

int main() {
//! [Tree Augmentation Example]
    using EdgeProp = boost::property<boost::edge_weight_t, double,
                boost::property<boost::edge_color_t, bool>>;
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                boost::no_property, EdgeProp>;
    using Edge = boost::graph_traits<Graph>::edge_descriptor;

    // sample problem
    std::vector<std::pair<int, int>> edges {{0,1},{1,2},{1,3},{3,4},{3,5},{0,3},{0,3},{2,4},{2,5},{4,5}};
    std::vector<EdgeProp> edge_properties {EdgeProp(0,1), EdgeProp(0,1),
        EdgeProp(0,1), EdgeProp(0,1), EdgeProp(0,1), EdgeProp(1,0),
        EdgeProp(1,0), EdgeProp(1,0), EdgeProp(1,0), EdgeProp(1,0)};

    Graph g(edges.begin(), edges.end(), edge_properties.begin(), 6);

    std::vector<Edge> solution;

    // optional input validity checking
    auto tree_aug = paal::ir::make_tree_aug(g, std::back_inserter(solution));
    auto error = tree_aug.check_input_validity();
    if (error) {
        std::cerr << "The input is not valid!" << std::endl;
        std::cerr << *error << std::endl;
        return -1;
    }

    // solve it
    auto result = paal::ir::tree_augmentation_iterative_rounding(
                    g, std::back_inserter(solution));

    if (result.first == paal::lp::OPTIMAL) {
        std::cout << "The solution contains the following nontree edges:" << std::endl;
        for (auto const & e : solution) {
            std::cout << "Edge " << e << std::endl;
        }
        std::cout << "Cost of the solution: " << *(result.second) << std::endl;
    }
    else {
        std::cout << "The instance is infeasible" << std::endl;
    }
    paal::lp::glp::free_env();
//! [Tree Augmentation Example]
    return 0;
}

