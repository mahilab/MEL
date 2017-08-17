#include "UnityGame.h"
#include <Windows.h>
#include <tchar.h>
#include "util.h"

namespace mel {

    UnityGame::UnityGame(std::string name, std::string path) : name_(name), path_(path) {

    }


    void UnityGame::launch() {

        print(path_);

        std::string open = "open";
        std::wstring wopen = std::wstring(open.begin(), open.end());
        std::wstring wpath = std::wstring(path_.begin(), path_.end());

        ShellExecute(NULL, wopen.c_str(), wpath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
    }

}