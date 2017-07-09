#include "MelShare.h"
#include <iostream>   

namespace mel {

    MelShare::MelShare(char* name, unsigned int bytes) : name_(name), bytes_(bytes) {
        try {
            // create a new windows shared memory 
            // (note this MelShare must remain in scope for as long as the shared memory needs to be accessed)
            shm_ = ip::windows_shared_memory(ip::open_or_create, name_, ip::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);

            //values_ = (int(*)[16])(region_.get_address());
            //values_ = new int[size_];
            //values_ = (int*)(region_.get_address());

            mutex_name_ = std::string(name) + "_mutex";
            mutex_name_w_ = std::wstring(mutex_name_.begin(), mutex_name_.end());
            mutex_ = CreateMutex(NULL, FALSE, mutex_name_w_.c_str());
            if (mutex_ == NULL) {
                std::cout << "ERROR: Failed to create mutex <" << name << ">." << std::endl;
                printf("WINDOWS ERROR: %d\n", GetLastError());
            }
            else {
                ReleaseMutex(mutex_);
            }
        }
        catch (ip::interprocess_exception &ex) {
            std::cout << "ERROR: Failed to create or open shared memory <" << name_ << ">." << std::endl;
        }
    }
    
    MelShare::~MelShare() {
        ReleaseMutex(mutex_);
        CloseHandle(mutex_);
    }


}