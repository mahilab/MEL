#include <MEL/Utility/Console.hpp>
#include <csignal>
#include <iostream>
#include <io.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace mel {

//==============================================================================
// CONSTANTS
//==============================================================================

#ifdef _WIN32
HANDLE stdout_handle = GetStdHandle(static_cast<DWORD>(-11));
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
struct Initializer {
    Initializer() { GetConsoleScreenBufferInfo(stdout_handle, &csbiInfo); }
    ~Initializer() { SetConsoleTextAttribute(stdout_handle, csbiInfo.wAttributes); }
};
Initializer initializer;
#endif

//==============================================================================
// CONSOLE OUTPUT
//==============================================================================

#ifdef _WIN32
void print_string(const std::string& str) {
    WriteConsoleA(stdout_handle, str.c_str(), static_cast<DWORD>(str.size()),
                  NULL, NULL);
}
#else
void print(const std::string& str) {
    std::cout << str;
}
#endif

//==============================================================================
// CONSOLE INPUT
//==============================================================================

void prompt(const std::string& message) {
    print_string(message);
    getchar();
}

bool register_ctrl_handler(int(*handler)(unsigned long)) {
#ifdef _WIN32
    return !!SetConsoleCtrlHandler((PHANDLER_ROUTINE)handler, TRUE);
#else
    signal(SIGINT, function);
#endif
}

//==============================================================================
// CONSOLE FORMAT
//==============================================================================

#ifdef _WIN32
WORD get_color(Color color, bool background) {
    DWORD val = 0;
    switch(color) {
        case Color::None:
            if (background)
                return (csbiInfo.wAttributes & 0xf0);
            val = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::Black:
            break;
        case Color::White:
            val = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::Red:
            val = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case Color::Green:
            val = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case Color::Blue:
            val = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::Cyan:
            val = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::Magenta:
            val = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::Yellow:
            val = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
    }
    if (background)
        return static_cast<WORD>(val * 16);
    else
        return static_cast<WORD>(val);
}

void set_text_color(Color foreground, Color background) {
    SetConsoleTextAttribute(stdout_handle, get_color(foreground, false) | get_color(background, true));
}

void reset_text_color() {
    SetConsoleTextAttribute(stdout_handle, csbiInfo.wAttributes);
}

#else

void set_text_color(Color foreground, Color background) {
    // TOOD
}

void reset_text_color() {
    // TODO
}

#endif

//==============================================================================
// MISIC
//==============================================================================

#ifdef _WIN32
const bool STDOUT_IS_A_TTY = !!_isatty(_fileno(stdout));
#else
const bool STDOUT_IS_A_TTY = !!isatty(fileno(stdout));
#endif

}  // namespace mel
