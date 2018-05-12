// MIT License
//
// MEL - Mechatronics Library
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

#ifndef MEL_NAMEDMUTEX_HPP
#define MEL_NAMEDMUTEX_HPP

#include <MEL/Utility/Lock.hpp>
#include <memory>
#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Blocks concurrent access to shared resources from multiple processes
class NamedMutex : public Lockable, NonCopyable {
public:
    /// The mode by which a mutex is constructed
    enum Mode {
        OpenOrCreate,  ///< create the named mutex if it does not exit
        OpenOnly       ///< only attempt to open an existing named mutex
    };

    /// Defaut constructor
    NamedMutex(std::string name, Mode mode = OpenOrCreate);

    /// Default destructor. Releases mutex if it is currently open.
    ~NamedMutex();

    /// Waits for mutex to release and attempts to lock it
    void lock() override;

    /// Releases lock on mutex
    void unlock() override;

private:
    class Impl;                   ///< Pimpl idiom
    std::unique_ptr<Impl> impl_;  ///< OS-specific implementation
    std::string name_;            ///< Name of the mutex
};

}  // namespace mel

#endif  // MEL_NAMEDMUTEX_HPP
