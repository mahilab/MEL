#===============================================================================
# GENERIC EXAMPLES
#===============================================================================

add_executable(tcp ex_tcp.cpp)
target_link_libraries(tcp MEL)

add_executable(udp ex_udp.cpp)
target_link_libraries(udp MEL)

add_executable(melnet ex_melnet.cpp)
target_link_libraries(melnet MEL)

add_executable(ctrl_c_handling ex_ctrl_c_handling.cpp)
target_link_libraries(ctrl_c_handling MEL)

add_executable(datalog ex_datalog.cpp)
target_link_libraries(datalog MEL)

add_executable(options ex_options.cpp)
target_link_libraries(options MEL)

add_executable(lockables ex_lockables.cpp)
target_link_libraries(lockables MEL)

add_executable(ring_buffer ex_ring_buffer.cpp)
target_link_libraries(ring_buffer MEL)

add_executable(logger ex_logger.cpp)
target_link_libraries(logger MEL)

add_executable(melscope ex_melscope.cpp)
target_link_libraries(melscope MEL)

add_executable(haptic_paddle ex_haptic_paddle.cpp)
target_link_libraries(haptic_paddle MEL)

add_executable(math ex_math.cpp)
target_link_libraries(math MEL)

#===============================================================================
# WINDOWS EXAMPLES
#===============================================================================

if(WIN32)
    add_executable(shared_memory ex_shared_memory.cpp)
    target_link_libraries(shared_memory MEL)

    add_executable(melshare ex_melshare.cpp)
    target_link_libraries(melshare MEL)

    add_executable(limiter ex_limiter.cpp)
    target_link_libraries(limiter MEL)
endif()

#===============================================================================
# QUANSER EXAMPLES
#===============================================================================

if(QUANSER)
    add_executable(q2usb ex_q2usb.cpp)
    target_link_libraries(q2usb MEL)

    add_executable(q8usb ex_q8usb.cpp)
    target_link_libraries(q8usb MEL)

    add_executable(emg_q8usb ex_emg_q8usb.cpp)
    target_link_libraries(emg_q8usb MEL)
endif()

#===============================================================================
# NI EXAMPLES
#===============================================================================

if(NI)
    add_executable(crio ex_crio.cpp)
    target_link_libraries(crio MEL)
endif()

#===============================================================================
# OPENWRIST EXAMPLES
#===============================================================================

if (OPENWRIST AND QUANSER)
    add_executable(openwrist_q8usb ex_openwrist_q8usb.cpp)
    target_link_libraries(openwrist_q8usb MEL)
elseif(OPENWRIST AND NI)
    # ...
endif()

#===============================================================================
# MAHI EXO-II EXAMPLES
#===============================================================================

if (MAHIEXOII AND QUANSER)
    add_executable(mahiexoii_q8usb ex_mahiexoii_q8usb.cpp)
    target_link_libraries(mahiexoii_q8usb MEL)

    add_executable(mahiexoiiemg_q8usb ex_mahiexoiiemg_q8usb.cpp)
    target_link_libraries(mahiexoiiemg_q8usb MEL)

endif()


#===============================================================================
# OPENWRIST & MAHI EXO-II EXAMPLES
#===============================================================================

if (OPENWRIST AND MAHIEXOII AND QUANSER)
    add_executable(udp_exo_teleop ex_udp_exo_teleop.cpp)
    target_link_libraries(udp_exo_teleop MEL)
endif()
