#pragma once
#include <string>
#include "util.h"

namespace mel {

    class Device {

    public:

        //---------------------------------------------------------------------
        // CONSTRUCTOR / DESTRUCTOR
        //---------------------------------------------------------------------

        Device(std::string name) : name_(name), enabled_(false) {}
        virtual ~Device() { }

        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        /// This function should enable the Device and set enaled_ = true
        virtual void enable() {
            enabled_ = true;
        }
        /// This function should disable the Devie and set enabled_ = false
        virtual void disable() {
            enabled_ = false;
        }

        //---------------------------------------------------------------------
        // PUBLIC VARIABLES
        //---------------------------------------------------------------------

        std::string name_; ///< The Device name

    protected:

        //---------------------------------------------------------------------
        // PROTECTED VARIABLES
        //---------------------------------------------------------------------

        bool enabled_ = false; ///< The Device enable status

    };

}