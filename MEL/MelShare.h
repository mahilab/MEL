#pragma once
#pragma warning(disable : 4390) // disables annoying Boost warning
#include <iostream>
#include <vector>
#include <array>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#include "Mutex.h"
#include <string>

//-----------------------------------------------------------------------------
// ERROR CODES
//-----------------------------------------------------------------------------
// >0 = successful read/write, return value is current map size
//  0   map size is zero
// -1 = failed to open shared memory map
// -2 = failed to open mutex
// -3 = wait on mutex abandoned
// -4 = wait on mutex timed out
// -5 = wait on mutex failed
// -6 = failed to release mutex
//-----------------------------------------------------------------------------

// TODO: There seems to be a bug where if a class has a MelShare member, and an instance of that class
// is passed by value, then the mutex for the MelShare ceases to function properly. This issue needs to be
// understood and fixed.

namespace mel {

    namespace share {

        //---------------------------------------------------------------------
        // BASE READ/WRITE FUNCTIONS (FORM BASIS OF ALL OTHER FUNCTIONS)
        //---------------------------------------------------------------------

        /// Reads the first n = #buffer_size values from mapped region #region into C-style array #buffer. 
        /// If the size of the buffer is greater than the size of the data in the map, extra buffer indices will be ignored.
        template <typename T>
        int read_map(T* buffer, const int buffer_size,
            const boost::interprocess::mapped_region& region_data,
            const boost::interprocess::mapped_region& region_size,
            const std::string& mutex_name)
        {
            int* size;
            volatile T* data;
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
                if (buffer_size > 0) {
                    data = static_cast<T*>(region_data.get_address());
                    for (int i = 0; i < std::min(buffer_size, *size); i++) {
                        buffer[i] = data[i];
                    }
                }
                switch (mutex.release()) {
                case Mutex::Result::RELEASE_FAILED:
                    return -6;
                }
                return *size;
            }  
            return -5;
        }

        /// Writes n = #buffer_size values into mapped region #region from #buffer.
        template <typename T>
        int write_map(T* buffer, const int buffer_size,
            const boost::interprocess::mapped_region& region_data,
            const boost::interprocess::mapped_region& region_size,
            const std::string& mutex_name)
        {
            int* size;
            volatile T* data;
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
            case  Mutex::Result::LOCK_ACQUIRED:
                size = static_cast<int*>(region_size.get_address());
                if (buffer_size > 0) {
                    data = static_cast<T*>(region_data.get_address());
                    *size = buffer_size;
                    for (int i = 0; i < buffer_size; i++) {
                        data[i] = buffer[i];
                    }
                }
                switch (mutex.release()) {
                case Mutex::Result::RELEASE_FAILED:
                    return -6;
                }
                return *size;
            }
            return -5;
        }

        /// Returns the current size of the data stored in the map
        int get_map_size(const boost::interprocess::mapped_region& region_size, const std::string& mutex_name);

        //---------------------------------------------------------------------
        // BASIC FUNCTIONS
        //---------------------------------------------------------------------

        /// Returns the current size of the data stored in the map
        int get_map_size(const std::string& name);

        /// Returns a string message from the map by reading an underlying char array
        std::string read_message(const std::string& name);

        /// Writes a string message to the map as a char array
        void write_message(const std::string& name, const std::string& message);

        //---------------------------------------------------------------------
        // TEMPLATED READ/WRITE FUNCTIONS THAT REOPEN AN EXISTING MELSHARE MAP
        //---------------------------------------------------------------------

        /// Opens shared memory map #name and reads n = #buffer_size values into C-style array #buffer
        template <typename T>
        int read_map(const std::string& name, T* buffer, const int buffer_size) {
            try {
                std::string name_size = name + "_size";
                std::string mutex_name = name + "_mutex";
                boost::interprocess::windows_shared_memory shm_data(boost::interprocess::open_only, name.c_str(), boost::interprocess::read_only);
                boost::interprocess::windows_shared_memory shm_size(boost::interprocess::open_only, name_size.c_str(), boost::interprocess::read_only);
                boost::interprocess::mapped_region region_data(shm_data, boost::interprocess::read_only);
                boost::interprocess::mapped_region region_size(shm_size, boost::interprocess::read_only);

                return read_map(buffer, buffer_size, region_data, region_size, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to open shared memory map <" << name << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
                return -1;
            }
        }

        /// Opens shared memory map #name and writes n = #buffer_size values from C-style array #buffer
        template <typename T>
        int write_map(const std::string& name, T* buffer, const int buffer_size) {
            try {
                std::string name_size = name + "_size";
                std::string mutex_name = name + "_mutex";
                boost::interprocess::windows_shared_memory shm_data(boost::interprocess::open_only, name.c_str(), boost::interprocess::read_write);
                boost::interprocess::windows_shared_memory shm_size(boost::interprocess::open_only, name_size.c_str(), boost::interprocess::read_write);
                boost::interprocess::mapped_region region_data(shm_data, boost::interprocess::read_write);
                boost::interprocess::mapped_region region_size(shm_size, boost::interprocess::read_write);
                return write_map(buffer, buffer_size, region_data, region_size, mutex_name);
            }
            catch (boost::interprocess::interprocess_exception &ex) {
                std::cout << "ERROR: Failed to open shared memory map <" << name << ">." << std::endl;
                std::cout << "BOOST ERROR: " << ex.what() << std::endl;
                return -1;
            }
        }


        //---------------------------------------------------------------------
        // TEMPLATED READ/WRITE FUNCTIONS FOR STL CONTAINERS
        //---------------------------------------------------------------------

        /// Opens shared memory map #name and reads n = buffer.size() values into STL vector #buffer
        template <typename T>
        int read_map(const std::string& name, std::vector<T>& buffer) {
            return read_map(name, &buffer[0], static_cast<int>(buffer.size()));
        }

        /// Opens shared memory map #name and reads n = buffer.size() values into STL array #buffer
        template <typename T, std::size_t N>
        int read_map(const std::string& name, std::array<T, N>& buffer) {
            return read_map(name, &buffer[0], static_cast<int>(buffer.size()));
        }

        /// Opens shared memory map #name and writes n = buffer.size() values from STL vector #buffer
        template <typename T>
        int write_map(const std::string& name, std::vector<T>& buffer) {
            return write_map(name, &buffer[0], static_cast<int>(buffer.size()));
        }

        /// Opens shared memory map #name and writes n = buffer.size() values from STL array #buffer
        template <typename T, std::size_t N>
        int write_map(const std::string& name, std::array<T, N>& buffer) {
            return write_map(name, &buffer[0], static_cast<int>(buffer.size()));
        }

        //------------------------------------------------------------------
        // MELSHARE CLASS WRAPS READ/WRITE INTO THE C++ INTERFACE
        //------------------------------------------------------------------

        class MelShare {

        public:

            MelShare(std::string name, unsigned int bytes = 256);
            MelShare(const MelShare& other);
            MelShare& operator=(const MelShare& other);

            /// Reads n = #buffer_size values into C-style array #buffer
            template<typename T>
            int read(T* buffer, int buffer_size) {
                return read_map(buffer, buffer_size, region_data_, region_size_, mutex_name_);
            }

            /// Reads single value into buffer
            template<typename T>
            int read(T& buffer) {
                return read(&buffer, 1);
            }

            /// Reads n = buffer.size() values into STL vector #buffer
            template<typename T>
            int read(std::vector<T>& buffer) {
                return read(&buffer[0], static_cast<int>(buffer.size()));
            }

            /// Reads n = buffer.size() values into STL array #buffer
            template <typename T, std::size_t N>
            int read(std::array<T, N>& buffer) {
                return read(&buffer[0], static_cast<int>(buffer.size()));
            }

            /// Writes n = #buffer_size values from C-style array #buffer
            template<typename T>
            int write(T* buffer, int buffer_size) {
                return write_map(buffer, buffer_size, region_data_, region_size_, mutex_name_);
            }

            /// Writes single value from buffer
            template<typename T>
            int write(T& buffer) {
                return write(&buffer, 1);
            }

            /// Writes n = buffer.size() values from STL vector #buffer
            template<typename T>
            int write(std::vector<T>& buffer) {
                return write(&buffer[0], static_cast<int>(buffer.size()));
            }

            /// Writes n = buffer.size() values from STL array #buffer
            template <typename T, std::size_t N>
            int write(std::array<T, N>& buffer) {
                return write(&buffer[0], static_cast<int>(buffer.size()));
            }

            /// Gets the current number of elements in the MELShare
            int get_size() {
                return  get_map_size(region_size_, mutex_name_);
            }

            std::string read_message() {
                int message_size = get_size();
                std::vector<char> message_chars(message_size);
                read(message_chars);
                return std::string(message_chars.begin(), message_chars.end());
            }

            void write_message(const std::string& message) {
                std::vector<char> message_chars(message.begin(), message.end());
                write(message_chars);
            }

            std::string name_;

        private:

            const char* name_data_;
            const char* name_size_;
            unsigned int bytes_data_;
            unsigned int bytes_size_;
            boost::interprocess::windows_shared_memory shm_data_;
            boost::interprocess::windows_shared_memory shm_size_;
            boost::interprocess::mapped_region region_data_;
            boost::interprocess::mapped_region region_size_;
            std::string mutex_name_;
            Mutex mutex_;
        };

 
    }

}