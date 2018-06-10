/*
 * Example for performing Basic Analog Input and Output
 *
 * Copyright (c) 2015,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>

/*
 * Include the myRIO header file.
 * The target type must be defined in your project, as a stand-alone #define,
 * or when calling the compiler from the command-line.
 */
#include "MyRio.h"
#include "AIO.h"

/*
 * Define some normal macros in case they are not available. Normally, myRIO.h
 * will include the appropriate header.
 */
#if !defined(UINT16_MAX)
#define UINT16_MAX 65535
#endif

#if !defined(INT16_MAX)
#define INT16_MAX 0x7fff
#endif

#if !defined(INT16_MIN)
#define INT16_MIN (-INT16_MAX - 1)
#endif

/*
 * Declare the myRIO NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in myRIO.c.
 *
 * This removes the need to pass the myrio_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session myrio_session;


/**
 * Write a voltage value to a single channel. The channel structure must
 * previously been initialized with the appropriate scale factors with
 * AnalogScaling.
 *
 * @param[in]  channel  A struct containing the registers for the AIO
 *                      channel to be written to
 * @param[in]  value    the voltage value in volts
 */
void Aio_Write(MyRio_Aio* channel, double value)
{
    NiFpga_Status status;
    uint16_t valueScaled;

    /*
     * The value is always stored in an unsigned 16-bit register. For a signed
     * channel, cast this value directly to a signed 16-bit value. Bound the
     * values to their respective limits.
     */
    if (channel->is_signed)
    {
        /*
         * Scale the voltage value to the raw value.
         */
        value = (value - channel->scale_offset) / channel->scale_weight;
        value = (value < INT16_MIN) ? INT16_MIN : value;
        value = (value > INT16_MAX) ? INT16_MAX : value;

        /*
         * Round the scaled value to the nearest integer.
         */
        value += (value < 0.0) ? -0.5 : 0.5;

        /*
         * Convert the scaled value to an unsigned integer.
         */
        valueScaled = (uint16_t)((int16_t)(value));
    }
    else
    {
        value = (value - channel->scale_offset) / channel->scale_weight + 0.5;
        value = (value < 0) ? 0 : value;
        value = (value > UINT16_MAX) ? UINT16_MAX : value;
        valueScaled = (uint16_t) value;
    }

    /*
     * Write the value to the value register.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_WriteU16(myrio_session, channel->val, valueScaled);

    /*
     * Check if there was an error writing to the write register.
     *
     * If there was an error then print an error message to stdout and return.
     */
    MyRio_ReturnIfNotSuccess(status,
            "Could not write to the AO value registers!");

    /*
     * Write the signal so that the FPGA will apply the new value.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_WriteU16(myrio_session, channel->set, 1);

    /*
     * Check if there was an error writing to the set register.
     *
     * If there was an error then print an error message to stdout.
     */
    MyRio_ReturnIfNotSuccess(status, "Could not write to the AO set registers!")
}


/**
 * Reads a voltage value to from single channel. The channel structure must
 * previously been initialized with the appropriate scale factors with
 * AnalogScaling.
 *
 * @param[in]  channel  A struct containing the registers for the AIO
 *                      channel to be read from
 * @return the voltage value in volts
 */
double Aio_Read(MyRio_Aio* channel)
{
    NiFpga_Status status;
    uint16_t value = 0;
    double scaledValue;

    /*
     * Get the value of the value register.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_ReadU16(myrio_session, channel->val, &value);

    /*
     * Check if there was an error reading from the read register.
     *
     * If there was an error then the rest of the function cannot complete
     * correctly so print an error message to stdout and return from the
     * function early. Return 0.0 as the read input value.
     */
    MyRio_ReturnValueIfNotSuccess(status, 0.0,
            "Could not read from the AI value registers!");

    /*
     * The value is always stored in an unsigned 16-bit register. For a signed
     * channel, cast this value directly to a signed 16-bit value.
     */
    if (channel->is_signed)
    {
        scaledValue = (int16_t)value * channel->scale_weight
                + channel->scale_offset;
    }
    else
    {
        scaledValue = value * channel->scale_weight + channel->scale_offset;
    }

    return scaledValue;
}


/**
 * Reads the weight and offset constants for the channel, and change them into
 * scaled value.
 *
 * @param[in,out]  channel  A struct containing the defined constants
 */
void Aio_Scaling(MyRio_Aio* channel)
{
    /*
     * Convert from nano-volts to volts.
     */
    channel->scale_weight = channel->wght / 1000000000.0;
    channel->scale_offset = channel->ofst / 1000000000.0;
}
