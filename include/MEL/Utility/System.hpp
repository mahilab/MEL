// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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

#pragma once

#include <MEL/Config.hpp>
#include <MEL/Core/Time.hpp>
#include <MEL/Core/Types.hpp>
#include <string>
#include <vector>

namespace mel {

//==============================================================================
// FILESYSTEM FUNCTIONS
//==============================================================================

/// Returns directory path separator for current OS (i.e. "/" or "\\")
std::string MEL_API get_separator();

/// Creates a single folder or path of folders if it doesn't exist.
void MEL_API create_directory(const std::string& path);

/// Returns true if the path exits
bool MEL_API directory_exits(std::string path);

/// Parses a path string and returns a vector of directory strings
std::vector<std::string> MEL_API split_path(std::string path);

/// Tidies up a file or directory path per the OS filesystem
std::string tidy_path(const std::string& path, bool is_file);

/// Splits "filename.ext" into "filename" and "ext"
void MEL_API split_filename(const std::string &filename_ext,
                            std::string &filename,
                            std::string &ext);

/// Parses a filepath into its subcomponents
bool MEL_API parse_filepath(const std::string &in, 
                           std::string &directory, std::string &filename, 
                           std::string &ext, std::string &full);

//==============================================================================
// SYSTEM FUNCTIONS
//==============================================================================

/// Sleeps the thread for a given amount of time as accurately as possible
void MEL_API sleep(Time duration);

/// Gets the last operating system error
std::string MEL_API get_last_os_error();

/// Enables real-time OS priority. The program must be run 'As Administrator'.
bool MEL_API enable_realtime();

/// Disables real-time OS priority. The program must be run 'As Administrator'.
bool MEL_API disable_realtime();

/// Gets the operating system's ID number of the calling thread
uint32 MEL_API get_thread_id();

//==============================================================================
// PEROFRMANCE MONITORING FUNCTIONS
//==============================================================================

/// Gets the CPU core usage as a percent used by all processes
double MEL_API cpu_usage_total();

/// Gets the CPU core usage as a percent used by this process
double MEL_API cpu_usage_process();

/// Gets the total virtual memory available in bytes
uint64 MEL_API virt_mem_available();

/// Gets the virtual memory used by all processes in bytes
uint64 MEL_API virt_mem_used_total();

/// Gets the virtual memory used by this process in bytes
uint64 MEL_API virt_mem_used_process();

/// Gets the total RAM available in bytes
uint64 MEL_API ram_available();

/// Gets the RAM used by all process in bytes
uint64 MEL_API ram_used_total();

/// Gets the RAM used by this process in bytes
uint64 MEL_API ram_used_process();

}  // namespace mel
