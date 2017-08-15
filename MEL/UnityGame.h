#pragma once
#include <string>

class UnityGame {

public:

    UnityGame(std::string name, std::string path) : name_(name) {}

    void launch();

    std::string name_;



    



};