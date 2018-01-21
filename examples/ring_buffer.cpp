
#include <MEL/Utility/RingBuffer.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main() {
    RingBuffer<int> x(5);
    std::string method;
    int idx;
    int value;
    while(true) {
        std::cin >> method;
        if (method == "push_back") {
            std::cin >> value;
            x.push_back(value);
        }
        else if (method == "push_front") {
            std::cin >> value;
            x.push_front(value);
        }
        else if (method == "pop_back") {
            print("a: ", false);
            print(x.pop_back());
        }
        else if (method == "pop_front") {
            print("a: ", false);
            print(x.pop_front());
        }
        else if (method == "[]") {
            std::cin >> idx;
            print("a: ", false);
            print(x[idx]);
        }
        else if (method == "[]=") {
            std::cin >> idx;
            std::cin >> value;
            print("a: ", false);
            x[idx] = value;
        }
        else if (method == "resize") {
            std::cin >> value;
            x.resize(value);
        }
        print(x.buffer_, false);
        print(" f:" + stringify(x.front_) +
         "  b:" + stringify(x.back_) +
         "  e:" + stringify(x.empty()) +
         "  f:" + stringify(x.full()) +
         "  s:" + stringify(x.size()) + "  ", false);
        print("x: ", false);
        for(std::size_t i = 0; i < x.size(); ++i) {

            std::cout << x[i] << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
