#include <MEL/Math/Random.hpp>
#include <MEL/Utility/Types.hpp>
#include <cassert>
#include <ctime>
#include <random>

namespace mel {

    namespace {

    #ifdef MEL_USE_STD_RANDOM_ENGINE
    // Use Mersenne Twister as default standard random engine
    typedef std::mt19937 Engine;
    #else
    // Random generator engine (Multiply With Carry)
    class Engine {
    public:
        // Type definition for usage inside Std.Random
        typedef uint32 result_type;
    public:
        // Constructor
        explicit Engine(uint32 seedVal) { seed(seedVal); }
        // Return random number
        uint32 operator()() {
            const uint64 a = 1967773755;
            x = a * (x & 0xffffffff) + (x >> 32);
            return static_cast<uint32>(x);
        }
        // set seed (compliant to Std.Random)
        void seed(uint32 seedVal = 0) { x = seedVal + !seedVal; }
        // Return minimal value (compliant to Std.Random)
        static constexpr uint32 min() { return 0; }
        // Return maximal value (compliant to Std.Random)
        static constexpr uint32 max() { return 0xffffffff; }
    private:
        uint64 x;
    };

    #endif  // MEL_USE_STD_RANDOM_ENGINE

    // Function initializing the engine and its seed at startup time
    Engine createInitialEngine() {
        return Engine(static_cast<unsigned long>(std::time(nullptr)));
    }

    // Pseudo random number generator engine
    Engine globalEngine = createInitialEngine();

    }  // namespace

    int random(int min, int max) {
        assert(min <= max);
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(globalEngine);
    }

    unsigned int random(unsigned int min, unsigned int max) {
        assert(min <= max);
        std::uniform_int_distribution<unsigned int> distribution(min, max);
        return distribution(globalEngine);
    }

    double random(double min, double max) {
        assert(min <= max);
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(globalEngine);
    }

    double randomDev(double middle, double deviation) {
        assert(deviation >= 0.f);
        return random(middle - deviation, middle + deviation);
    }

    void setRandomSeed(unsigned long seed) {
        globalEngine.seed(seed);
    }

}
