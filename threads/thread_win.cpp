#include <Windows.h>
#include <winbase.h>

#include <cstdio>
#include <iostream>

void SubThread(void) {
    int i;
    for (i = 0; i < 5; i++) {
        std::cout << "SubThread" << std::endl;
        Sleep(2000);
    }
}

int main(int argc, char *argv[]) {
    std::cout << "Create Thread" << std::endl;
    DWORD IDThread;
    HANDLE hThread;
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SubThread, NULL, 0,
                           &IDThread);
    if (hThread == NULL) {
        std::cout << "Create Thread Error" << std::endl;
    }
    int i;
    for (i = 0; i < 5; i++) {
        std::cout << "Main Thread: " << i << std::endl;
        if (i == 1) {
            if (SuspendThread(hThread) == 0xFFFFFFFF) {
                std::cout << "Suspend Thread Error." << std::endl;
            } else {
                std::cout << "Suspend Thread is ok." << std::endl;
            }
        }
        if (i == 3) {
            if (ResumeThread(hThread) == 0xFFFFFFFF) {
                std::cout << "Resume Thread Error!" << std::endl;
            } else {
                std::cout << "Resume Thread is ok" << std::endl;
            }
        }
        Sleep(4000);
    }
}