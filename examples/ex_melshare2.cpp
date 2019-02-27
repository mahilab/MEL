#include <MEL/Communications/MelShare2.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Utility/Options.hpp>
#include <array>

using namespace mel;



int main(int argc, char* argv[]) {
    Options options("melshare2.exe", "MelShare2");
    options.add_options()("a", "Runs A")("b", "Runs B");
    auto input = options.parse(argc, argv);

    std::vector<double> data;

    if (input.count("a")) {
        MelShare2<double, 10> ms("ms2", OpenOrCreate);
        data = {1,2,3,4,5};
        ms.write(data);
        while (data[0] == 1)
            ms.read(data);
        print(data);
    }
    else if (input.count("b")) {
        MelShare2<double, 10> ms("ms2", OpenOnly);
        if (ms.mapped()) {
            ms.read(data);
            print(data);
            data = {5,4,3,2,1,0};
            ms.write(data);
        }
        else {
            print("You must run A first!");
        }
    }
    return 0;
}