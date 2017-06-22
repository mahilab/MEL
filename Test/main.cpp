#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include <chrono>


std::vector<int> sort_channels_and_create_indexer(std::vector<unsigned int>& channels) {
    // sort the channels
    std::sort(channels.begin(), channels.end());
    // create indexer
    std::vector<int> indexer(channels.back() + 1, -1);
    // add channel indices to indexer
    int indice = 0;
    for (auto it = channels.begin(); it != channels.end(); ++it) {
        indexer[*it] = indice;
        indice += 1;
    }
    return indexer;
}



int main(int argc, char * argv[]) {


    std::vector<unsigned int> channels = { 0, 5, 4 ,1, 8, 10, 3 };

    std::vector<int> indexer = sort_channels_and_create_indexer(channels);

    
    for (auto i = channels.begin(); i != channels.end(); ++i)
        std::cout << *i << " ";
    std::cout << std::endl;

    for (auto i = indexer.begin(); i != indexer.end(); ++i)
        std::cout << *i << " ";
    std::cout << std::endl;

    for (auto i = channels.begin(); i != channels.end(); ++i)
        std::cout << indexer[*i] << " ";
    std::cout << std::endl;
    
    // test code
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::vector<int> indicies;
    for (auto i = channels.begin(); i != channels.end(); ++i) {
        int index = std::find(channels.begin(), channels.end(), *i) - channels.begin();
        std::cout << index << std::endl;
    }    

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Nanoseconds = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << std::endl;

    for (auto it = indicies.begin(); it != indicies.end(); ++it)
        std::cout << *it << " ";
}
