#include "MelShare.h"
namespace mel {

    namespace share {

        MelShare::MelShare(char* name, unsigned int bytes) : name_(name), bytes_(bytes) {
            try {
                // create a new windows shared memory 
                // (note this MelShare must remain in scope for as long as the shared memory needs to be accessed)
                shm_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_, boost::interprocess::read_write, bytes_);
                region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);

                mutex_name_ = std::string(name) + "_mutex";
                std::wstring w_mutex_name = std::wstring(mutex_name_.begin(), mutex_name_.end());
                mutex_ = CreateMutex(NULL, FALSE, w_mutex_name.c_str());
                if (mutex_ == NULL) {
                    std::cout << "ERROR: Failed to create mutex <" << mutex_name_ << ">." << std::endl;
                    printf("WINDOWS ERROR: %d\n", GetLastError());
                }
                else {
                    if (GetLastError() == ERROR_ALREADY_EXISTS) {
                        std::cout << "WARNING: CreateMutex opened an existing mutex when trying to create mutex <" << mutex_name_ << ">." << std::endl;
                    }
                    ReleaseMutex(mutex_);
                }
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to create or open shared memory <" << name_ << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
            }
        }

        MelShare::MelShare(const MelShare& other) {
            name_ = other.name_;
            bytes_ = other.bytes_;
            shm_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_, boost::interprocess::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_ = other.mutex_; // may need to use this https://msdn.microsoft.com/en-us/library/windows/desktop/ms724251(v=vs.85).aspx

        }

        MelShare& MelShare::operator=(const MelShare& other) {
            name_ = other.name_;
            bytes_ = other.bytes_;
            shm_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_, boost::interprocess::read_write, bytes_);
            region_ = boost::interprocess::mapped_region(shm_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_ = other.mutex_; // may need to use this https://msdn.microsoft.com/en-us/library/windows/desktop/ms724251(v=vs.85).aspx
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