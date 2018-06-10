/*
 * AIO.h
 *
 * Copyright (c) 2015,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef AIO_h_
#define AIO_h_

#include "MyRio.h"

#if NiFpga_Cpp
extern "C" {
#endif


/**
 * Registers and settings for a particular analog IO.
 * Analog input and analog output share the same structure, but it is
 * not possible to read from an analog output and write to an analog
 * input.
 */
typedef struct
{
    uint32_t val;           /**< AIO value register */
    uint32_t wght;          /**< AIO weight constant value */
    uint32_t ofst;          /**< AIO offset constant value */
    uint32_t set;           /**< AO set register, unused for AI */
    NiFpga_Bool is_signed;  /**< Is the AIO signed */
    double scale_weight;    /**< AIO weight scaled value */
    double scale_offset;    /**< AIO offset scaled value */
} MyRio_Aio;


/**
 * Registers for the analog subsystem.
 */
typedef struct
{
    uint32_t ai_rdy;        /**< Analog input ready register */
} MyRio_Aio_Rdy;


/**
 * Write a voltage value to a single channel.
 */
void Aio_Write(MyRio_Aio* channel, double value);


/**
 * Read the value of a single channel.
 */
double Aio_Read(MyRio_Aio* channel);


/**
 * Get the scaling weight and offset for a single channel.
 */
void Aio_Scaling(MyRio_Aio* channel);

#if NiFpga_Cpp
}
#endif

#endif /* AIO_h_ */
