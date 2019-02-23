# This file groups all MEL header and source files 

#===============================================================================
# HEADER FILES
#===============================================================================

# headers directory
set(MEL_HEADERS_DIR "${CMAKE_SOURCE_DIR}/include/MEL")

# collections of common subdirectory headers
set(MEL_BASE_HEADERS "")
set(MEL_COMMUNICATIONS_HEADERS "")
set(MEL_CORE_HEADERS "")
set(MEL_DAQ_HEADERS "")
set(MEL_DEVICES_HEADERS "")
set(MEL_LOGGING_HEADERS "")
set(MEL_MATH_HEADERS "")
set(MEL_MECHATRONICS_HEADERS "")
set(MEL_UTILITY_HEADERS "")

# collections of platform/hardware specific headers
set(MEL_WINDOWS_HEADERS "")
set(MEL_MYRIO_HEADERS "")
set(MEL_QUANSER_HEADERS "")
set(MEL_MYO_HEADERS "")

# MEL Base
list(APPEND MEL_BASE_HEADERS
    "${MEL_HEADERS_DIR}/Communications.hpp"
    "${MEL_HEADERS_DIR}/Config.hpp"
    "${MEL_HEADERS_DIR}/Core.hpp"
    "${MEL_HEADERS_DIR}/Math.hpp"
    "${MEL_HEADERS_DIR}/Mechatronics.hpp"
)

# MEL Communications
set(MEL_COMMUNICATIONS_HEADERS_DIR "${MEL_HEADERS_DIR}/Communications")
list(APPEND MEL_COMMUNICATIONS_HEADERS
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/Http.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/IpAddress.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/MelNet.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/MelShare.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/Packet.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/SerialPort.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/SerialSettings.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/SharedMemory.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/Socket.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/SocketSelector.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/TcpListener.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/TcpSocket.hpp"
    "${MEL_COMMUNICATIONS_HEADERS_DIR}/UdpSocket.hpp"
)

# MEL Core
set(MEL_CORE_HEADERS_DIR "${MEL_HEADERS_DIR}/Core")
list(APPEND MEL_CORE_HEADERS
    "${MEL_CORE_HEADERS_DIR}/Clock.hpp"
    "${MEL_CORE_HEADERS_DIR}/Console.hpp"
    "${MEL_CORE_HEADERS_DIR}/Device.hpp"
    "${MEL_CORE_HEADERS_DIR}/Frequency.hpp"
    "${MEL_CORE_HEADERS_DIR}/NonCopyable.hpp"
    "${MEL_CORE_HEADERS_DIR}/Time.hpp"
    "${MEL_CORE_HEADERS_DIR}/Timer.hpp"
    "${MEL_CORE_HEADERS_DIR}/Timestamp.hpp"
    "${MEL_CORE_HEADERS_DIR}/Types.hpp"
)

# MEL DAQ
set(MEL_DAQ_HEADERS_DIR "${MEL_HEADERS_DIR}/Daq")
list(APPEND MEL_DAQ_HEADERS
    "${MEL_DAQ_HEADERS_DIR}/ChannelBase.hpp"
    "${MEL_DAQ_HEADERS_DIR}/DaqBase.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Encoder.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Input.hpp"
    "${MEL_DAQ_HEADERS_DIR}/InputOutput.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Module.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Output.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Registry.hpp"
    "${MEL_DAQ_HEADERS_DIR}/VirtualDaq.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Watchdog.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Detail/ChannelBase.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Input.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/InputOutput.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Module.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Output.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Registry.inl"
)

# MEL Devices
set(MEL_DEVICES_HEADERS_DIR "${MEL_HEADERS_DIR}/Devices")
list(APPEND MEL_DEVICES_HEADERS
    "${MEL_DEVICES_HEADERS_DIR}/AtiSensor.hpp"
    "${MEL_DEVICES_HEADERS_DIR}/VoltPaqX4.hpp"
)

# MEL Logging
set(MEL_LOGGING_HEADERS_DIR "${MEL_HEADERS_DIR}/Logging")
list(APPEND MEL_LOGGING_HEADERS
    "${MEL_LOGGING_HEADERS_DIR}/Csv.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/DataLogger.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/File.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Log.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/LogRecord.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Severity.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Table.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Detail/Csv.inl"
    "${MEL_LOGGING_HEADERS_DIR}/Detail/StreamMeta.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Formatters/CsvFormatter.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Formatters/FuncMessageFormatter.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Formatters/MessageOnlyFormatter.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Formatters/TxtFormatter.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Writers/ColorConsoleWriter.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Writers/ConsoleWriter.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Writers/RollingFileWriter.hpp"
    "${MEL_LOGGING_HEADERS_DIR}/Writers/Writer.hpp"
)

# MEL Math
set(MEL_MATH_HEADERS_DIR "${MEL_HEADERS_DIR}/Math")
list(APPEND MEL_MATH_HEADERS
    "${MEL_MATH_HEADERS_DIR}/Butterworth.hpp"
    "${MEL_MATH_HEADERS_DIR}/Chirp.hpp"
    "${MEL_MATH_HEADERS_DIR}/Constants.hpp"
    "${MEL_MATH_HEADERS_DIR}/Differentiator.hpp"
    "${MEL_MATH_HEADERS_DIR}/Filter.hpp"
    "${MEL_MATH_HEADERS_DIR}/Functions.hpp"
    "${MEL_MATH_HEADERS_DIR}/Integrator.hpp"
    "${MEL_MATH_HEADERS_DIR}/Random.hpp"
    "${MEL_MATH_HEADERS_DIR}/TimeFunction.hpp"
    "${MEL_MATH_HEADERS_DIR}/Waveform.hpp"
)

# MEL Mechatronics
set(MEL_MECHATRONICS_HEADERS_DIR "${MEL_HEADERS_DIR}/Mechatronics")
list(APPEND MEL_MECHATRONICS_HEADERS
    "${MEL_MECHATRONICS_HEADERS_DIR}/Actuator.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/Amplifier.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/ForceSensor.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/Joint.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/Limiter.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/Motor.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/PdController.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/PidController.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/PositionSensor.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/Robot.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/TorqueSensor.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/VelocitySensor.hpp"
    "${MEL_MECHATRONICS_HEADERS_DIR}/VirtualVelocitySensor.hpp"
)

# MEL Utility
set(MEL_UTILITY_HEADERS_DIR "${MEL_HEADERS_DIR}/Utility")
list(APPEND MEL_UTILITY_HEADERS
    "${MEL_UTILITY_HEADERS_DIR}/Lock.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/Mutex.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/NamedMutex.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/Options.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/RingBuffer.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/Singleton.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/Spinlock.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/StateMachine.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/StlStreams.hpp"
    "${MEL_UTILITY_HEADERS_DIR}/System.hpp"
)

# collect common header files
set(MEL_COMMON_HEADERS
    ${MEL_BASE_HEADERS}
    ${MEL_COMMUNICATIONS_HEADERS}
    ${MEL_CORE_HEADERS}
    ${MEL_DAQ_HEADERS}
    ${MEL_DEVICES_HEADERS}
    ${MEL_LOGGING_HEADERS}
    ${MEL_MATH_HEADERS}
    ${MEL_MECHATRONICS_HEADERS}
    ${MEL_UTILITY_HEADERS}
)

list(APPEND MEL_WINDOWS_HEADERS
    "${MEL_UTILITY_HEADERS_DIR}/Windows/ExternalApp.hpp"
    "${MEL_DEVICES_HEADERS_DIR}/Windows/Keyboard.hpp"
    "${MEL_DEVICES_HEADERS_DIR}/Windows/XboxController.hpp"
)

list(APPEND MEL_MYRIO_HEADERS
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRio.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioAI.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioAO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioConnector.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioDIO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioEncoder.hpp"
)

list(APPEND MEL_QUANSER_HEADERS
    "${MEL_DAQ_HEADERS_DIR}/Quanser/Q2Usb.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/Q8Usb.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserAI.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserAO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDaq.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDI.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDIO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserEncoder.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserOptions.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserWatchdog.hpp"
)

list(APPEND MEL_MYO_HEADERS
    "${MEL_DEVICES_HEADERS_DIR}/Myo/MyoBand.hpp"
)

#===============================================================================
# SOURCE FILES
#===============================================================================

# source directory
set(MEL_SRC_DIR "${CMAKE_SOURCE_DIR}/src/MEL")

# collections of subdirectory sources
set(MEL_BASE_SRC "")
set(MEL_COMMUNICATIONS_SRC "")
set(MEL_CORE_SRC "")
set(MEL_DAQ_SRC "")
set(MEL_DEVICES_SRC "")
set(MEL_LOGGING_SRC "")
set(MEL_MATH_SRC "")
set(MEL_MECHATRONICS_SRC "")
set(MEL_UTILITY_SRC "")

# collections of platform/hardware specific sources
set(MEL_WINDOWS_SRC "")
set(MEL_QUANSER_SRC "")
set(MEL_MYRIO_SRC "")
set(MEL_MYO_SRC "")

# MEL Base
# (no source files yet)

# MEL Communications
set(MEL_COMMUNICATIONS_SRC_DIR "${MEL_SRC_DIR}/Communications")
list(APPEND MEL_COMMUNICATIONS_SRC
    "${MEL_COMMUNICATIONS_SRC_DIR}/Http.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/IpAddress.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/MelNet.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/MelShare.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/Packet.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/SerialPort.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/SharedMemory.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/Socket.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/SocketSelector.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/TcpListener.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/TcpSocket.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/UdpSocket.cpp"
    "${MEL_COMMUNICATIONS_SRC_DIR}/Detail/rs232.c"
    "${MEL_COMMUNICATIONS_SRC_DIR}/Detail/rs232.h"
)

# MEL Core
set(MEL_CORE_SRC_DIR "${MEL_SRC_DIR}/Core")
list(APPEND MEL_CORE_SRC
    "${MEL_CORE_SRC_DIR}/Clock.cpp"
    "${MEL_CORE_SRC_DIR}/Console.cpp"
    "${MEL_CORE_SRC_DIR}/Device.cpp"
    "${MEL_CORE_SRC_DIR}/Frequency.cpp"
    "${MEL_CORE_SRC_DIR}/Time.cpp"
    "${MEL_CORE_SRC_DIR}/Timer.cpp"
    "${MEL_CORE_SRC_DIR}/Timestamp.cpp"
    "${MEL_CORE_SRC_DIR}/Types.cpp"
)

# MEL DAQ
set(MEL_DAQ_SRC_DIR "${MEL_SRC_DIR}/Daq")
list(APPEND MEL_DAQ_SRC
    "${MEL_DAQ_SRC_DIR}/DaqBase.cpp"
    "${MEL_DAQ_SRC_DIR}/Encoder.cpp"
    "${MEL_DAQ_SRC_DIR}/Module.cpp"
    "${MEL_DAQ_SRC_DIR}/Registry.cpp"
    "${MEL_DAQ_SRC_DIR}/VirtualDaq.cpp"
    "${MEL_DAQ_SRC_DIR}/Watchdog.cpp"
)

# MEL Devices
set(MEL_DEVICES_SRC_DIR "${MEL_SRC_DIR}/Devices")
list(APPEND MEL_DEVICES_SRC
    "${MEL_DEVICES_SRC_DIR}/AtiSensor.cpp"
    "${MEL_DEVICES_SRC_DIR}/VoltPaqX4.cpp"
)

# MEL Logging
set(MEL_LOGGING_SRC_DIR "${MEL_SRC_DIR}/Logging")
list(APPEND MEL_LOGGING_SRC
    "${MEL_LOGGING_SRC_DIR}/Csv.cpp"
    "${MEL_LOGGING_SRC_DIR}/DataLogger.cpp"
    "${MEL_LOGGING_SRC_DIR}/File.cpp"
    "${MEL_LOGGING_SRC_DIR}/Log.cpp"
    "${MEL_LOGGING_SRC_DIR}/LogRecord.cpp"
    "${MEL_LOGGING_SRC_DIR}/Table.cpp"
)

# MEL Math
set(MEL_MATH_SRC_DIR "${MEL_SRC_DIR}/Math")
list(APPEND MEL_MATH_SRC
    "${MEL_MATH_SRC_DIR}/Butterworth.cpp"
    "${MEL_MATH_SRC_DIR}/Chirp.cpp"
    "${MEL_MATH_SRC_DIR}/Constants.cpp"
    "${MEL_MATH_SRC_DIR}/Differentiator.cpp"
    "${MEL_MATH_SRC_DIR}/Filter.cpp"
    "${MEL_MATH_SRC_DIR}/Functions.cpp"
    "${MEL_MATH_SRC_DIR}/Integrator.cpp"
    "${MEL_MATH_SRC_DIR}/Random.cpp"
    "${MEL_MATH_SRC_DIR}/TimeFunction.cpp"
    "${MEL_MATH_SRC_DIR}/Waveform.cpp"
)

# MEL Mechatronics
set(MEL_MECHATRONICS_SRC_DIR "${MEL_SRC_DIR}/Mechatronics")
list(APPEND MEL_MECHATRONICS_SRC
    "${MEL_MECHATRONICS_SRC_DIR}/Actuator.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/Amplifier.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/ForceSensor.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/Joint.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/Limiter.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/Motor.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/PdController.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/PidController.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/PositionSensor.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/Robot.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/TorqueSensor.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/VelocitySensor.cpp"
    "${MEL_MECHATRONICS_SRC_DIR}/VirtualVelocitySensor.cpp"
)

# MEL Utility
set(MEL_UTILITY_SRC_DIR "${MEL_SRC_DIR}/Utility")
list(APPEND MEL_UTILITY_SRC
    "${MEL_UTILITY_SRC_DIR}/Lock.cpp"
    "${MEL_UTILITY_SRC_DIR}/Mutex.cpp"
    "${MEL_UTILITY_SRC_DIR}/NamedMutex.cpp"
    "${MEL_UTILITY_SRC_DIR}/Spinlock.cpp"
    "${MEL_UTILITY_SRC_DIR}/StateMachine.cpp"
    "${MEL_UTILITY_SRC_DIR}/System.cpp"
)

# collect common source files
set(MEL_COMMON_SRC
    ${MEL_BASE_SRC}
    ${MEL_COMMUNICATIONS_SRC}
    ${MEL_CORE_SRC}
    ${MEL_DAQ_SRC}
    ${MEL_DEVICES_SRC}
    ${MEL_LOGGING_SRC}
    ${MEL_MATH_SRC}
    ${MEL_MECHATRONICS_SRC}
    ${MEL_UTILITY_SRC}
)

list(APPEND MEL_WINDOWS_SRC
    "${MEL_DEVICES_SRC_DIR}/Windows/Keyboard.cpp"
    "${MEL_DEVICES_SRC_DIR}/Windows/XboxController.cpp"
    "${MEL_UTILITY_SRC_DIR}/Windows/ExternalApp.cpp"
)

list(APPEND MEL_MYRIO_SRC
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/MyRio.cpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/MyRioAI.cpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/MyRioAO.cpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/MyRioConnector.cpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/MyRioDIO.cpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/MyRioEncoder.cpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioUtil.cpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioUtil.hpp"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/IRQConfigure.c"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/IRQConfigure.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/libvisa.so"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/MyRio.c"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/MyRio.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/MyRio1900.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/MyRio1950.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/NiFpga.c"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/NiFpga.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/NiFpga_MyRio1900Fpga60.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/NiFpga_MyRio1950Fpga60.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/visa.h"
    "${MEL_DAQ_SRC_DIR}/NI/myRIO/Detail/MyRioFpga60/visatype.h"
)

list(APPEND MEL_QUANSER_SRC
    "${MEL_DAQ_SRC_DIR}/Quanser/Q2Usb.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/Q8Usb.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserAI.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserAO.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDaq.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDI.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDIO.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDO.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserEncoder.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserOptions.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserWatchdog.cpp"
)

list(APPEND MEL_MYO_SRC
    "${MEL_DEVICES_SRC_DIR}/Myo/MyoBand.cpp"
)