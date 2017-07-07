#pragma once
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>


namespace mel {


    class MelShare {

    public:

        MelShare(char* name, int size);
        ~MelShare();

        const char* name_;
        //int(*values_)[16];
        int* values_;
        int size_;

        boost::interprocess::shared_memory_object shmem_;
        boost::interprocess::mapped_region region_;

    };

}