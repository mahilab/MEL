// MIT License
//
// MEL - Mechatronics Engine & Library
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

#ifndef MEL_LOGGER_HPP
#define MEL_LOGGER_HPP

#include <MEL/Logging/Writers/Writer.hpp>
#include <MEL/Utility/Singleton.hpp>
#include <vector>

#ifndef DEFAULT_MEL_LOGGER
#define DEFAULT_LOGGER 0
#endif

namespace mel {
template <int instance>
class Logger : public Singleton<Logger<instance> >, public Writer {
public:
    /// Constructs a new Logger instance with a max severity
    Logger(Severity maxSeverity = None) : max_severity_(maxSeverity) {}

    /// Adds a write to the Logger
    Logger& add_writer(Writer* writer) {
        assert(writer != this);
        writers_.push_back(writer);
        return *this;
    }

    /// Returns the max severity of the Logger
    Severity get_max_severity() const { return max_severity_; }

    /// Sets the max severity of the Logger
    void set_max_severity(Severity severity) { max_severity_ = severity; }

    /// Checks a severity against the max severity and returns true if is less
    /// or equally severe
    bool check_severity(Severity severity) const {
        return severity <= max_severity_;
    }

    /// Writes a Record to the Logger
    virtual void write(const Record& record) {
        if (check_severity(record.get_severity())) {
            *this += record;
        }
    }

    void operator+=(const Record& record) {
        for (std::vector<Writer*>::iterator it = writers_.begin();
             it != writers_.end(); ++it) {
            if ((*it)->check_severity(record.get_severity()))
                (*it)->write(record);
        }
    }

private:
    Severity max_severity_;         ///< max severity
    std::vector<Writer*> writers_;  ///< writers for this Logger
};

template <int instance>
inline Logger<instance>* get_logger() {
    return Logger<instance>::get_instance();
}

inline Logger<DEFAULT_LOGGER>* get_logger() {
    return Logger<DEFAULT_LOGGER>::get_instance();
}
}  // namespace mel

#endif  // MEL_LOGGER_HPP
