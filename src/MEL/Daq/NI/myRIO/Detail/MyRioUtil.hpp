#pragma once


namespace mel {

/// Set a myRIO register bit to 1
void set_register_bit(unsigned int reg, int i);

/// Clear a myRIO register bit to 0
void clr_register_bit(unsigned int reg, int i);

/// Get a myRIO reigster bit
bool get_register_bit(unsigned int reg, int i);

} // namespace mel
