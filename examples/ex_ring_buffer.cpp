
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
            print_string("a: ");
            print(x.pop_back());
        }
        else if (method == "pop_front") {
            print_string("a: ");
            print(x.pop_front());
        }
        else if (method == "[]") {
            std::cin >> idx;
            print_string("a: ");
            print(x[idx]);
        }
        else if (method == "[]=") {
            std::cin >> idx;
            std::cin >> value;
            print_string("a: ");
            x[idx] = value;
        }
        else if (method == "resize") {
            std::cin >> value;
            x.resize(value);
        }
        else if (method == "clear") {
            x.clear();
        }
        for(std::size_t i = 0; i < x.size(); ++i) {

            std::cout << x[i] << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
