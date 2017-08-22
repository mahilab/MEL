#include "Mutex.h"
#include <Windows.h>
#include <iostream>
#include "util.h"
#include <string>

namespace mel {

    Mutex::Mutex() : name_(""), mode_(Mode::OPEN) {}

    Mutex::Mutex(std::string name, Mutex::Mode mode) : 
        name_(name),
        mode_(mode)
    {
       std::wstring w_name = std::wstring(name_.begin(), name_.end());
       if (mode == Mode::CREATE) {
           handle_ = CreateMutex(NULL, FALSE, w_name.c_str());
           if (handle_ == NULL) {
               std::cout << "ERROR: Failed to create mutex <" << name_ << ">." << std::endl;
               printf("WINDOWS ERROR: %d\n", GetLastError());
           }
           else {
               if (GetLastError() == ERROR_ALREADY_EXISTS) {
                   std::cout << "WARNING: CreateMutex opened an existing mutex when trying to create mutex <" << name_ << ">." << std::endl;
               }
               ReleaseMutex(handle_);
           }
       }
       else if (mode == Mode::OPEN) {
           handle_ = OpenMutex(MUTEX_ALL_ACCESS, FALSE, w_name.c_str());
       }
    }

    Mutex::~Mutex() {
        ReleaseMutex(handle_);
        CloseHandle(handle_);
    }

    Mutex::Result Mutex::try_lock() {
        if (handle_ != NULL) {
            DWORD dwWaitResult;
            dwWaitResult = WaitForSingleObject(handle_, INFINITE);
            switch (dwWaitResult) {
            case WAIT_OBJECT_0:
                return Result::LOCK_ACQUIRED;
            case WAIT_ABANDONED:
                std::cout << "ERROR: Wait on mutex <" << name_ << "> abandoned." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
                return Result::LOCK_ABANDONED;
            case WAIT_TIMEOUT:
                std::cout << "ERROR: Wait on mutex <" << name_ << "> timed out." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
                return Result::LOCK_TIMEOUT;
            case WAIT_FAILED:
                std::cout << "ERROR: Wait on mutex <" << name_ << "> failed." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
                return Result::LOCK_FAILED;
            }
        } 
        else {
            std::cout << "ERROR: Failed to open mutex <" << name_ << ">." << std::endl;
            printf("WINDOWS ERROR: %d\n", GetLastError());
            return Result::NOT_OPEN;
        }
    }

    Mutex::Result Mutex::release() {
        if (!ReleaseMutex(handle_)) {
            std::cout << "ERROR: Failed to release mutex <" << name_ << ">." << std::endl;
            printf("WINDOWS ERROR: %d\n", GetLastError());
            return Result::RELEASE_FAILED;
        }        
    }
}