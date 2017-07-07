#include "MelShareDll.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

namespace ip = boost::interprocess;

int read_char_map(char* name, char* buffer, int size) {
    char mutex_name[32];
    strcpy(mutex_name, name);
    strcat(mutex_name, "_mutex");
    ip::named_mutex mutex(ip::open_or_create, mutex_name);
    try {
        ip::scoped_lock<ip::named_mutex> lock(mutex);
        ip::windows_shared_memory shm(ip::open_only, name, ip::read_only);
        ip::mapped_region region(shm, ip::read_only);
        volatile char* temp = static_cast<char*>(region.get_address());
        for (int i = 0; i < size; i++) {
            buffer[i] = temp[i];
        }
        return 1;
    }
    catch (ip::interprocess_exception &ex) {
        return 0;
    }
}

int read_int_map(char* name, int* buffer, int size) {
    char mutex_name[32];
    strcpy(mutex_name, name);
    strcat(mutex_name, "_mutex");
    ip::named_mutex mutex(ip::open_or_create, mutex_name);
    try {
        ip::scoped_lock<ip::named_mutex> lock(mutex);
        ip::windows_shared_memory shm(ip::open_only, name, ip::read_only);
        ip::mapped_region region(shm, ip::read_only);
        volatile int* temp = static_cast<int*>(region.get_address());
        for (int i = 0; i < size; i++) {
            buffer[i] = temp[i];
        }
        return 1;
    }
    catch (ip::interprocess_exception &ex) {
        return 0;
    }
}

int read_double_map(char* name, double* buffer, int size) {
    char mutex_name[32];
    strcpy(mutex_name, name);
    strcat(mutex_name, "_mutex");
    ip::named_mutex mutex(ip::open_or_create, mutex_name);
    try {
        ip::scoped_lock<ip::named_mutex> lock(mutex);
        ip::windows_shared_memory shm(ip::open_only, name, ip::read_only);
        ip::mapped_region region(shm, ip::read_only);
        volatile double* temp = static_cast<double*>(region.get_address());
        for (int i = 0; i < size; i++) {
            buffer[i] = temp[i];
        }
        return 1;
    }
    catch (ip::interprocess_exception &ex) {
        return 0;
    }
}

int write_char_map(char* name, char* buffer, int size) {
    char mutex_name[32];
    strcpy(mutex_name, name);
    strcat(mutex_name, "_mutex");
    ip::named_mutex mutex(ip::open_or_create, mutex_name);
    try {
        ip::scoped_lock<ip::named_mutex> lock(mutex);
        ip::windows_shared_memory shm(ip::open_only, name, ip::read_write);
        ip::mapped_region region(shm, ip::read_write);
        volatile char* temp = static_cast<char*>(region.get_address());
        for (int i = 0; i < size; i++) {
            temp[i] = buffer[i];
        }
        return 1;
    }
    catch (ip::interprocess_exception &ex) {
        return 0;
    }
}

int write_int_map(char* name, int* buffer, int size) {
    char mutex_name[32];
    strcpy(mutex_name, name);
    strcat(mutex_name, "_mutex");
    ip::named_mutex mutex(ip::open_or_create, mutex_name);
    try {
        ip::scoped_lock<ip::named_mutex> lock(mutex);
        ip::windows_shared_memory shm(ip::open_only, name, ip::read_write);
        ip::mapped_region region(shm, ip::read_write);
        volatile int* temp = static_cast<int*>(region.get_address());
        for (int i = 0; i < size; i++) {
            temp[i] = buffer[i];
        }
        return 1;
    }
    catch (ip::interprocess_exception &ex) {
        return 0;
    }
}

int write_double_map(char* name, double* buffer, int size) {
    char mutex_name[32];
    strcpy(mutex_name, name);
    strcat(mutex_name, "_mutex");
    ip::named_mutex mutex(ip::open_or_create, mutex_name);
    try {
        ip::scoped_lock<ip::named_mutex> lock(mutex);
        ip::windows_shared_memory shm(ip::open_only, name, ip::read_write);
        ip::mapped_region region(shm, ip::read_write);
        volatile double* temp = static_cast<double*>(region.get_address());
        for (int i = 0; i < size; i++) {
            temp[i] = buffer[i];
        }
        return 1;
    }
    catch (ip::interprocess_exception &ex) {
        return 0;
    }
}