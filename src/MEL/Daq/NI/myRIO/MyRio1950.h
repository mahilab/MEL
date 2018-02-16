/*
 * myRIO 1950 Target Header File
 *
 * Definitions for myRIO 1950
 * This file should be included from "MyRio.h", never directly
 *
 * Copyright (c) 2013,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef MyRio1950_h_
#define MyRio1950_h_

/* Guard against direct include */
#ifndef MyRio_h_
#    error "Include MyRio.h instead of MyRio1950.h"
#endif


/* Guard against including multiple myRIO target header files. */
#ifndef MyRioXXXX
#   define MyRioXXXX "MyRio1950.h"
#else
#    error "Attempt to include more that one MyRioXXXX.h file."
#endif


/*
 * Include NiFpga header files
 */
#include "NiFpga.h"
#include "NiFpga_MyRio1950Fpga30.h"


/*
 * Set the Bitfile and the Signtature
 */
#define MyRio_Bitfile	NiFpga_MyRio1950Fpga30_Bitfile
#define MyRio_Signature NiFpga_MyRio1950Fpga30_Signature


/*
 * Define the weight and offset constants of connector A&B
 */
#if !defined(AIA_0WGHT)
#define AIA_0WGHT  1220703
#endif

#if !defined(AIA_0OFST)
#define AIA_0OFST  0
#endif

#if !defined(AIA_0SIGN)
#define AIA_0SIGN  0
#endif

#if !defined(AIA_1WGHT)
#define AIA_1WGHT  1220703
#endif

#if !defined(AIA_1OFST)
#define AIA_1OFST  0
#endif

#if !defined(AIA_1SIGN)
#define AIA_1SIGN  0
#endif

#if !defined(AIA_2WGHT)
#define AIA_2WGHT  1220703
#endif

#if !defined(AIA_2OFST)
#define AIA_2OFST  0
#endif

#if !defined(AIA_2SIGN)
#define AIA_2SIGN  0
#endif

#if !defined(AIA_3WGHT)
#define AIA_3WGHT  1220703
#endif

#if !defined(AIA_3OFST)
#define AIA_3OFST  0
#endif

#if !defined(AIA_3SIGN)
#define AIA_3SIGN  0
#endif

#if !defined(AIB_0WGHT)
#define AIB_0WGHT  1220703
#endif

#if !defined(AIB_0OFST)
#define AIB_0OFST  0
#endif

#if !defined(AIB_0SIGN)
#define AIB_0SIGN  0
#endif

#if !defined(AIB_1WGHT)
#define AIB_1WGHT  1220703
#endif

#if !defined(AIB_1OFST)
#define AIB_1OFST  0
#endif

#if !defined(AIB_1SIGN)
#define AIB_1SIGN  0
#endif

#if !defined(AIB_2WGHT)
#define AIB_2WGHT  1220703
#endif

#if !defined(AIB_2OFST)
#define AIB_2OFST  0
#endif

#if !defined(AIB_2SIGN)
#define AIB_2SIGN  0
#endif

#if !defined(AIB_3WGHT)
#define AIB_3WGHT  1220703
#endif

#if !defined(AIB_3OFST)
#define AIB_3OFST  0
#endif

#if !defined(AIB_3SIGN)
#define AIB_3SIGN  0
#endif

#if !defined(AOA_0WGHT)
#define AOA_0WGHT  1220703
#endif

#if !defined(AOA_0OFST)
#define AOA_0OFST  0
#endif

#if !defined(AOA_0SIGN)
#define AOA_0SIGN  0
#endif

#if !defined(AOA_1WGHT)
#define AOA_1WGHT  1220703
#endif

#if !defined(AOA_1OFST)
#define AOA_1OFST  0
#endif

#if !defined(AOA_1SIGN)
#define AOA_1SIGN  0
#endif

#if !defined(AOB_0WGHT)
#define AOB_0WGHT  1220703
#endif

#if !defined(AOB_0OFST)
#define AOB_0OFST  0
#endif

#if !defined(AOB_0SIGN)
#define AOB_0SIGN  0
#endif

#if !defined(AOB_1WGHT)
#define AOB_1WGHT  1220703
#endif

#if !defined(AOB_1OFST)
#define AOB_1OFST  0
#endif

#if !defined(AOB_1SIGN)
#define AOB_1SIGN  0
#endif

/*
 * Define the accelerometer weight constant.
 */
#if !defined(ACCSCALEWGHT_CST)
#define ACCSCALEWGHT_CST  256
#endif

/*
 * Define the IRQ number range
 */
#if !defined(IRQNO_MAX)
#define IRQNO_MAX  8
#endif

#if !defined(IRQNO_MIN)
#define IRQNO_MIN  1
#endif


/**
 * Redefinition of the NiFpga Bool Indicator enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadBool and NiFpga_WriteBool functions should be used to access these
 * items.
 */
typedef enum
{
    AOSYSSTAT = NiFpga_MyRio1950Fpga30_IndicatorBool_AOSYSSTAT,
    SYSACCRDY = NiFpga_MyRio1950Fpga30_IndicatorBool_SYSACCRDY,
    SYSAIRDY = NiFpga_MyRio1950Fpga30_IndicatorBool_SYSAIRDY,
    SYSRDY = NiFpga_MyRio1950Fpga30_IndicatorBool_SYSRDY,
} MyRio1950Fpga30_IndicatorBool;


/**
 * Redefinition of the NiFpga U8 Indicator enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadU8 and NiFpga_WriteU8 functions should be used to access these
 * items.
 */
typedef enum
{
    DIBTN = NiFpga_MyRio1950Fpga30_IndicatorU8_DIBTN,
    DIOA_158IN = NiFpga_MyRio1950Fpga30_IndicatorU8_DIOA_158IN,
    DIOA_70IN = NiFpga_MyRio1950Fpga30_IndicatorU8_DIOA_70IN,
    DIOB_158IN = NiFpga_MyRio1950Fpga30_IndicatorU8_DIOB_158IN,
    DIOB_70IN = NiFpga_MyRio1950Fpga30_IndicatorU8_DIOB_70IN,
    ENCASTAT = NiFpga_MyRio1950Fpga30_IndicatorU8_ENCASTAT,
    ENCBSTAT = NiFpga_MyRio1950Fpga30_IndicatorU8_ENCBSTAT,
    I2CADATI = NiFpga_MyRio1950Fpga30_IndicatorU8_I2CADATI,
    I2CASTAT = NiFpga_MyRio1950Fpga30_IndicatorU8_I2CASTAT,
    I2CBDATI = NiFpga_MyRio1950Fpga30_IndicatorU8_I2CBDATI,
    I2CBSTAT = NiFpga_MyRio1950Fpga30_IndicatorU8_I2CBSTAT,
    SPIASTAT = NiFpga_MyRio1950Fpga30_IndicatorU8_SPIASTAT,
    SPIBSTAT = NiFpga_MyRio1950Fpga30_IndicatorU8_SPIBSTAT,
} MyRio1950Fpga30_IndicatorU8;


/**
 * Redefinition of the NiFpga U16 Indicator enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadU16 and NiFpga_WriteU16 functions should be used to access these
 * items.
 */
typedef enum
{
    ACCXVAL = NiFpga_MyRio1950Fpga30_IndicatorU16_ACCXVAL,
    ACCYVAL = NiFpga_MyRio1950Fpga30_IndicatorU16_ACCYVAL,
    ACCZVAL = NiFpga_MyRio1950Fpga30_IndicatorU16_ACCZVAL,
    AIA_0VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIA_0VAL,
    AIA_1VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIA_1VAL,
    AIA_2VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIA_2VAL,
    AIA_3VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIA_3VAL,
    AIB_0VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIB_0VAL,
    AIB_1VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIB_1VAL,
    AIB_2VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIB_2VAL,
    AIB_3VAL = NiFpga_MyRio1950Fpga30_IndicatorU16_AIB_3VAL,
    PWMA_0CNTR = NiFpga_MyRio1950Fpga30_IndicatorU16_PWMA_0CNTR,
    PWMA_1CNTR = NiFpga_MyRio1950Fpga30_IndicatorU16_PWMA_1CNTR,
    PWMA_2CNTR = NiFpga_MyRio1950Fpga30_IndicatorU16_PWMA_2CNTR,
    PWMB_0CNTR = NiFpga_MyRio1950Fpga30_IndicatorU16_PWMB_0CNTR,
    PWMB_1CNTR = NiFpga_MyRio1950Fpga30_IndicatorU16_PWMB_1CNTR,
    PWMB_2CNTR = NiFpga_MyRio1950Fpga30_IndicatorU16_PWMB_2CNTR,
    SPIADATI = NiFpga_MyRio1950Fpga30_IndicatorU16_SPIADATI,
    SPIBDATI = NiFpga_MyRio1950Fpga30_IndicatorU16_SPIBDATI,
} MyRio1950Fpga30_IndicatorU16;


/**
 * Redefinition of the NiFpga U32 Indicator enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadU32 and NiFpga_WriteU32 functions should be used to access these
 * items.
 */
typedef enum
{
    ENCACNTR = NiFpga_MyRio1950Fpga30_IndicatorU32_ENCACNTR,
    ENCBCNTR = NiFpga_MyRio1950Fpga30_IndicatorU32_ENCBCNTR,
    IRQTIMERREAD = NiFpga_MyRio1950Fpga30_IndicatorU32_IRQTIMERREAD,
} MyRio1950Fpga30_IndicatorU32;


/**
 * Redefinition of the NiFpga Bool Control enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadBool and NiFpga_WriteBool functions should be used to access these
 * items.
 */
typedef enum
{
    AOSYSGO = NiFpga_MyRio1950Fpga30_ControlBool_AOSYSGO,
    I2CAGO = NiFpga_MyRio1950Fpga30_ControlBool_I2CAGO,
    I2CBGO = NiFpga_MyRio1950Fpga30_ControlBool_I2CBGO,
    IRQDI_BTNENA = NiFpga_MyRio1950Fpga30_ControlBool_IRQDI_BTNENA,
    IRQDI_BTNFALL = NiFpga_MyRio1950Fpga30_ControlBool_IRQDI_BTNFALL,
    IRQDI_BTNRISE = NiFpga_MyRio1950Fpga30_ControlBool_IRQDI_BTNRISE,
    IRQTIMERSETTIME = NiFpga_MyRio1950Fpga30_ControlBool_IRQTIMERSETTIME,
    SPIAGO = NiFpga_MyRio1950Fpga30_ControlBool_SPIAGO,
    SPIBGO = NiFpga_MyRio1950Fpga30_ControlBool_SPIBGO,
} MyRio1950Fpga30_ControlBool;


/**
 * Redefinition of the NiFpga U8 Control enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadU8 and NiFpga_WriteU8 functions should be used to access these
 * items.
 */
typedef enum
{
    DIOA_158DIR = NiFpga_MyRio1950Fpga30_ControlU8_DIOA_158DIR,
    DIOA_158OUT = NiFpga_MyRio1950Fpga30_ControlU8_DIOA_158OUT,
    DIOA_70DIR = NiFpga_MyRio1950Fpga30_ControlU8_DIOA_70DIR,
    DIOA_70OUT = NiFpga_MyRio1950Fpga30_ControlU8_DIOA_70OUT,
    DIOB_158DIR = NiFpga_MyRio1950Fpga30_ControlU8_DIOB_158DIR,
    DIOB_158OUT = NiFpga_MyRio1950Fpga30_ControlU8_DIOB_158OUT,
    DIOB_70DIR = NiFpga_MyRio1950Fpga30_ControlU8_DIOB_70DIR,
    DIOB_70OUT = NiFpga_MyRio1950Fpga30_ControlU8_DIOB_70OUT,
    DOLED30 = NiFpga_MyRio1950Fpga30_ControlU8_DOLED30,
    ENCACNFG = NiFpga_MyRio1950Fpga30_ControlU8_ENCACNFG,
    ENCBCNFG = NiFpga_MyRio1950Fpga30_ControlU8_ENCBCNFG,
    I2CAADDR = NiFpga_MyRio1950Fpga30_ControlU8_I2CAADDR,
    I2CACNFG = NiFpga_MyRio1950Fpga30_ControlU8_I2CACNFG,
    I2CACNTL = NiFpga_MyRio1950Fpga30_ControlU8_I2CACNTL,
    I2CACNTR = NiFpga_MyRio1950Fpga30_ControlU8_I2CACNTR,
    I2CADATO = NiFpga_MyRio1950Fpga30_ControlU8_I2CADATO,
    I2CBADDR = NiFpga_MyRio1950Fpga30_ControlU8_I2CBADDR,
    I2CBCNFG = NiFpga_MyRio1950Fpga30_ControlU8_I2CBCNFG,
    I2CBCNTL = NiFpga_MyRio1950Fpga30_ControlU8_I2CBCNTL,
    I2CBCNTR = NiFpga_MyRio1950Fpga30_ControlU8_I2CBCNTR,
    I2CBDATO = NiFpga_MyRio1950Fpga30_ControlU8_I2CBDATO,
    IRQAI_A_0NO = NiFpga_MyRio1950Fpga30_ControlU8_IRQAI_A_0NO,
    IRQAI_A_1NO = NiFpga_MyRio1950Fpga30_ControlU8_IRQAI_A_1NO,
    IRQAI_A_30CNFG = NiFpga_MyRio1950Fpga30_ControlU8_IRQAI_A_30CNFG,
    IRQDIO_A_0NO = NiFpga_MyRio1950Fpga30_ControlU8_IRQDIO_A_0NO,
    IRQDIO_A_1NO = NiFpga_MyRio1950Fpga30_ControlU8_IRQDIO_A_1NO,
    IRQDIO_A_2NO = NiFpga_MyRio1950Fpga30_ControlU8_IRQDIO_A_2NO,
    IRQDIO_A_3NO = NiFpga_MyRio1950Fpga30_ControlU8_IRQDIO_A_3NO,
    IRQDIO_A_70ENA = NiFpga_MyRio1950Fpga30_ControlU8_IRQDIO_A_70ENA,
    IRQDIO_A_70FALL = NiFpga_MyRio1950Fpga30_ControlU8_IRQDIO_A_70FALL,
    IRQDIO_A_70RISE = NiFpga_MyRio1950Fpga30_ControlU8_IRQDIO_A_70RISE,
    IRQDI_BTNNO = NiFpga_MyRio1950Fpga30_ControlU8_IRQDI_BTNNO,
    PWMA_0CNFG = NiFpga_MyRio1950Fpga30_ControlU8_PWMA_0CNFG,
    PWMA_0CS = NiFpga_MyRio1950Fpga30_ControlU8_PWMA_0CS,
    PWMA_1CNFG = NiFpga_MyRio1950Fpga30_ControlU8_PWMA_1CNFG,
    PWMA_1CS = NiFpga_MyRio1950Fpga30_ControlU8_PWMA_1CS,
    PWMA_2CNFG = NiFpga_MyRio1950Fpga30_ControlU8_PWMA_2CNFG,
    PWMA_2CS = NiFpga_MyRio1950Fpga30_ControlU8_PWMA_2CS,
    PWMB_0CNFG = NiFpga_MyRio1950Fpga30_ControlU8_PWMB_0CNFG,
    PWMB_0CS = NiFpga_MyRio1950Fpga30_ControlU8_PWMB_0CS,
    PWMB_1CNFG = NiFpga_MyRio1950Fpga30_ControlU8_PWMB_1CNFG,
    PWMB_1CS = NiFpga_MyRio1950Fpga30_ControlU8_PWMB_1CS,
    PWMB_2CNFG = NiFpga_MyRio1950Fpga30_ControlU8_PWMB_2CNFG,
    PWMB_2CS = NiFpga_MyRio1950Fpga30_ControlU8_PWMB_2CS,
    SPIAIRQNO = NiFpga_MyRio1950Fpga30_ControlU8_SPIAIRQNO,
    SPIBIRQNO = NiFpga_MyRio1950Fpga30_ControlU8_SPIBIRQNO,
    SYSSELECTA = NiFpga_MyRio1950Fpga30_ControlU8_SYSSELECTA,
    SYSSELECTB = NiFpga_MyRio1950Fpga30_ControlU8_SYSSELECTB,
} MyRio1950Fpga30_ControlU8;


/**
 * Redefinition of the NiFpga U16 Control enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadU16 and NiFpga_WriteU16 functions should be used to access these
 * items.
 */
typedef enum
{
    AOA_0VAL = NiFpga_MyRio1950Fpga30_ControlU16_AOA_0VAL,
    AOA_1VAL = NiFpga_MyRio1950Fpga30_ControlU16_AOA_1VAL,
    AOB_0VAL = NiFpga_MyRio1950Fpga30_ControlU16_AOB_0VAL,
    AOB_1VAL = NiFpga_MyRio1950Fpga30_ControlU16_AOB_1VAL,
    IRQAI_A_0HYSTERESIS = NiFpga_MyRio1950Fpga30_ControlU16_IRQAI_A_0HYSTERESIS,
    IRQAI_A_0THRESHOLD = NiFpga_MyRio1950Fpga30_ControlU16_IRQAI_A_0THRESHOLD,
    IRQAI_A_1HYSTERESIS = NiFpga_MyRio1950Fpga30_ControlU16_IRQAI_A_1HYSTERESIS,
    IRQAI_A_1THRESHOLD = NiFpga_MyRio1950Fpga30_ControlU16_IRQAI_A_1THRESHOLD,
    PWMA_0CMP = NiFpga_MyRio1950Fpga30_ControlU16_PWMA_0CMP,
    PWMA_0MAX = NiFpga_MyRio1950Fpga30_ControlU16_PWMA_0MAX,
    PWMA_1CMP = NiFpga_MyRio1950Fpga30_ControlU16_PWMA_1CMP,
    PWMA_1MAX = NiFpga_MyRio1950Fpga30_ControlU16_PWMA_1MAX,
    PWMA_2CMP = NiFpga_MyRio1950Fpga30_ControlU16_PWMA_2CMP,
    PWMA_2MAX = NiFpga_MyRio1950Fpga30_ControlU16_PWMA_2MAX,
    PWMB_0CMP = NiFpga_MyRio1950Fpga30_ControlU16_PWMB_0CMP,
    PWMB_0MAX = NiFpga_MyRio1950Fpga30_ControlU16_PWMB_0MAX,
    PWMB_1CMP = NiFpga_MyRio1950Fpga30_ControlU16_PWMB_1CMP,
    PWMB_1MAX = NiFpga_MyRio1950Fpga30_ControlU16_PWMB_1MAX,
    PWMB_2CMP = NiFpga_MyRio1950Fpga30_ControlU16_PWMB_2CMP,
    PWMB_2MAX = NiFpga_MyRio1950Fpga30_ControlU16_PWMB_2MAX,
    SPIACNFG = NiFpga_MyRio1950Fpga30_ControlU16_SPIACNFG,
    SPIACNT = NiFpga_MyRio1950Fpga30_ControlU16_SPIACNT,
    SPIADATO = NiFpga_MyRio1950Fpga30_ControlU16_SPIADATO,
    SPIBCNFG = NiFpga_MyRio1950Fpga30_ControlU16_SPIBCNFG,
    SPIBCNT = NiFpga_MyRio1950Fpga30_ControlU16_SPIBCNT,
    SPIBDATO = NiFpga_MyRio1950Fpga30_ControlU16_SPIBDATO,
} MyRio1950Fpga30_ControlU16;


/**
 * Redefinition of the NiFpga U32 Control enum.
 * This is provided as a convenience to shorten the default enum names. The
 * NiFpga_ReadU32 and NiFpga_WriteU32 functions should be used to access these
 * items.
 */
typedef enum
{
    IRQDIO_A_0CNT = NiFpga_MyRio1950Fpga30_ControlU32_IRQDIO_A_0CNT,
    IRQDIO_A_1CNT = NiFpga_MyRio1950Fpga30_ControlU32_IRQDIO_A_1CNT,
    IRQDIO_A_2CNT = NiFpga_MyRio1950Fpga30_ControlU32_IRQDIO_A_2CNT,
    IRQDIO_A_3CNT = NiFpga_MyRio1950Fpga30_ControlU32_IRQDIO_A_3CNT,
    IRQDI_BTNCNT = NiFpga_MyRio1950Fpga30_ControlU32_IRQDI_BTNCNT,
    IRQTIMERWRITE = NiFpga_MyRio1950Fpga30_ControlU32_IRQTIMERWRITE,
} MyRio1950Fpga30_ControlU32;


#endif /* MyRio1950_h_ */
