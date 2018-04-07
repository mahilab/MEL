#ifndef MEL_TIMESTAMP_HPP
#define MEL_TIMESTAMP_HPP

#include <string>

namespace mel {

/// Encapsulates a timestamp
class Timestamp {
public:
    /// Default constructor
    Timestamp();

    /// Returns timestamp string as "yyyy-mm-dd"
    std::string yyyy_mm_dd() const;

    /// Returns timestamp as "hh:mm:ss.mmm"
    std::string hh_mm_ss_mmm() const;

    /// Returns timestamp as "yyyy-mm-dd_hh:mm:ss"
    std::string yyyy_mm_dd_hh_mm_ss() const;

    /// Returns timestamp as "yyyy-mm-dd_hh:mm:ss.mmm"
    std::string yyyy_mm_dd_hh_mm_ss_mmm() const;

public:
    int year;      ///< year
    int month;     ///< month                    [1-12]
    int yday;      ///< day of year              [1-366]
    int mday;      ///< day of month             [1-31]
    int wday;      ///< day of week (Sunday = 1) [1-7]
    int hour;      ///< hour                     [0-23]
    int min;       ///< minute                   [0-59]
    int sec;       ///< second                   [0-59]
    int millisec;  ///< millisecond              [0-999]
};

}  // namespace mel

#endif  // MEL_TIMESTAMP_HPP
