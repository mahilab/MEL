#include <iostream>
#include <boost/program_options.hpp>
#include "util.h"
#include "MelShare.h"
#include "Clock.h"
#include "Integrator.h"
#include "UnityGame.h"
#include "Input.h"
#include <cmath>
#include <random>


// This is the MEL Examples program. It is divided in sections by comment headers.
// With the exception of PROGRAM OPTIONS, each section is self contained and 
// does not require any variables from other sections. To run a particular secition,
// use the appropriate program option argument when running the exe from the 
// command line.

int main(int argc, char * argv[]) {

    //-------------------------------------------------------------------------
    // PROGRAM OPTIONS:    >Examples.exe --help
    //-------------------------------------------------------------------------

    // use boost set up your program options 
    // this allows us to pass arguments through the command line like
    boost::program_options::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("melshare", "example demonstrating how to use MELShare for two way communication with C# or Python")
        ("melscope", "another MELShare demo that produces test data for also introducing in MELScope")
        ("unity", "example of how to launch a Unity game from C++")
        ("test","test");

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), var_map);
    boost::program_options::notify(var_map);

    // running "Examples.exe --help" at the command line will produce a nicely
    // formatted list of program options
    if (var_map.count("help")) {
        mel::print(desc);
        return 0;
    }

    //-------------------------------------------------------------------------
    // MELSHARE EXAMPLE:    >Examples.exe --melshare
    //-------------------------------------------------------------------------

    if (var_map.count("melshare")) {

        // create shared memory maps (optionally define size in bytes, default = 256 bytes)
        // note that on Windows, the shared memory maps will only stay active for as long as
        // these instances stay in scope, so create them inside of your main loop or inside of a
        // class isntance that you expect to stay alive for the duration of the program (e.g. OpenWrist or ME-II).
        mel::share::MelShare map0("map0");
        mel::share::MelShare map1("map1");
        mel::share::MelShare map2("map2", 80); // 10 doubles * 8 bytes/double

        // create new data containers to write to map (vectors, STL arrays, and C-style arrays can be used)
        std::vector<char> my_chars = { 'a','b','c' }; // STL vector
        std::array<int, 5> my_ints = { 1,1,2,3,5 };    // STL array
        double my_doubles[10] = { 0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9 }; // C-style array

        std::cout << "Press ENTER to write values.";
        getchar();

        // write the data to the maps
        map0.write(my_chars); // non-static version called using dot operator on a MelShare instance (fastest method)
        mel::share::write_map("map1", my_ints); // alternately, static version that can be called anywhere if you know the map name (slightly slower)
        map2.write(my_doubles, 10); // C-style arrays can be used with both non-static and static versions, but you must input size manually

        mel::print("Wrote: ");
        std::cout << "map0:    ";  mel::print(my_chars);
        std::cout << "map1:    ";  mel::print(my_ints);
        std::cout << "map2:    ";  mel::print(my_doubles, 10, true);

        std::cout << "Run Python or C# code, then press ENTER to receive new values.";
        getchar();

        // Data in a map can change type and size, so long as the data stored does not exceed the initial bytes
        // size. The only caveat is that corresponding writer-reader arguement types must match, e.g. if you
        // last wrote a double type vector/array to a map, you must read the map back into a double type vector/array
        // for any meaningful interpretation.

        // Here we expect the size of my_ints to increase by two (see Python or C#). If you require this sort of behavior, 
        // consider using a single vector instead of the multiple arrays like this example shows.
        std::array<int, 7> my_ints_bigger{ 0,1,0,0,0,0,0 };

        // read the altered data from the maps
        map0.read(my_chars);
        map1.read(my_doubles, 10); // this demonstrates that we can read doubles from a map that previously wrote ints
        mel::share::read_map("map2", my_ints_bigger); // and vice-versa

        mel::print("Read: ");
        std::cout << "map0:    ";  mel::print(my_chars);
        std::cout << "map1:    ";  mel::print(my_doubles, 10, true);
        std::cout << "map2:    ";  mel::print(my_ints_bigger);

        return 0;
    }

    //-------------------------------------------------------------------------
    // MELSCOPE EXAMPLE:    >Examples.exe --melscope
    //-------------------------------------------------------------------------

    if (var_map.count("melscope")) {

        // Create our MELShare maps. We will make one map, "integrals", to demonstate
        // MEL's Integrator class. We will also make three other maps, "cpp2py", 
        // "py2cpp_ampl", and "py2cpp_freq" to deomstate bidirectional communication in 
        // MELShare. While bidirectional communcation over a single MELShare map is possible 
        // with the read_write() functions, it can be tricky to get the timing right. 
        // It's best to just keep two maps open for this purpose.
        mel::share::MelShare integrals("integrals");
        mel::share::MelShare cpp2py("cpp2py");
        mel::share::MelShare py2cpp_ampl("py2cpp_ampl");
        mel::share::MelShare py2cpp_freq("py2cpp_freq");

        // create the data buffers for each map
        std::array<double, 4> integrals_data = { 0, 0, 0, 0 };
        std::array<double, 4> cpp2py_data = { 0, 0, 0, 0 };
        std::array<double, 4> py2cpp_ampl_data = { 10, 8, 6, 4 };
        std::array<double, 4> py2cpp_freq_data = { 0.4, 0.6, 0.8, 1.0 };

        // create a MEL Integrator for doing some integration. note the initial value 5
        mel::Integrator integrator = mel::Integrator(5, mel::Integrator::Technique::Trapezoidal);

        // create a MEL Clock to control the rate of our data generating loop
        mel::Clock clock(1000); // 1000 Hz clock

        // In this example, we will have the data in cpp2py be a funcion of the data
        // from py2cpp. Specifically, we will generate some periodic data, and use
        // scalers from py2cpp to change the amplitudes and frequencies. For that 
        // reason, we will start off by actually writing py2cpp so that it has some 
        // intial values. 
        py2cpp_ampl.write(py2cpp_ampl_data);
        py2cpp_freq.write(py2cpp_freq_data);

        // start the clock
        clock.start();

        // run the data loop for 60 seconds
        while (clock.time() < 60) {

            // generate new data for integrals
            integrals_data[0] = clock.time();                                          //     t
            integrals_data[1] = 0.5 * (cos(clock.time()) + 7 * cos(7 * clock.time())); // dx/dt = 1/2 * [ cos(t) + 7 * cos(7t) ] 
            integrals_data[2] = sin(4 * clock.time()) * cos(3 * clock.time()) + 5;     //     x = sin(4t) * cos(3t) + 5
            integrals_data[3] = integrator.integrate(integrals_data[1], clock.time()); //     x ~ integrate(dx/dt)

            // write integrals
            integrals.write(integrals_data);
            
            // read data from py2cpp_ampl and py2cpp_freq
            py2cpp_ampl.read(py2cpp_ampl_data);
            py2cpp_freq.read(py2cpp_freq_data);

            // generate new data for cpp2py
            cpp2py_data[0] = mel::sin_wave(py2cpp_ampl_data[0], py2cpp_freq_data[0], clock.time());
            cpp2py_data[1] = mel::square_wave(py2cpp_ampl_data[1], py2cpp_freq_data[1], clock.time());;
            cpp2py_data[2] = mel::triangle_wave(py2cpp_ampl_data[2], py2cpp_freq_data[2], clock.time());
            cpp2py_data[3] = mel::sawtooth_wave(py2cpp_ampl_data[3], py2cpp_freq_data[3], clock.time());

            // write cpp2py
            cpp2py.write(cpp2py_data);

            // wait the clock
            clock.wait();
        }
    }

    if (var_map.count("test")) {
        srand(2);
        std::vector<int> numbers = { 1,2,3,4,5,6,7 };
        std::random_shuffle(numbers.begin(), numbers.end());
        mel::print(numbers);
        std::random_shuffle(numbers.begin(), numbers.end());
        mel::print(numbers);
        std::random_shuffle(numbers.begin(), numbers.end());
        mel::print(numbers);
        std::random_shuffle(numbers.begin(), numbers.end());
        mel::print(numbers);
        std::random_shuffle(numbers.begin(), numbers.end());
        mel::print(numbers);
    }

    if (var_map.count("unity")) {
        mel::UnityGame my_game("my_game", "C:\\Users\\epeze\\Dropbox\\OpenWrist\\Software\\Games\\Pendulum\\Build\\Pendulum.exe");
        my_game.launch();
        mel::Clock::wait_for(10);
    }
}


