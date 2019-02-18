#pragma once

#include <string>

namespace mel {

/// Set a myRIO register bit to 1
void set_register_bit(unsigned int reg, int i);

/// Clear a myRIO register bit to 0
void clr_register_bit(unsigned int reg, int i);

/// Get a myRIO reigster bit
bool get_register_bit(unsigned int reg, int i);

/// Formats NI Error
std::string get_nifpga_error_message(int error);

} // namespace mel
