#include <MEL/Communications/MelShare2.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Utility/Options.hpp>

using namespace mel;

struct Thing {
    int x;
};

int main(int argc, char* argv[]) {
    Options options("melshare2.exe", "MelShare2");
    options.add_options()("a", "Runs A")("b", "Runs B");
    auto input = options.parse(argc, argv);

    if (input.count("a")) {
        MelShare2<Thing> ms("ms2", OpenOrCreate);
        Thing thing;
        thing.x = 5;
        std::vector<Thing> data = { thing };
        ms.write(data);
        prompt("Press Enter to exit.");
    }
    else if (input.count("b")) {
        MelShare2<Thing> ms("ms2", OpenOnly);
        if (ms.is_mapped()) {
            auto data = ms.read();
            print(data[0].x);
        }
        else {
            print("You must run A first!");
        }
    }
    return 0;
}