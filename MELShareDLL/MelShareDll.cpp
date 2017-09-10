#include "MelShareDll.h"

int get_map_size(char* name) {
    return mel::comm::get_map_size(name);
}

int read_char_map(char* name, char* buffer, int buffer_size) {
    return mel::comm::read_map(name, buffer, buffer_size);
}

int read_int_map(char* name, int* buffer, int buffer_size) {
    return mel::comm::read_map(name, buffer, buffer_size);
}

int read_float_map(char* name, float* buffer, int buffer_size) {
    return mel::comm::read_map(name, buffer, buffer_size);
}

int read_double_map(char* name, double* buffer, int buffer_size) {
    return mel::comm::read_map(name, buffer, buffer_size);
}

int write_char_map(char* name, char* buffer, int buffer_size) {
    return mel::comm::write_map(name, buffer, buffer_size);
}

int write_int_map(char* name, int* buffer, int buffer_size) {
    return mel::comm::write_map(name, buffer, buffer_size);
}

int write_float_map(char* name, float* buffer, int buffer_size) {
    return mel::comm::write_map(name, buffer, buffer_size);
}

int write_double_map(char* name, double* buffer, int buffer_size) {
    return mel::comm::write_map(name, buffer, buffer_size);
}