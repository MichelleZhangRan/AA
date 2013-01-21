
#ifndef __2_LOCAL_SEARCH__
#define __2_LOCAL_SEARCH__

#include "local_search/local_search.hpp"
#include "local_search/trivial_neighbour.hpp"
#include "local_search/2_local_search/2_local_search_updater.hpp"
#include "local_search/2_local_search/2_local_search_checker.hpp"
#include "local_search/2_local_search/2_local_search_solution_container.hpp"
#include "data_structures/simple_cycle_manager.hpp"

namespace paal {
namespace local_search {
namespace two_local_search {


template <typename VertexType, 
          typename Metric, 
          typename NeighbourGetter = TrivialNeigbourGetter,
          template <class> CheckIfImprove = CheckIfImprove2Opt,
          template <class> Cycle = data_structures::SimpleCycle>

         class  TwoLocalSearchStep : 
             public LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle>, 
                        NeighbourGetter, CheckIfImprove, TwoLocalSearchUpdater >  {

                typedef LocalSearchStepMultiSolution<TwoLocalSearchContainer<Cycle> , NeighbourGetter, 
                    CheckIfImprove, TwoLocalSearchUpdater > LocalSearchStepT;

                public:

                    //TODO CHECK INAPPROPRIATE ORDER  could be invalid
                    template <typename SolutionIter>  
                     TwoLocalSearchStep(SolutionIter solBegin, SolutionIter solEnd, Metric & m
                             , NeighbourGetter ng = NeighbourGetter()) 
                        :      LocalSearchStepT(m_cycleAdapter, ng, 
                                 CheckIfImprove<Metric>(m), TwoLocalSearchUpdater()),
                                m_cycleManager(solBegin, solEnd),
                                m_cycleAdapter(m_cycleManager) {}

                    Cycle & getCycle() {
                        return m_cycleManager;
                    }

                private:
                    Cycle<VertexType>  m_cycleManager;
                    TwoLocalSearchContainer<Cycle> m_cycleAdapter;
             };

} //two_local_search
} //local_search
} //paal

#endif // __2_LOCAL_SEARCH__

