macro(mel_example MEL_EXAMPLE_NAME)
    project(${MEL_EXAMPLE_NAME})
    add_executable(${MEL_EXAMPLE_NAME} "ex_${MEL_EXAMPLE_NAME}.cpp")
    target_link_libraries(${MEL_EXAMPLE_NAME} MEL)
    set_target_properties(${MEL_EXAMPLE_NAME} PROPERTIES FOLDER "Examples")
endmacro()

#===============================================================================
# GENERIC EXAMPLES
#===============================================================================

mel_example(tcp)
mel_example(udp)
mel_example(melnet)
mel_example(ctrl_c_handling)
mel_example(datalog)
mel_example(filter)
mel_example(options)
mel_example(lockables)
mel_example(ring_buffer)
mel_example(logger)
mel_example(melscope)
mel_example(math)
mel_example(shared_memory)
mel_example(melshare)
mel_example(chat)

if(WIN32)
    mel_example(limiter)
    mel_example(keyboard)
endif()

if(QUANSER)
    mel_example(q2usb)
    mel_example(q8usb)
    mel_example(q8usb_loopback)
    mel_example(haptic_paddle)
    mel_example(overview)
endif()

if(NI_ARM)
    mel_example(myrio)
endif()

if (MYO)
    mel_example(myo)
endif()
