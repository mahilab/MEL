#include <Windows.h>
#include <tchar.h>
#include "util.h"
#include "ExternalApp.h"

namespace mel {

    ExternalApp::ExternalApp(std::string name, std::string path) : name_(name), path_(path) {

    }

    void ExternalApp::launch() {

        std::string open = "open";
        std::wstring wopen = std::wstring(open.begin(), open.end());
        std::wstring wpath = std::wstring(path_.begin(), path_.end());

        int result;
        result = (int)ShellExecute(NULL, wopen.c_str(), wpath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
        if (result <= 0) {
            print("ERROR: Failed to launch ExternalApp " + namify(name_) + ".");
            print("WINDOWS ERROR: " + get_last_windows_error_message());
        }
    }

}