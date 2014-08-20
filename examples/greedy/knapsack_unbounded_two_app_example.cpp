/**
 * @file knapsack_two_app_example.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

    //! [Knapsack Example]
#include "paal/greedy/knapsack_unbounded_two_app.hpp"

#include <vector>
#include <iostream>

int main() {
    using Objects = std::vector<std::pair<int, int>>;
    Objects objects{{1, 3}, {2, 2} , {3, 65} ,
        {1, 1} , {2, 2} , {4, 3} , {1, 1} , {10, 23}};
    const int capacity = 6;
    auto size = [](std::pair<int, int> object) {return object.first;};
    auto value = [](std::pair<int, int> object) {return object.second;};

    Objects result;
    std::cout << "Knapsack unbounded" << std::endl;
    auto maxValue = paal::knapsack_unbounded_two_app(
        objects, capacity,
        std::back_inserter(result), value, size);

    std::cout << "Max value " << maxValue.first << ", Total size "
              << maxValue.second << std::endl;
    for(auto o : result) {
        std::cout << "{ size = " << o.first << ", value = " << o.second << "} ";
    }
    std::cout << std::endl;

    return 0;
}
    //! [Knapsack Example]