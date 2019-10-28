# common examples
mel_example(hello_world)
mel_example(tcp)
mel_example(udp)
mel_example(melnet)
mel_example(ctrl_c_handling)
mel_example(table)
mel_example(filter)
mel_example(options)
mel_example(pid)
mel_example(lockables)
mel_example(ring_buffer)
mel_example(log)
mel_example(melscope)
mel_example(math)
mel_example(shared_memory)
mel_example(melshare)
mel_example(chat)
mel_example(comms_server)
mel_example(virtual_daq)
mel_example(serial)
mel_example(csv)
mel_example(time)

# windows only examples
if(WIN32)

    mel_example(limiter)
    mel_example(keyboard)

    # quanser only examples
    if(MEL_QUANSER)
        mel_example(q2usb               DEPENDS MEL::quanser)
        mel_example(q8usb               DEPENDS MEL::quanser)
        mel_example(q8usb_loopback      DEPENDS MEL::quanser)
        mel_example(qpid                DEPENDS MEL::quanser)
        mel_example(haptic_paddle       DEPENDS MEL::quanser)
        mel_example(overview            DEPENDS MEL::quanser)
        mel_example(ati_sensor          DEPENDS MEL::quanser)
        mel_example(quanser_performance DEPENDS MEL::quanser)
    endif()

    # sensoary 826 only examples
    if (MEL_SENSORAY826)
        mel_example(s826 DEPENDS MEL::sensoray MEL::quanser)
    endif()

    # myro example
    if (MEL_MYO)
        mel_example(myoband DEPENDS MEL::myo)
    endif()

endif()

# myRIO example
if(MEL_NI_ARM)
    mel_example(myrio)
endif()


