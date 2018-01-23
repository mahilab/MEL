#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

// The Options class in MEL is taken directly from the open-source cxxopts.
// Consult its documentation/wiki for more info: (github.com/jarro2783/cxxopts)

int main(int argc, char *argv[]) {

    // create Options object
    Options options("options.exe", "Simple Program Demonstrating Options");

    // add options
    // options can be short, long, or both (short must come first)
    // for example:   options.exe -e   OR   options.exe --evan
    options.add_options()
        ("e,evan", "Print's a message from Evan.")  
        ("h,help", "Prints helpful information.")
        ("i,integer", "Gets an integer from user.", value<int>());

    // parse options
    auto result = options.parse(argc, argv);

    // do things with result:

    if (result.count("evan") > 0)
        print("Hello, my name is Evan!");

    if (result.count("help") > 0)
        print(options.help());

    if (result.count("integer") > 0) {
        int i = result["integer"].as<int>();
        print("You entered " + stringify(i));
    }

    return 0;
}
