#include "UnityGame.h"
#include <Windows.h>
#include <tchar.h>


void UnityGame::launch() {
    char aux_string[255];

    wchar_t *waux_string = new wchar_t[strlen(aux_string) + 1]; //memory allocation
    mbstowcs(waux_string, aux_string, strlen(aux_string) + 1);
    //ShellExecute(NULL, "open", "C:\\Users\\MAHI\\Dropbox\\OpenWrist\\Software\\Games\\AJLS\\AJLS_Unity_5.6.1f1\\Builds\\AJLS.exe", NULL, NULL, SW_SHOWDEFAULT)
}