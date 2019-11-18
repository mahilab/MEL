#include <MEL/Utility/Windows/ExternalApp.hpp>
#include <windows.h>
#include <tchar.h>

namespace mel {

ExternalApp::ExternalApp(std::string name, std::string path) : name_(name), path_(path) {

}

void ExternalApp::launch() {

    std::string open = "open";
    std::wstring wopen = std::wstring(open.begin(), open.end());
    std::wstring wpath = std::wstring(path_.begin(), path_.end());

    // ShellExecute(NULL, wopen.c_str(), wpath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
    ShellExecute(NULL, open.c_str(), path_.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

void ExternalApp::launch(std::string params) {

    std::string open = "open";
    std::wstring wopen = std::wstring(open.begin(), open.end());
    std::wstring wpath = std::wstring(path_.begin(), path_.end());

    // ShellExecute(NULL, wopen.c_str(), wpath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
    ShellExecute(NULL, open.c_str(), path_.c_str(), params.c_str(), NULL, SW_SHOWDEFAULT);
}

} // namespace mel
