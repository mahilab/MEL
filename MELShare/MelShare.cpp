#include "MelShare.h"
namespace mel {

    namespace share {

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

        int read_float_map(char* name, float* buffer, int buffer_size) {
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

        int write_float_map(char* name, float* buffer, int buffer_size) {
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

        int read_write_float_map(char* name, float* read_buffer, int read_buffer_size, float* write_buffer, int write_buffer_size) {
            return read_write_map(name, read_buffer, read_buffer_size, write_buffer, write_buffer_size);
        }

        int read_write_double_map(char* name, double* read_buffer, int read_buffer_size, double* write_buffer, int write_buffer_size) {
            return read_write_map(name, read_buffer, read_buffer_size, write_buffer, write_buffer_size);
        }

    }

}