#include "MelShare.h"
#include <string>   

namespace mel {

    MelShare::MelShare(char* name, int size) : name_(name), size_(size) {
        // remove any shared memory obejects with the same name
        boost::interprocess::shared_memory_object::remove(name_);
        shmem_ = boost::interprocess::shared_memory_object(boost::interprocess::create_only, name_, boost::interprocess::read_write);
        shmem_.truncate(4 * 16);
        region_ = boost::interprocess::mapped_region(shmem_, boost::interprocess::read_write);
        //values_ = (int(*)[16])(region_.get_address());
        values_ = new int[size_];
        values_ = (int*)(region_.get_address());
        for (int i = 0; i < size; i++)
            values_[i] = 0;
    }
    
    MelShare::~MelShare() {
        boost::interprocess::shared_memory_object::remove(name_);
    }
}