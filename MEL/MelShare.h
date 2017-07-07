#pragma once
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <vector>
#include <chrono>


namespace mel {

    namespace ip = boost::interprocess;
    
    class MelShare {

    public:

        MelShare(const char* name, const unsigned int bytes = 256);
        ~MelShare();

        // STATIC READ/WRITE FUNCTIONS (CAN BE CALLED ANYWHERE)

        template <typename T>
        static int  read_map(const char* name, T* buffer, int size) {
            char mutex_name[32];
            strcpy(mutex_name, name);
            strcat(mutex_name, "_mutex");
            ip::named_mutex mutex(ip::open_or_create, mutex_name);
            try {
                ip::scoped_lock<ip::named_mutex> lock(mutex);
                ip::windows_shared_memory shm(ip::open_only, name, ip::read_only);
                ip::mapped_region region(shm, ip::read_only);
                volatile T* temp = static_cast<T*>(region.get_address());
                for (int i = 0; i < size; i++) {
                    buffer[i] = temp[i];
                }
                return 1;
            }
            catch (ip::interprocess_exception &ex) {
                return 0;
            }
        }

        template <typename T>
        static int read_map(const char* name, std::vector<T>& buffer) {
            return read_map(name, &buffer[0], buffer.size());
        }

        template <typename T>
        static int  write_map(const char* name, T* buffer, int size) {
            //std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            char mutex_name[32];
            strcpy(mutex_name, name);
            strcat(mutex_name, "_mutex");
           // ip::named_mutex mutex(ip::open_or_create, mutex_name);
            try {
                //ip::scoped_lock<ip::named_mutex> lock(mutex);
                ip::windows_shared_memory shm(ip::open_only, name, ip::read_write);
                ip::mapped_region region(shm, ip::read_write);
                volatile T* temp = static_cast<T*>(region.get_address());
                for (int i = 0; i < size; i++) {
                    temp[i] = buffer[i];
                }
                //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                //std::chrono::nanoseconds duration = end - start;
                //std::cout << duration.count() / 1000000000.0 << std::endl;
                return 1;
            }
            catch (ip::interprocess_exception &ex) {
                return 0;
            }
        }

        template <typename T>
        static int  write_map(const char* name, std::vector<T>& buffer) {
            return write_map(name, &buffer[0], buffer.size());
        }

        // NON-STATIC READ/WRITE FUNCTIONS (CALLED ONLY FROM AN INSTANCE)

        template<typename T>
        int read(T* buffer, int size) {
            return read_map(name_, buffer, size); }

        template<typename T>
        int read(std::vector<T>& buffer) { 
            return read_map(name_, buffer); }

        template<typename T>
        int write(T* buffer, int size) { 
            return write_map(name_, buffer, size); }

        template<typename T>
        int write(std::vector<T>& buffer) { 
            return write_map(name_, buffer); }

    private:

        const char* name_;
        const unsigned int bytes_;
        ip::windows_shared_memory shm_;
        ip::mapped_region region_;
        char mutex_name_[32];
        //int(*values_)[16];
        int* values_;

    };

}