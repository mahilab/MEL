#pragma once
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include "mel_types.h"

namespace mel {

    namespace util {

        //-------------------------------------------------------------------------
        // UTILITY FUNCTIONS
        //-------------------------------------------------------------------------

        const std::string get_ymdhms();  ///< Get current date/time, format is YYYY-MM-DD.HH:mm:ss
        std::string get_last_windows_error_message();

        /// Enables soft realtime performance. The program must be run 'As Administrator'
        /// to obtain the REALTIME priority policy. Otherwise, it will default to HIGH priority policy, 
        /// which is still higher than typical Windows applications (NORMAL in most cases).
        void enable_realtime();
        ///  Disables soft realtime performance.
        void disable_realtime();

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