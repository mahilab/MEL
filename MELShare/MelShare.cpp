#include "MelShare.h"

// error codes
//  1 = successful read/write
// -1 = failed to open shared memory map
// -2 = failed to open mutex
// -3 = wait on mutex abandoned
// -4 = wait on mutex timed out
// -5 = wait on mutex failed
// -6 = failed to release mutex
// -7 = failed to close mutex handle

namespace mel {

    namespace share {

        MelShare::MelShare(char* name, unsigned int bytes) : name_(name), bytes_(bytes) {
            try {
                // create a new windows shared memory 
                // (note this MelShare must remain in scope for as long as the shared memory needs to be accessed)
                shm_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_, boost::interprocess::read_write, bytes_);
                region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);

                std::string mutex_name_s = std::string(name) + "_mutex";
                mutex_name_ = std::wstring(mutex_name_s.begin(), mutex_name_s.end());
                mutex_ = CreateMutex(NULL, FALSE, mutex_name_.c_str());
                if (mutex_ == NULL) {
                    std::cout << "ERROR: Failed to create mutex <" << name << ">." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                }
                else {
                    ReleaseMutex(mutex_);
                }
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to create or open shared memory <" << name_ << ">." << std::endl;
            }
        }

        MelShare::MelShare(const MelShare& other) {
            name_ = other.name_;
            bytes_ = other.bytes_;
            shm_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_, boost::interprocess::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_ = other.mutex_;
        }

        MelShare& MelShare::operator=(const MelShare& other) {
            name_ = other.name_;
            bytes_ = other.bytes_;
            shm_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_, boost::interprocess::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_ = other.mutex_;
            return *this;
        }

        MelShare::~MelShare() {
            ReleaseMutex(mutex_);
            CloseHandle(mutex_);
        }

        // C Linkages for Python and C#

        int read_char_map(char* name, char* buffer, int size) {
            return read_map(name, buffer, size);
        }

        int read_int_map(char* name, int* buffer, int size) {
            return read_map(name, buffer, size);
        }

        int read_double_map(char* name, double* buffer, int size) {
            return read_map(name, buffer, size);
        }

        int write_char_map(char* name, char* buffer, int size) {
            return write_map(name, buffer, size);
        }

        int write_int_map(char* name, int* buffer, int size) {
            return write_map(name, buffer, size);
        }

        int write_double_map(char* name, double* buffer, int size) {
            return write_map(name, buffer, size);
        }


    }

}