#pragma once

namespace mel {

    //-------------------------------------------------------------------------
    // TYPEDEF BASIC TYPES
    //-------------------------------------------------------------------------

    typedef             char  int8;   ///<                       -127 to +127
    typedef unsigned    char uint8;   ///<                          0 to +255
    typedef            short  int16;  ///<                    -32,767 to +32,767
    typedef unsigned   short uint16;  ///<                          0 to +65,535
    typedef              int  int32;  ///<             -2,147,483,648 to +2,147,483,647
    typedef unsigned     int uint32;  ///<                          0 to +4,294,967,295
    typedef signed   __int64 int64;   ///< -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807
    typedef unsigned __int64 uint64;  ///<                          0 to 18,446,744,073,709,551,615

    typedef std::vector<char>   char_vec;
    typedef std::vector<int8>   int8_vec;
    typedef std::vector<uint8>  uint8_vec;
    typedef std::vector<int16>  int16_vec;
    typedef std::vector<uint16> uint16_vec;
    typedef std::vector<int32>  int32_vec;
    typedef std::vector<uint32> uint32_vec;
    typedef std::vector<int64>  int64_vec;
    typedef std::vector<uint64> uint64_vec;
    typedef std::vector<double> double_vec;

    //-------------------------------------------------------------------------
    // TYPDEF MEL TYPES BASED ON DAQ MANUFACTUERER
    //-------------------------------------------------------------------------

    #define QUANSER
    #ifdef QUANSER

    typedef double voltage;
    typedef int8   dsignal;
    typedef uint32 channel;

    typedef std::vector<voltage> voltage_vec;
    typedef std::vector<dsignal> dsignal_vec;
    typedef std::vector<channel> channel_vec;

    #endif

    #ifdef NI
    #endif

}