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

#ifndef MEL_XBOXCONTROLLER_HPP
#define MEL_XBOXCONTROLLER_HPP

#include <memory>

namespace mel {

/// Encapsulates an Xbox 360 or Xbox One Controller
class XboxController {
public:
    /// Represents an Xbox Controller Button
    enum Button {
        A,      ///< A button
        B,      ///< B button
        X,      ///< X button
        Y,      ///< Y button
        LB,     ///< Left bumper
        RB,     ///< Right bumper
        LS,     ///< Left stick
        RS,     ///< Right stick
        Up,     ///< D-pad up
        Down,   ///< D-pad down
        Left,   ///< D-pad left
        Right,  ///< D-pad right
        Back,   ///< Back button
        Start   ///< Start button
    };

    /// Represents an Xbox Controller Axis
    enum Axis {
        LX = 0,  ///< Left stick x-axis
        LY = 1,  ///< Left stick y-axis
        LT = 2,  ///< Left trigger
        RX = 3,  ///< Right stick x-axis
        RY = 4,  ///< Right stick y-axis
        RT = 5   ///< Righter trigger
    };

public:
    /// Constructor
    XboxController(int controller_number);

    /// Destructor
    ~XboxController();

    /// Returns true if the controller is connected
    bool is_connected();

    /// Returns true if the button is pressed
    bool is_button_pressed(Button button);

    /// Returns current state of axsis in range -1.0 (or 0.0) to 1.0
    double get_axis(Axis axis);

    /// Sets the deadzone of an axis in range 0.0 to 1.0
    void set_deadzone(Axis axis, double deadzone = 0.0);

    /// Vibriates the controller
    void vibrate(double left_motor = 0.0, double right_motor = 0.0);

private:
    class Impl;                   ///< Pimpl idiom
    std::unique_ptr<Impl> impl_;  ///< OS-specific implementation
};

}  // namespace mel

#endif  // MEL_XBOXCONTROLLER_HPP
