#pragma once
#include <string>

namespace mel {

    class UnityGame {

    public:

        UnityGame(std::string name, std::string path);

        void launch();
        void close();

        std::string name_;
        std::string path_;

    };

}