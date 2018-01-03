#pragma once

namespace mel {

//==============================================================================
// GENERIC
//==============================================================================

extern const double PI;          ///< pi
extern const double E;           ///< e
extern const double EPS;         ///< smallest double such that 1.0 + EPS != 1.0
extern const double INF;         ///< positive infinity (negate for negative infinity)
extern const double NaN;         ///< not-a-number

//==============================================================================
// CONVERSIONS
//==============================================================================

extern const double DEG2RAD;     ///< multiply degrees by this value to convert to radians
extern const double RAD2DEG ;    ///< multiply radians by this value to convert to degrees
extern const double INCH2METER;  ///< multiply inches by this value to convert to meters
extern const double METER2INCH;  ///< multiply meters by this value to convert to inches

} /// namespace mel
