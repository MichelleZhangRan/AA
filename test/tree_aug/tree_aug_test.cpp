/**
 * @file bounded_degree_mst.cpp
 * @brief 
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */

#define BOOST_TEST_MODULE bounded_degree_mst_test

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "utils/logger.hpp"
#include "paal/iterative_rounding/bounded_degree_min_spanning_tree/bounded_degree_mst.hpp"

using namespace  paal;
using namespace  paal::ir;
using namespace  boost;

struct LogVisitor : public TrivialVisitor {

    template <typename LP>
    void roundCol(LP & lp, int col, double val) {
        LOG("Column "<< col << " rounded to " << val);
    }
    
    template <typename LP>
    void relaxRow(LP & lp, int row) {
        LOG("Relax row " << row);
    }
};

typedef adjacency_list < vecS, vecS, undirectedS,
                            property < vertex_degree_t, int,
                                property < vertex_index_t, int >
                                     >,
                            property < edge_weight_t, double > > Graph;
typedef adjacency_list_traits < vecS, vecS, undirectedS > Traits;
typedef graph_traits < Graph >::edge_descriptor Edge;

template <typename Graph, typename TreeMap, typename Cost>
Edge addEdge(Graph & g,  int u, int v, 
	     TreeMap tree, bool inT, 
	     Cost & cost, double c) {
    bool b;
    Traits::edge_descriptor e;
    std::tie(e, b) = add_edge(u, v, g);
    assert(b);
    tree[e]=inT;
    cost[e] = c;
    return e;
}

typedef property_map < Graph, vertex_index_t >::type Index;
typedef property_map < Graph, edge_weight_t >::type Cost;
typedef property_map < Graph, edge_color_t >::type TreeMap;

BOOST_AUTO_TEST_CASE(tree_aug) {
    Graph g;
    Cost cost = get(edge_weight, g);
    TreeMap treeMap      = get(edge_color, g);

    


    //Example G2.lgf=tree_aug_example_input.lgfmod
    addEdge(g, 0, 1, treeMap, true, cost,0 );
    addEdge(g, 1, 2, treeMap, true, cost,0 );
    addEdge(g, 1, 3, treeMap, true, cost,0 );
    addEdge(g, 3, 4, treeMap, true, cost,0 );
    addEdge(g, 3, 5, treeMap, true, cost,0 );
    addEdge(g, 0, 3, treeMap, false, cost,1 );
    addEdge(g, 0, 2, treeMap, false, cost,1 );
    addEdge(g, 2, 4, treeMap, false, cost,1 );
    addEdge(g, 2, 5, treeMap, false, cost,1 );
    addEdge(g, 4, 5, treeMap, false, cost,1 );
    Index indices = get(vertex_index, g);
    

    paal::ir::TreeAug<Graph, TreeMap, Cost> treeaug(g,treeMap,cost);

      
    paal::ir::IterativeRounding<decltype(treeaug)> ir(treeaug);
    LOG(paal::ir::solve(ir));
    BOOST_CHECK(ir.integerSolution());

    
}

