#include "MelShare.h"
#include <iostream>   

namespace mel {

    MelShare::MelShare(const char* name, const unsigned int bytes) : name_(name), bytes_(bytes) {
        try {
            // create a new windows shared memory 
            // (note this MelShare must remain in scope for as long as the shared memory needs to be accessed)
            shm_ = ip::windows_shared_memory(ip::create_only, name_, ip::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);

            //values_ = (int(*)[16])(region_.get_address());
            //values_ = new int[size_];
            //values_ = (int*)(region_.get_address());

            // remove possibly lingering mutex
            strcpy(mutex_name_, name);
            strcat(mutex_name_, "_mutex");
            ip::named_mutex::remove(mutex_name_);
        }
        catch (ip::interprocess_exception &ex) {
            std::cout << "ERROR: Failed to create or open shared memory <" << name_ << ">." << std::endl;
        }
    }
    
    MelShare::~MelShare() {
        ip::named_mutex::remove(mutex_name_);
    }

}