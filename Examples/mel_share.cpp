#include <iostream>
#include "util.h"
#include "MelShare.h"

// https://docs.microsoft.com/en-us/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp

int main(int argc, char * argv[]) {

    // create shared memory maps (optionally define size in bytes, default = 256 bytes)
    // note that on Windows, the shared memory maps will only stay active for as long as
    // these instances stay in scope, so create them inside of your main loop or inside of a
    // class isntance that you expect to stay alive for the duration of the program (e.g. OpenWrist or ME-II).
    mel::share::MelShare map0("map0");
    mel::share::MelShare map1("map1");
    mel::share::MelShare map2("map2", 80); // 10 doubles * 8 bytes/double

    // create new data containers to write to map (vectos and C-style arrays can be used)
    mel::char_vec  my_chars = { 'a','b','c' };
    mel::int32_vec my_ints = { 1,1,2,3,5 };
    double my_doubles[10] = { 0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9 };

    std::cout << "Press ENTER to write values.";
    getchar();

    // write the data to the maps
    map0.write(my_chars); // non-static version called using dot operator on a MelShare instance (fastest method)
    mel::share::write_map("map1",my_ints); // alternately, static version that can be called anywhere if you know the map name (slightly slower)
    map2.write(my_doubles, 10); // C-style arrays can be used with both non-static and static versions, but you must input size manually

    std::cout << "Wrote: " << std::endl;
    std::cout << "map0:    ";  mel::print_vector(my_chars);
    std::cout << "map1:    ";  mel::print_vector(my_ints);
    std::cout << "map2:    ";  mel::print_array(my_doubles, 10);

    std::cout << "Run Python or C# code, then press ENTER to receive new values.";
    getchar();

    // Data in a map can change type and size, so long as the data stored does not exceed the initial bytes
    // size. The only caveat is that corresponding writer-reader arguement types must match, e.g. if you
    // last wrote a double type vector/array to a map, you must read the map back into a double type vector/array
    // for any meaningful interpretation.

    my_ints.resize(7); // here we expect the size of my_ints to increase by two (see Python or C#)

    // read the altered data from the maps
    map0.read(my_chars);
    map1.read(my_doubles, 10); // this demonstrates that we can read doubles from a map that previously wrote ints
    mel::share::read_map("map2", my_ints); // and vice-versa

    std::cout << "Read: " << std::endl;
    std::cout << "map0:    ";  mel::print_vector(my_chars);
    std::cout << "map1:    ";  mel::print_array(my_doubles, 10);
    std::cout << "map2:    ";  mel::print_vector(my_ints);

    std::cout << "Press ENTER to exit and free the shared memory.";
    getchar();

    return 0;
}


