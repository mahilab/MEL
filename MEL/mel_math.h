
namespace mel {

    namespace math {

        /// Returns -1 for negative numbers, 1 for positive numbers, and 0 for numbers equal to 0.
        template <typename T> int sign(T val) {
            return (T(0) < val) - (val < T(0));
        }

    }
}

