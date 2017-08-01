#include "MelShare.h"
namespace mel {

    namespace share {

        MelShare::MelShare(std::string name, unsigned int bytes) : 
            name_(name),
            name_data_(name.c_str()), 
            bytes_data_(bytes),
            bytes_size_(4)
            {
            try {
                std::string name_size_temp = name + "_size";
                name_size_ = name_size_temp.c_str();
                // create a new windows shared memory 
                // (note this MelShare must remain in scope for as long as the shared memory needs to be accessed)
                shm_data_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_data_, boost::interprocess::read_write, bytes_data_);
                shm_size_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_size_, boost::interprocess::read_write, bytes_size_);

                region_data_ = boost::interprocess::mapped_region(shm_data_, boost::interprocess::read_write);
                region_size_ = boost::interprocess::mapped_region(shm_size_, boost::interprocess::read_write);

                int* size = static_cast<int*>(region_size_.get_address());
                *size = 0;

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
                std::cout << "ERROR: Failed to create or open shared memory <" << name_data_ << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
            }
        }

        MelShare::MelShare(const MelShare& other) {
            name_ = other.name_;
            name_data_ = other.name_data_;
            name_size_ = other.name_size_;
            bytes_data_ = other.bytes_data_;
            bytes_size_ = other.bytes_size_;
            shm_data_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_data_, boost::interprocess::read_write, bytes_data_);
            shm_size_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_size_, boost::interprocess::read_write, bytes_size_);
            region_data_ = boost::interprocess::mapped_region(shm_data_, boost::interprocess::read_write);
            region_size_ = boost::interprocess::mapped_region(shm_size_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_ = other.mutex_; // may need to use this https://msdn.microsoft.com/en-us/library/windows/desktop/ms724251(v=vs.85).aspx
        }

        MelShare& MelShare::operator=(const MelShare& other) {
            name_ = other.name_;
            name_data_ = other.name_data_;
            name_size_ = other.name_size_;
            bytes_data_ = other.bytes_data_;
            bytes_size_ = other.bytes_size_;
            shm_data_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_data_, boost::interprocess::read_write, bytes_data_);
            shm_size_ = boost::interprocess::windows_shared_memory(boost::interprocess::open_or_create, name_size_, boost::interprocess::read_write, bytes_size_);
            region_data_ = boost::interprocess::mapped_region(shm_data_, boost::interprocess::read_write);
            region_size_ = boost::interprocess::mapped_region(shm_size_, boost::interprocess::read_write);
            mutex_name_ = other.mutex_name_;
            mutex_ = other.mutex_; // may need to use this https://msdn.microsoft.com/en-us/library/windows/desktop/ms724251(v=vs.85).aspx
            return *this;
        }

        MelShare::~MelShare() {
            ReleaseMutex(mutex_);
            CloseHandle(mutex_);
        }

        // C Linkages for Python and C#

        int get_map_size(char* name) {
            int* empty = nullptr;
            return read_map(name, empty, 0);
        }

        int read_char_map(char* name, char* buffer, int buffer_size) {
            return read_map(name, buffer, buffer_size);
        }

        int read_int_map(char* name, int* buffer, int buffer_size) {
            return read_map(name, buffer, buffer_size);
        }

        int read_double_map(char* name, double* buffer, int buffer_size) {
            return read_map(name, buffer, buffer_size);
        }

        int write_char_map(char* name, char* buffer, int buffer_size) {
            return write_map(name, buffer, buffer_size);
        }

        int write_int_map(char* name, int* buffer, int buffer_size) {
            return write_map(name, buffer, buffer_size);
        }

        int write_double_map(char* name, double* buffer, int buffer_size) {
            return write_map(name, buffer, buffer_size);
        }

        int read_write_char_map(char* name, char* read_buffer, int read_buffer_size, char* write_buffer, int write_buffer_size) {
            return read_write_map(name, read_buffer, read_buffer_size, write_buffer, write_buffer_size);
        }

        int read_write_int_map(char* name, int* read_buffer, int read_buffer_size, int* write_buffer, int write_buffer_size) {
            return read_write_map(name, read_buffer, read_buffer_size, write_buffer, write_buffer_size);
        }

        int read_write_double_map(char* name, double* read_buffer, int read_buffer_size, double* write_buffer, int write_buffer_size) {
            return read_write_map(name, read_buffer, read_buffer_size, write_buffer, write_buffer_size);
        }

    }

}