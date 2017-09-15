#include "Input.h"
#include <Windows.h>
#include <csignal>
#include "mel_util.h"
#include <iostream>
#include <string>

namespace mel {

    namespace util {

        bool Input::is_key_pressed(Input::Key key)
        {
            int vkey = 0;
            switch (key)
            {
            default:                vkey = 0;             break;
            case Input::A:          vkey = 'A';           break;
            case Input::B:          vkey = 'B';           break;
            case Input::C:          vkey = 'C';           break;
            case Input::D:          vkey = 'D';           break;
            case Input::E:          vkey = 'E';           break;
            case Input::F:          vkey = 'F';           break;
            case Input::G:          vkey = 'G';           break;
            case Input::H:          vkey = 'H';           break;
            case Input::I:          vkey = 'I';           break;
            case Input::J:          vkey = 'J';           break;
            case Input::K:          vkey = 'K';           break;
            case Input::L:          vkey = 'L';           break;
            case Input::M:          vkey = 'M';           break;
            case Input::N:          vkey = 'N';           break;
            case Input::O:          vkey = 'O';           break;
            case Input::P:          vkey = 'P';           break;
            case Input::Q:          vkey = 'Q';           break;
            case Input::R:          vkey = 'R';           break;
            case Input::S:          vkey = 'S';           break;
            case Input::T:          vkey = 'T';           break;
            case Input::U:          vkey = 'U';           break;
            case Input::V:          vkey = 'V';           break;
            case Input::W:          vkey = 'W';           break;
            case Input::X:          vkey = 'X';           break;
            case Input::Y:          vkey = 'Y';           break;
            case Input::Z:          vkey = 'Z';           break;
            case Input::Num0:       vkey = '0';           break;
            case Input::Num1:       vkey = '1';           break;
            case Input::Num2:       vkey = '2';           break;
            case Input::Num3:       vkey = '3';           break;
            case Input::Num4:       vkey = '4';           break;
            case Input::Num5:       vkey = '5';           break;
            case Input::Num6:       vkey = '6';           break;
            case Input::Num7:       vkey = '7';           break;
            case Input::Num8:       vkey = '8';           break;
            case Input::Num9:       vkey = '9';           break;
            case Input::Escape:     vkey = VK_ESCAPE;     break;
            case Input::LControl:   vkey = VK_LCONTROL;   break;
            case Input::LShift:     vkey = VK_LSHIFT;     break;
            case Input::LAlt:       vkey = VK_LMENU;      break;
            case Input::LSystem:    vkey = VK_LWIN;       break;
            case Input::RControl:   vkey = VK_RCONTROL;   break;
            case Input::RShift:     vkey = VK_RSHIFT;     break;
            case Input::RAlt:       vkey = VK_RMENU;      break;
            case Input::RSystem:    vkey = VK_RWIN;       break;
            case Input::Menu:       vkey = VK_APPS;       break;
            case Input::LBracket:   vkey = VK_OEM_4;      break;
            case Input::RBracket:   vkey = VK_OEM_6;      break;
            case Input::SemiColon:  vkey = VK_OEM_1;      break;
            case Input::Comma:      vkey = VK_OEM_COMMA;  break;
            case Input::Period:     vkey = VK_OEM_PERIOD; break;
            case Input::Quote:      vkey = VK_OEM_7;      break;
            case Input::Slash:      vkey = VK_OEM_2;      break;
            case Input::BackSlash:  vkey = VK_OEM_5;      break;
            case Input::Tilde:      vkey = VK_OEM_3;      break;
            case Input::Equal:      vkey = VK_OEM_PLUS;   break;
            case Input::Dash:       vkey = VK_OEM_MINUS;  break;
            case Input::Space:      vkey = VK_SPACE;      break;
            case Input::Return:     vkey = VK_RETURN;     break;
            case Input::Enter:      vkey = VK_RETURN;     break;
            case Input::BackSpace:  vkey = VK_BACK;       break;
            case Input::Tab:        vkey = VK_TAB;        break;
            case Input::PageUp:     vkey = VK_PRIOR;      break;
            case Input::PageDown:   vkey = VK_NEXT;       break;
            case Input::End:        vkey = VK_END;        break;
            case Input::Home:       vkey = VK_HOME;       break;
            case Input::Insert:     vkey = VK_INSERT;     break;
            case Input::Delete:     vkey = VK_DELETE;     break;
            case Input::Add:        vkey = VK_ADD;        break;
            case Input::Subtract:   vkey = VK_SUBTRACT;   break;
            case Input::Multiply:   vkey = VK_MULTIPLY;   break;
            case Input::Divide:     vkey = VK_DIVIDE;     break;
            case Input::Left:       vkey = VK_LEFT;       break;
            case Input::Right:      vkey = VK_RIGHT;      break;
            case Input::Up:         vkey = VK_UP;         break;
            case Input::Down:       vkey = VK_DOWN;       break;
            case Input::Numpad0:    vkey = VK_NUMPAD0;    break;
            case Input::Numpad1:    vkey = VK_NUMPAD1;    break;
            case Input::Numpad2:    vkey = VK_NUMPAD2;    break;
            case Input::Numpad3:    vkey = VK_NUMPAD3;    break;
            case Input::Numpad4:    vkey = VK_NUMPAD4;    break;
            case Input::Numpad5:    vkey = VK_NUMPAD5;    break;
            case Input::Numpad6:    vkey = VK_NUMPAD6;    break;
            case Input::Numpad7:    vkey = VK_NUMPAD7;    break;
            case Input::Numpad8:    vkey = VK_NUMPAD8;    break;
            case Input::Numpad9:    vkey = VK_NUMPAD9;    break;
            case Input::F1:         vkey = VK_F1;         break;
            case Input::F2:         vkey = VK_F2;         break;
            case Input::F3:         vkey = VK_F3;         break;
            case Input::F4:         vkey = VK_F4;         break;
            case Input::F5:         vkey = VK_F5;         break;
            case Input::F6:         vkey = VK_F6;         break;
            case Input::F7:         vkey = VK_F7;         break;
            case Input::F8:         vkey = VK_F8;         break;
            case Input::F9:         vkey = VK_F9;         break;
            case Input::F10:        vkey = VK_F10;        break;
            case Input::F11:        vkey = VK_F11;        break;
            case Input::F12:        vkey = VK_F12;        break;
            case Input::F13:        vkey = VK_F13;        break;
            case Input::F14:        vkey = VK_F14;        break;
            case Input::F15:        vkey = VK_F15;        break;
            case Input::Pause:      vkey = VK_PAUSE;      break;
            }
            return (GetAsyncKeyState(vkey) & 0x8000) != 0;
        }

        bool Input::is_console_window_focused() {
            return (GetConsoleWindow() == GetForegroundWindow());
        }

        void Input::ignore_ctrl_c() {
            signal(SIGINT, SIG_IGN);
        }

        void Input::set_console_echo(bool enabled) {
            HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
            DWORD mode;
            GetConsoleMode(hStdin, &mode);

            if (!enabled)
                mode &= ~ENABLE_ECHO_INPUT;
            else
                mode |= ENABLE_ECHO_INPUT;

            SetConsoleMode(hStdin, mode);
        }

        void Input::clear_console_input_buffer()
        {
            // If you happen to have any trouble clearing already cleared buffer, uncomment the section below.
            keybd_event('S', 0, 0, 0);
            keybd_event('S', 0,KEYEVENTF_KEYUP, 0);
            keybd_event(VK_BACK, 0, 0, 0);
            keybd_event(VK_BACK, 0,KEYEVENTF_KEYUP, 0);
            PINPUT_RECORD ClearingVar1 = new INPUT_RECORD[256];
            DWORD ClearingVar2;
            ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ClearingVar1, 256, &ClearingVar2);
            delete[] ClearingVar1;
        }

        void Input::wait_for_key(Key key, bool require_focus) {
            while (true) {
                if (is_key_pressed(key)) {
                    if (require_focus) {
                        if (is_console_window_focused()) {
                            return;
                        }
                    }
                    else {
                        return;
                    }
                }
                Sleep(10);
            }
            clear_console_input_buffer();
        }

        Input::Key Input::wait_for_keys(std::vector<Key> keys, bool require_focus) {
            while (true) {
                for (int i = 0; i < keys.size(); ++i) {
                    if (is_key_pressed(keys[i])) {
                        if (require_focus) {
                            if (is_console_window_focused()) {
                                return keys[i];
                            }
                        }
                        else {
                            return keys[i];
                        }
                    }
                }
                Sleep(10);
            }
            clear_console_input_buffer();
        }

        void Input::acknowledge(std::string message, Key key) {
            util::print(message);
            wait_for_key(key);
        }

    }

}

// Implementation Resources:
// Key enums: https://github.com/SFML/SFML
// echo mode: https://stackoverflow.com/questions/1413445/reading-a-password-from-stdcin
// clear buffer: https://stackoverflow.com/questions/20222277/delete-key-press-historygetkeystate-c
// to do: https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events