#include <Windows.h>
#include <tchar.h>
#include "mel_util.h"
#include "ExternalApp.h"

namespace mel {

    namespace util {

        ExternalApp::ExternalApp(std::string name, std::string path) : name_(name), path_(path) {

        }

        void ExternalApp::launch() {

            std::string open = "open";
            std::wstring wopen = std::wstring(open.begin(), open.end());
            std::wstring wpath = std::wstring(path_.begin(), path_.end());

            ShellExecute(NULL, wopen.c_str(), wpath.c_str(), NULL, NULL, SW_SHOWDEFAULT);

        }

    }

}