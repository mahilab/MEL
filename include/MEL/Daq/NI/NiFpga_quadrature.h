/*
 * Generated with the FPGA Interface C API Generator 17.0.0
 * for NI-RIO 17.0.0 or later.
 */

#ifndef __NiFpga_quadrature_h__
#define __NiFpga_quadrature_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1700
#endif

#include <MEL/Daq/NI/NiFpga.h>

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_quadrature_Bitfile;
 */
#define NiFpga_quadrature_Bitfile "NiFpga_quadrature.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_quadrature_Signature = "A2D7D2DC3720C12E8CB0173281A75096";

typedef enum
{
   NiFpga_quadrature_IndicatorI32_count = 0x1800C,
   NiFpga_quadrature_IndicatorI32_index_save = 0x18000,
} NiFpga_quadrature_IndicatorI32;

typedef enum
{
   NiFpga_quadrature_ControlBool_direction = 0x18016,
   NiFpga_quadrature_ControlBool_disable = 0x1800A,
   NiFpga_quadrature_ControlBool_reset = 0x18012,
   NiFpga_quadrature_ControlBool_timing_A = 0x1801A,
   NiFpga_quadrature_ControlBool_timing_B = 0x1801E,
   NiFpga_quadrature_ControlBool_timing_Z = 0x18022,
} NiFpga_quadrature_ControlBool;

typedef enum
{
   NiFpga_quadrature_ControlI32_reset_value = 0x18004,
} NiFpga_quadrature_ControlI32;

#endif
