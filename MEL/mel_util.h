#pragma once
#include <vector>
#include <array>
#include <iostream>
#include <string>

namespace mel {

    //-------------------------------------------------------------------------
    // TYPEDEF BASIC TYPES
    //-------------------------------------------------------------------------

    typedef             char  int8;   ///<           -127 to +127
    typedef unsigned    char uint8;   ///<              0 to +255
    typedef            short  int16;  ///<        -32,767 to +32,767
    typedef unsigned   short uint16;  ///<              0 to +65,535
    typedef              int  int32;  ///< -2,147,483,648 to +2,147,483,647
    typedef unsigned     int uint32;  ///<              0 to +4,294,967,295
    typedef signed   __int64 int64;   ///<
    typedef unsigned __int64 uint64;  ///<

    typedef std::vector<char>   char_vec;
    typedef std::vector<int8>   int8_vec;
    typedef std::vector<uint8>  uint8_vec;
    typedef std::vector<int16>  int16_vec;
    typedef std::vector<uint16> uint16_vec;
    typedef std::vector<int32>  int32_vec;
    typedef std::vector<uint32> uint32_vec;
    typedef std::vector<double> double_vec;

    //-------------------------------------------------------------------------
    // TYPDEF MEL TYPES BASED ON DAQ MANUFACTUERER
    //-------------------------------------------------------------------------

#define QUANSER
#ifdef QUANSER

    typedef double voltage;
    typedef int8   dsignal;
    typedef uint32 channel;

    typedef std::vector<voltage> voltage_vec;
    typedef std::vector<dsignal> dsignal_vec;
    typedef std::vector<channel> channel_vec;

#endif

#ifdef NI
#endif

    namespace util {

        //-------------------------------------------------------------------------
        // UTILITY FUNCTIONS
        //-------------------------------------------------------------------------

        const std::string get_ymdhms();  ///< Get current date/time, format is YYYY-MM-DD.HH:mm:ss
        std::string get_last_windows_error_message();

        /// Enables soft realtime scheduling performance. The program must be run 'As Administrator'
        /// to obtain the REALTIME priority policy. Otherwise, it will default to HIGH priority policy, 
        /// which is still higher than typical Windows applications (NORMAL in most cases).
        void enable_realtime();

        //-------------------------------------------------------------------------
        // PRINTING AND FORMATTING FUNCTIONS
        //-------------------------------------------------------------------------

        /// Prints anything that works with std::cout. 
        /// end_line = true will print a new line after printing value
        template <typename T>
        void print(T value, bool end_line = true) {
            std::cout << value;
            if (end_line)
                std::cout << std::endl;
        }

        /// Prints STL vector
        template <typename T>
        void print(std::vector<T> vector, bool end_line = true) {
            for (auto it = vector.begin(); it != vector.end(); ++it) {
                std::cout << *it << " ";
            }
            if (end_line)
                std::cout << std::endl;
        }

        /// Prints STL array
        template <typename T, size_t N>
        void print(std::array<T, N> array, bool end_line = true) {
            for (auto it = array.begin(); it != array.end(); ++it) {
                std::cout << *it << " ";
            }
            if (end_line)
                std::cout << std::endl;
        }

        /// Prints C-style array
        template <typename T>
        void print(T* array, size_t size, bool end_line = true) {
            for (int i = 0; i < size; i++) {
                std::cout << array[i] << " ";
            }
            if (end_line)
                std::cout << std::endl;
        }

        /// Prints MEL 2D array
        /*template <typename T, std::size_t N, std::size_t M>
        void print(array_2D<T,N,M> array, bool end_line = true) {
            for (auto it_row = array_.begin(); it_row != array_.end(); ++it_row) {
                for (auto it_col = (*it_row).begin(); it_col != (*it_row).end(); ++it_col) {
                    std::cout << *it_col << " ";
                }
                if (end_line)
                    std::cout << std::endl;
            }
        }*/

        /// Formats string name in between angle brackets for MEL printing puproses
        std::string namify(std::string name);

        //-------------------------------------------------------------------------
        // USEFUL CLASSES & TYPES
        //-------------------------------------------------------------------------

        /// Template 2D array for storing 2D data arrays and doing simple arithmetic
        template <typename T, std::size_t N, std::size_t M>
        struct array_2D {
        public:
            array_2D() {};
            array_2D(T val) {
                for (auto it_row = array_.begin(); it_row != array_.end(); ++it_row) {
                    for (auto it_col = (*it_row).begin(); it_col != (*it_row).end(); ++it_col) {
                        *it_col = val;
                    }
                }
            };
            std::array<T, M> operator [](int i) const { return array_[i]; }
            std::array<T, M>& operator [](int i) { return array_[i]; }
            std::array<T, N> multiply(std::array<T, M> b) {
                std::array<T, N> c;
                c.fill(0);
                for (auto it_row = array_.begin(); it_row != array_.end(); ++it_row) {
                    for (auto it_col = (*it_row).begin(); it_col != (*it_row).end(); ++it_col) {
                        c[it_row - array_.begin()] += *it_col * b[it_col - (*it_row).begin()];
                    }
                }
                return c;
            };
        private:
            std::array<std::array<T, M>, N> array_;
        };

    }
}