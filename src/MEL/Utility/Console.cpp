#include <MEL/Utility/Console.hpp>
#include <csignal>
#include <iostream>

#ifdef __linux__
#include <unistd.h>
#elif _WIN32
#include <io.h>
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
    Initializer() {
        GetConsoleScreenBufferInfo(stdout_handle, &csbiInfo);
    }
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
void print_string(const std::string& str) {
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

bool (*ctrl_handler)(CtrlEvent);

#ifdef _WIN32
BOOL win_handler(DWORD var) {
    if (var == CTRL_C_EVENT)
        return static_cast<int>(ctrl_handler(CtrlEvent::CtrlC));
    else if (var == CTRL_BREAK_EVENT)
        return static_cast<int>(ctrl_handler(CtrlEvent::CtrlQuit));
    else if (var == CTRL_CLOSE_EVENT)
        return static_cast<int>(ctrl_handler(CtrlEvent::Close));
    else if (var == CTRL_LOGOFF_EVENT)
        return static_cast<int>(ctrl_handler(CtrlEvent::Logoff));
    else if (var == CTRL_SHUTDOWN_EVENT)
        return static_cast<int>(ctrl_handler(CtrlEvent::Shutdown));
    else
        return 0;
}

bool register_ctrl_handler(bool (*handler)(CtrlEvent)) {
    ctrl_handler = handler;
    return !!SetConsoleCtrlHandler((PHANDLER_ROUTINE)win_handler, TRUE);
}
#else

void posix_handler(int sig) {
    if (sig == SIGINT)
        ctrl_handler(CtrlEvent::CtrlC);
    else if (sig == SIGQUIT)
        ctrl_handler(CtrlEvent::CtrlQuit);
}

bool register_ctrl_handler(bool (*handler)(CtrlEvent)) {
    ctrl_handler = handler;
    signal(SIGINT, posix_handler);
    signal(SIGQUIT, posix_handler);
    return true;
}

#endif


//==============================================================================
// CONSOLE FORMAT
//==============================================================================

#ifdef _WIN32

WORD get_color(Color color, bool background) {
    DWORD val = 0;
    switch(color) {

        case Color::None:
            if (background)
                return (csbiInfo.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY));
            val = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;

        case Color::Black:
            break;

        case Color::White:
            val = FOREGROUND_INTENSITY;
        case Color::Gray:
            val = val | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;

        case Color::Red:
            val = FOREGROUND_INTENSITY;
        case Color::DarkRed:
            val = val | FOREGROUND_RED;
            break;

        case Color::Green:
            val = FOREGROUND_INTENSITY;
        case Color::DarkGreen:
            val = val | FOREGROUND_GREEN;
            break;

        case Color::Blue:
            val = FOREGROUND_INTENSITY;
        case Color::DarkBlue:
            val = val | FOREGROUND_BLUE;
            break;

        case Color::Cyan:
            val = FOREGROUND_INTENSITY;
        case Color::Aqua:
            val = val | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;

        case Color::Magenta:
            val = FOREGROUND_INTENSITY;
        case Color::Purple:
            val = val | FOREGROUND_RED | FOREGROUND_BLUE;
            break;

        case Color::Yellow:
            val = FOREGROUND_INTENSITY;
        case Color::Gold:
            val = val | FOREGROUND_RED | FOREGROUND_GREEN;
            break;

    }
    if (background)
        return static_cast<WORD>(val * 16);
    else
        return static_cast<WORD>(val);
}

void set_text_color(Color foreground, Color background) {
    WORD attributes = get_color(foreground, false) | get_color(background, true);
    SetConsoleTextAttribute(stdout_handle, attributes);
}

void reset_text_color() {
    SetConsoleTextAttribute(stdout_handle, csbiInfo.wAttributes);
}

#else

void set_text_color(Color foreground, Color background) {
    // background
    if (background == Color::None)
        std::cout << "\x1B[0m";
    else if (background == Color::Black)
        std::cout << "\x1B[40m";
    else if (background == Color::Gray)
        std::cout << "\x1B[100m";
    else if (background == Color::White)
        std::cout << "\x1B[107m";
    else if (background == Color::Red)
        std::cout << "\x1B[101m";
    else if (background == Color::DarkRed)
        std::cout << "\x1B[41m";
    else if (background == Color::Green)
        std::cout << "\x1B[102m";
    else if (background == Color::DarkGreen)
        std::cout << "\x1B[42m";
    else if (background == Color::Blue)
        std::cout << "\x1B[104m";
    else if (background == Color::DarkBlue)
        std::cout << "\x1B[44m";
    else if (background == Color::Cyan)
        std::cout << "\x1B[106m";
    else if (background == Color::Aqua)
        std::cout << "\x1B[46m";
    else if (background == Color::Magenta)
        std::cout << "\x1B[105m";
    else if (background == Color::Purple)
        std::cout << "\x1B[45m";
    else if (background == Color::Yellow)
        std::cout << "\x1B[103m";
    else if (background == Color::Gold)
        std::cout << "\x1B[43m";
    // set foreground
    if (foreground == Color::None)
        std::cout << "\x1B[97m";
    else if (foreground == Color::Black)
        std::cout << "\x1B[30m";
    else if (foreground == Color::Gray)
        std::cout << "\x1B[90m";
    else if (foreground == Color::White)
        std::cout << "\x1B[97m";
    else if (foreground == Color::Red)
        std::cout << "\x1B[91m";
    else if (foreground == Color::DarkRed)
        std::cout << "\x1B[31m";
    else if (foreground == Color::Green)
        std::cout << "\x1B[92m";
    else if (foreground == Color::DarkGreen)
        std::cout << "\x1B[32m";
    else if (foreground == Color::Blue)
        std::cout << "\x1B[94m";
    else if (foreground == Color::DarkBlue)
        std::cout << "\x1B[34m";
    else if (foreground == Color::Cyan)
        std::cout << "\x1B[96m";
    else if (foreground == Color::Aqua)
        std::cout << "\x1B[36m";
    else if (foreground == Color::Magenta)
        std::cout << "\x1B[95m";
    else if (foreground == Color::Purple)
        std::cout << "\x1B[35m";
    else if (foreground == Color::Yellow)
        std::cout << "\x1B[93m";
    else if (foreground == Color::Gold)
        std::cout << "\x1B[33m";
}

void reset_text_color() {
    std::cout << "\x1B[0m\x1B[0K";
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
