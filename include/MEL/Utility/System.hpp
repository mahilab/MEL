// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_SYSTEM_HPP
#define MEL_SYSTEM_HPP

#include <MEL/Utility/Time.hpp>
#include <MEL/Utility/Types.hpp>
#include <string>
#include <vector>

namespace mel {

//==============================================================================
// DATE FUNCTIONS
//==============================================================================

/// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string get_ymdhms();

//==============================================================================
// DIRECTORY FUNCTIONS
//==============================================================================

/// Returns directory path divider for current operating system
std::string get_path_slash();

/// Parses a path string and returns a vector of directory strings
std::vector<std::string> parse_path(std::string path);

/// Creates a single folder or path of folders if it doesn't exist.
void create_directory(std::string path);

//==============================================================================
// SYSTEM FUNCTIONS
//==============================================================================

/// Sleeps the thread for a given amount of time as accurately as possible
void sleep(Time duration);

/// Gets the last operating system error
std::string get_last_os_error();

/// Enables real-time OS priority. The program must be run 'As Administrator'.
bool enable_realtime();

///  Disables real-time OS priority. The program must be run 'As Administrator'.
bool disable_realtime();

//==============================================================================
// PEROFRMANCE MONITORING FUNCTIONS
//==============================================================================

/// Gets the CPU core usage as a percent used by all processes
double cpu_usage_total();

/// Gets the CPU core usage as a percent used by this process
double cpu_usage_process();

/// Gets the total virtual memory available in bytes
uint64 virt_mem_available();

/// Gets the virtual memory used by all processes in bytes
uint64 virt_mem_used_total();

/// Gets the virtual memory used by this process in bytes
uint64 virt_mem_used_process();

/// Gets the total RAM available in bytes
uint64 ram_available();

/// Gets the RAM used by all process in bytes
uint64 ram_used_total();

/// Gets the RAM used by this process in bytes
uint64 ram_used_process();

} // namespace mel

#endif // MEL_SYSTEM_HPP
