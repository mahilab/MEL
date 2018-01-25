// MIT License
//
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

#ifndef MEL_SPINLOCK_HPP
#define MEL_SPINLOCK_HPP

#include <MEL/Utility/Lock.hpp>
#include <atomic>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Blocks concurrent access to shared resources from multiple threads
class Spinlock : public Lockable, NonCopyable {

public:

    /// Lock the Spinlock
    void lock() override;

    /// Unlock the Spinlock
    void unlock() override;

private:

    std::atomic_flag lock_ = ATOMIC_FLAG_INIT;  //< atomic flag lock

};

} // namespace mel

#endif // MEL_SPINLOCK_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
