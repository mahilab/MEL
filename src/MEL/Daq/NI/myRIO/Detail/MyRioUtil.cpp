#include "MyRioUtil.hpp"
#include <MEL/Logging/Log.hpp>
#include "MyRioFpga60/MyRio.h"
#include <cassert>

extern NiFpga_Session myrio_session;

/*

// TODO: Make error code lookup table

0	NiFpga_Status_Success	No errors or warnings.
−50400	NiFpga_Status_FifoTimeout	The timeout expired before the FIFO operation could complete.
−52000	NiFpga_Status_MemoryFull	A memory allocation failed. Try again after rebooting.
−52003	NiFpga_Status_SoftwareFault	An unexpected software error occurred.
−52005	NiFpga_Status_InvalidParameter	A parameter to a function was not valid. This could be a NULL pointer, a bad value, etc.
−52006	NiFpga_Status_ResourceNotFound	A required resource was not found. The NiFpga.* library, the RIO resource, or some other resource may be missing.
−52010	NiFpga_Status_ResourceNotInitialized	A required resource was not properly initialized. This could occur if NiFpga_Initialize was not called or a required NiFpga_IrqContext was not reserved.
−61003	NiFpga_Status_FpgaAlreadyRunning	The FPGA is already running.
−61018	NiFpga_Status_DownloadError	An error occurred downloading the VI to the FPGA device. Verify that the target is connected and powered and that the resource of the target is properly configured.
−61024	NiFpga_Status_DeviceTypeMismatch	The bitfile was not compiled for the specified resource's device type.
−61046	NiFpga_Status_CommunicationTimeout	An error was detected in the communication between the host computer and the FPGA target.
−61060	NiFpga_Status_IrqTimeout	The timeout expired before any of the IRQs were asserted.
−61070	NiFpga_Status_CorruptBitfile	The specified bitfile is invalid or corrupt.
−61072	NiFpga_Status_BadDepth	The requested FIFO depth is invalid. It is either 0 or an amount not supported by the hardware.
−61073	NiFpga_Status_BadReadWriteCount	The number of FIFO elements is invalid. Either the number is greater than the depth of the host memory DMA FIFO, or more elements were requested for release than had been acquired.
−61083	NiFpga_Status_ClockLostLock	A hardware clocking error occurred. A derived clock lost lock with its base clock during the execution of the LabVIEW FPGA VI. If any base clocks with derived clocks are referencing an external source, make sure that the external source is connected and within the supported frequency, jitter, accuracy, duty cycle, and voltage specifications. Also verify that the characteristics of the base clock match the configuration specified in the FPGA Base Clock Properties. If all base clocks with derived clocks are generated from free-running, on-board sources, please contact National Instruments technical support at ni.com/support.
−61141	NiFpga_Status_FpgaBusy	The operation could not be performed because the FPGA is busy. Stop all activities on the FPGA before requesting this operation. If the target is in Scan Interface programming mode, put it in FPGA Interface programming mode.
−61200	NiFpga_Status_FpgaBusyFpgaInterfaceCApi	The operation could not be performed because the FPGA is busy operating in FPGA Interface C API mode. Stop all activities on the FPGA before requesting this operation.
−61201	NiFpga_Status_FpgaBusyScanInterface	The chassis is in Scan Interface programming mode. In order to run FPGA VIs, you must go to the chassis properties page, select FPGA programming mode, and deploy settings.
−61202	NiFpga_Status_FpgaBusyFpgaInterface	The operation could not be performed because the FPGA is busy operating in FPGA Interface mode. Stop all activities on the FPGA before requesting this operation.
−61203	NiFpga_Status_FpgaBusyInteractive	The operation could not be performed because the FPGA is busy operating in Interactive mode. Stop all activities on the FPGA before requesting this operation.
−61204	NiFpga_Status_FpgaBusyEmulation	The operation could not be performed because the FPGA is busy operating in Emulation mode. Stop all activities on the FPGA before requesting this operation.
−61211	NiFpga_Status_ResetCalledWithImplicitEnableRemoval	LabVIEW FPGA does not support the Reset method for bitfiles that allow removal of implicit enable signals in single-cycle Timed Loops.
−61212	NiFpga_Status_AbortCalledWithImplicitEnableRemoval	LabVIEW FPGA does not support the Abort method for bitfiles that allow removal of implicit enable signals in single-cycle Timed Loops.
−61213	NiFpga_Status_CloseAndResetCalledWithImplicitEnableRemoval	LabVIEW FPGA does not support Close and Reset if Last Reference for bitfiles that allow removal of implicit enable signals in single-cycle Timed Loops. Pass NiFpga_CloseAttribute_NoResetIflastSession to NiFpga_Close instead.
−61214	NiFpga_Status_ImplicitEnableRemovalButNotYetRun	For bitfiles that allow removal of implicit enable signals in single-cycle Timed Loops, LabVIEW FPGA does not support this method prior to running the bitfile.
−61215	NiFpga_Status_RunAfterStoppedCalledWithImplicitEnableRemoval	Bitfiles that allow removal of implicit enable signals in single-cycle Timed Loops can run only once. Download the bitfile again before re-running the VI.
−61216	NiFpga_Status_GatedClockHandshakingViolation	A gated clock has violated the handshaking protocol. If you are using external gated clocks, ensure that they follow the required clock gating protocol. If you are generating your clocks internally, please contact National Instruments Technical Support.
−61219	NiFpga_Status_ElementsNotPermissibleToBeAcquired	The number of elements requested must be less than or equal to the number of unacquired elements left in the host memory DMA FIFO. There are currently fewer unacquired elements left in the FIFO than are being requested. Release some acquired elements before acquiring more elements.
−61499	NiFpga_Status_InternalError	An unexpected internal error occurred.
−63003	NiFpga_Status_TotalDmaFifoDepthExceeded	The NI-RIO driver was unable to allocate memory for a FIFO. This can happen when the combined depth of all DMA FIFOs exceeds the maximum depth for the controller, or when the controller runs out of system memory. You may be able to reconfigure the controller with a greater maximum FIFO depth. For more information, refer to NI KnowledgeBase article 65OF2ERQ.
−63033	NiFpga_Status_AccessDenied	Access to the remote system was denied. Use MAX to check the Remote Device Access settings under Software»NI-RIO»NI-RIO Settings on the remote system.
−63038	NiFpga_Status_HostVersionMismatch	The NI-RIO software on the host is not compatible with the software on the target. Upgrade the NI-RIO software on the host in order to connect to this target.
−63040	NiFpga_Status_RpcConnectionError	A connection could not be established to the specified remote device. Ensure that the device is on and accessible over the network, that NI-RIO software is installed, and that the RIO server is running and properly configured.
−63043	NiFpga_Status_RpcSessionError	The RPC session is invalid. The target may have reset or been rebooted. Check the network connection and retry the operation.
−63082	NiFpga_Status NiFpga_Status_FifoReserved	The operation could not complete because another session is accessing the FIFO. Close the other session and retry.
−63083	NiFpga_Status_FifoElementsCurrentlyAcquired	A Read FIFO or Write FIFO function was called while the host had acquired elements of the FIFO. Release all acquired elements before reading or writing.
−63084	NiFpga_Status_MisalignedAccess	A function was called using a misaligned address. The address must be a multiple of the size of the datatype.
−63085	NiFpga_Status_ControlOrIndicatorTooLarge	The FPGA Read/Write Control Function is accessing a control or indicator with data that exceeds the maximum size supported on the current target. Refer to the hardware documentation for the limitations on data types for this target.
−63100	NiFpga_Status_InvalidUsage	Invalid usage. The syntax is as follows: capigen.exe [-e] [<bitfile> [<output_directory> [<prefix_override>]]]
−63101	NiFpga_Status_BitfileReadError	A valid .lvbitx bitfile is required. If you are using a vaild .lvbitx bitfile, the bitfile may not be compatible with the software you are using. Determine which version of LabVIEW was used to make the bitfile, update your software to that version or later, and try again.
−63106	NiFpga_Status_SignatureMismatch	The specified signature does not match the signature of the bitfile. If the bitfile has been recompiled, regenerate the C API and rebuild the application.
−63107	NiFpga_Status_IncompatibleBitfile	The bitfile you are trying to use is incompatible with the version of NI-RIO installed on the target and/or host. Update the version of NI-RIO on the target and/or host to the same version (or later) used to compile the bitfile. Alternatively, recompile the bitfile with the same version of NI-RIO that is currently installed on the target and/or host.
−63192	NiFpga_Status_InvalidResourceName	Either the supplied resource name is invalid as a RIO resource name, or the device was not found. Use MAX to find the proper resource name for the intended device.
−63193	NiFpga_Status_FeatureNotSupported	The requested feature is not supported.
−63194	NiFpga_Status_VersionMismatch	The NI-RIO software on the target system is not compatible with this software. Upgrade the NI-RIO software on the target system.
−63195	NiFpga_Status_InvalidSession	The session is invalid or has been closed.
−63198	NiFpga_Status_OutOfHandles	The maximum number of open FPGA sessions has been reached. Close some open sessions.

*/


namespace mel {

void set_register_bit(unsigned int reg, int i) {
    assert(i >= 0 && i < 8);
    uint8_t bits;
    NiFpga_Status status;
    status = NiFpga_ReadU8(myrio_session, reg, &bits);
    if (status < 0) {
        LOG(Error) << "Could not read from system select register";
        return;
    }
    bits |= (1UL << i);
    status = NiFpga_WriteU8(myrio_session, reg, bits);
    if (status < 0) {
        LOG(Error) << "Could not write to the system select register";
    }
}

void clr_register_bit(unsigned int reg, int i) {
    assert(i >= 0 && i < 8);
    uint8_t bits;
    NiFpga_Status status;
    status = NiFpga_ReadU8(myrio_session, reg, &bits);
    if (status < 0) {
        LOG(Error) << "Could not read from system select register";
        return;
    }
    bits &= ~(1UL << i);
    status = NiFpga_WriteU8(myrio_session, reg, bits);
    if (status < 0) {
        LOG(Error) << "Could not write to the system select register";
    }
}

bool get_register_bit(unsigned int reg, int i) {
    assert(i >= 0 && i < 8);
    uint8_t bits;
    NiFpga_Status status;
    status = NiFpga_ReadU8(myrio_session, reg, &bits);
    if (status < 0) {
        LOG(Error) << "Could not read from system select register";
    }
    return ((bits >> i) & 1) != 0;
}

std::string get_nifpga_error_message(int error) {
    if (error < 0)
        return "(NI FPGA Error #" + std::to_string(error) + ")";
    else if (error > 0)
        return "(NI FPGA Warning #" + std::to_string(error) + ")";
    else
        return "";
}


} // namespace mel
