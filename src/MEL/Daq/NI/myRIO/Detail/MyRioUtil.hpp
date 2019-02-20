#pragma once

#include <string>
#include <vector>
#include <bitset>

namespace mel {

/// Set a myRIO register bit to 1
void set_register_bit(unsigned int reg, int i);

/// Clear a myRIO register bit to 0
void clr_register_bit(unsigned int reg, int i);

/// Get a myRIO reigster bit
bool get_register_bit(unsigned int reg, int i);

/// Read a whole register into a bitset
std::bitset<8> read_register(unsigned int reg);

/// Formats NI Error
std::string get_nifpga_error_message(int error);

} // namespace mel
