#include "MelShare.h"

namespace mel {

    namespace share {

        int get_map_size(const boost::interprocess::mapped_region& region_size,
            const std::string& mutex_name)
        {
            int* size;
            Mutex mutex(mutex_name, Mutex::Mode::OPEN);
            switch (mutex.try_lock()) {
            case  Mutex::Result::NOT_OPEN:
                return -2;
            case  Mutex::Result::LOCK_ABANDONED:
                return -3;
            case  Mutex::Result::LOCK_TIMEOUT:
                return -4;
            case  Mutex::Result::LOCK_FAILED:
                return -5;
            case Mutex::Result::LOCK_ACQUIRED:
                size = static_cast<int*>(region_size.get_address());
                switch (mutex.release()) {
                case Mutex::Result::RELEASE_FAILED:
                    return -6;
                }
                return *size;
            }
        }

        int get_map_size(const std::string& name) {
            try {
                std::string name_size = name + "_size";
                std::string mutex_name = name + "_mutex";
                boost::interprocess::windows_shared_memory shm_size(boost::interprocess::open_only, name_size.c_str(), boost::interprocess::read_only);
                boost::interprocess::mapped_region region_size(shm_size, boost::interprocess::read_only);

                return get_map_size(region_size, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to open shared memory map <" << name << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
                return -1;
            }
        }

        std::string read_message(const std::string& name) {
            int message_size = get_map_size(name);
            std::vector<char> message_chars(message_size);
            read_map(name, message_chars);
            return std::string(message_chars.begin(), message_chars.end());
        }

        void write_message(const std::string& name, const std::string& message) {
            std::vector<char> message_chars(message.begin(), message.end());
            write_map(name, message_chars);
        }
        
        MelShare::MelShare(std::string name, unsigned int bytes) :
            name_(name),
            name_data_(name.c_str()),
            bytes_data_(bytes),
            bytes_size_(4),
            mutex_name_(std::string(name) + "_mutex"),
            mutex_(Mutex(mutex_name_,Mutex::Mode::CREATE))
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

    }

}