// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Zane Zook (gadzooks@rice.edu)

/*
This file defines the Epos Motor class which holds all the lower level commands
sent to the Maxon controllers in the system. This specific version is customized
to work with the EPOS4 controller but it can be modified to work with other
controllers
*/

//==============================================================================
// LIBRARY IMPORT
//==============================================================================

// libraries for Epos Motor Class
#include <MEL/Core/Device/EPOS/EposMotor.hpp>

// libraries for data logging and output
#include <MEL/Logging/Log.hpp>
#include <MEL/Core/Console.hpp>


// other misc standard libraries
#include <iostream>


//==============================================================================
// CONSTRUCTOR
//==============================================================================

/*
Constructor for the maxonMotor class
 */
EposMotor::EposMotor():
    // initializing controller information
	portName = (char*)"USB0";
	nodeId =	1;
	errorCode = 0;
	keyHandle = 0;

	//samplePeriod =	1; //n-times 0.1ms, 1 is 1000Hz
	//samples =		100;

	// initializing motor control parameters
	desVelocity = 10000;
	desAcceleration = 100000;
	desDeceleration = 100000;
{	
}

/*
Destructor for the maxonMotor class
 */
MaxonMotor::~MaxonMotor()
{
}


//==============================================================================
// PRIVATE FUNCTIONS
//==============================================================================

/*
Once the device has been opened, attempts to set the
controller into position control mode.
 */
void MaxonMotor::enableControl()
{
	BOOL inFault = FALSE;

	// checks the controller for any faults
	if (VCS_GetFaultState(keyHandle, nodeId, &inFault, &errorCode))
	{
		// attempts to clear the fault from the controller if in a fault
		if (inFault && !VCS_ClearFault(keyHandle, nodeId, &errorCode))
		{
			cout << "Clear fault failed!, error code = " << errorCode << endl;
			return;
		}

		BOOL enabled = FALSE;

		// attempts to enable controller
		if (VCS_GetEnableState(keyHandle, nodeId, &enabled, &errorCode))
		{
			if (!enabled && !VCS_SetEnableState(keyHandle, nodeId, &errorCode))
			{
				cout << "Set enable state failed!, error code = " << errorCode << endl;
			}
			else
			{
				cout << "Set enable state succeeded!" << endl;
			}
		}
	}
	else
	{
		cout << "Get fault state failed!, error code = " << errorCode << endl;
	}

	// attempts to set controller to position control mode
	if (!VCS_ActivateProfilePositionMode(keyHandle, nodeId, &errorCode))
	{
		cout << "Activate profile position mode failed!" << endl;
	}

	// sets the controller's data recorder parameters
	//setRecorderParam();
}

/*
Sets the position control parameters once the device
has been enabled
 */
void MaxonMotor::setControlParam()
{
	// attempts to set the controllers' position control parameters
	if (!VCS_SetPositionProfile(keyHandle, nodeId, desVelocity, desAcceleration, desDeceleration, &errorCode))
	{
		cout << "Position control parameter update failed!, error code = " << errorCode << endl;
	}
	cout << "Position control parameters updated!" << endl;
}

/*
Sets the data recorder parameters once the device
has been enabled
*/
//void maxonMotor::setRecorderParam()
//{
//	// define relevant parameters for data recorder
//	WORD demPosIndex = 0x6062; // addresses found from EPOS studio software
//	WORD actPosIndex = 0x6064;
//	BYTE posSubIndex = 0x00; 
//	BYTE objectSize = 4; // positions values are in int32 format
//	
//	// attempts to set the recorder parameters
//	if (!VCS_SetRecorderParameter(keyHandle, nodeId, samplePeriod, samples, &errorCode)) 
//	{
//		cout << "Data recorder parameter update failed!, error code = " << errorCode << endl;
//	}
//	cout << "Recorder parameters updated!" << endl;
//
//	// activates demand position data recorder channel
//	if (!VCS_ActivateChannel(keyHandle, nodeId, 1, demPosIndex, posSubIndex, objectSize, &errorCode))
//	{
//		cout << "Data recorder channel activation failed!, error code = " << errorCode << endl;
//	}
//	cout << "Recorder channels activated!" << endl;
//
//	// activates actual position recorder channel
//	if (!VCS_ActivateChannel(keyHandle, nodeId, 2, demPosIndex, posSubIndex, objectSize, &errorCode))
//	{
//		cout << "Data recorder channel activation failed!, error code = " << errorCode << endl;
//	}
//	cout << "Recorder channels activated!" << endl;
//}

/*
Turns off the position control on the controller
 */
void MaxonMotor::disableControl()
{
	BOOL inFault = FALSE;

	// checks the controller for any faults
	if (VCS_GetFaultState(keyHandle, nodeId, &inFault, &errorCode))
	{
		// attempts to clear the fault from the controller if in a fault
		if (inFault && !VCS_ClearFault(keyHandle, nodeId, &errorCode))
		{
			cout << "Clear fault failed!, error code = " << errorCode << endl;
			return;
		}

		BOOL enabled = FALSE;

		// attempts to disable controller
		if (VCS_GetEnableState(keyHandle, nodeId, &enabled, &errorCode))
		{
			if (enabled && !VCS_SetDisableState(keyHandle, nodeId, &errorCode))
			{
				cout << "Set disable state failed!, error code = " << errorCode << endl;
			}
			else
			{
				cout << "Set disable state succeeded!" << endl;
			}
		}
	}
	else
	{
		cout << "Get fault state failed!, error code = " << errorCode << endl;
	}
}


//==============================================================================
// DEVICE CONNECTION FUNCTIONS
//==============================================================================

/*
Opens communication from the computer to the specific 
controller being referenced through USB comms.
 */
void MaxonMotor::start()
{
	// Configuring EPOS4 for motor control
	char deviceName[] =		"EPOS4";
	char protocolName[] =	"MAXON SERIAL V2";
	char interfaceName[] =	"USB";

	// Opens device communication
	keyHandle = VCS_OpenDevice(deviceName, protocolName, interfaceName, portName, &errorCode);
	if (keyHandle == 0)
	{
		cout << "Open device failure, error code = " << errorCode << endl;
	}
	else
	{
		cout << "Open device success!" << endl;
	}

	// Enables device in position control mode
	enableControl();
}

/*
Closes communication from the computer to the specific
controller being referenced through USB comms.
 */
void MaxonMotor::end()
{
	// turns off position control
	disableControl();

	cout << "Closing Device!" << endl;

	// closes communication with controller
	if (keyHandle != 0)
	{
		VCS_CloseDevice(keyHandle, &errorCode);
	}
	VCS_CloseAllDevices(&errorCode);
}


//==============================================================================
// DEVICE PARAMETER FUNCTIONS
//==============================================================================

/*
Indicates what USB port the MAXON motor controller is 
connected to.
 */
void MaxonMotor::setPort(char* port)
{
	portName = port;
}

/*
Sets each one of the control parameters for position 
control mode
 */
void MaxonMotor::setControlParam(unsigned int desVel, unsigned int desAcc, unsigned int desDec)
{
	desVelocity =		desVel;
	desAcceleration =	desAcc;
	desDeceleration =	desDec;
	
	// sets the controller's control parameters
	setControlParam();
}

/*
Sets the sampling freqeuncy and number of samples or the data recorder
*/
//void maxonMotor::setRecorderParam(unsigned int desSampleFreq, unsigned int desSamples)
//{
//	int HZ_TO_MS = 1/1000;
//	samplePeriod = desSampleFreq * HZ_TO_MS;
//	samples = desSamples;
//}


//==============================================================================
// MOVEMENT FUNCTIONS
//==============================================================================

/*
Commands motor controller to move motor to specified position
 */
void MaxonMotor::move(long desPosition)
{
	BOOL Absolute =		TRUE; 
	BOOL Immediately =	TRUE;

	// convert from degrees to encoder counts
	desPosition = desPosition * g_DEGREES_TO_COUNT;

	// sends signal to move Maxon motor to specified position
	if (!VCS_MoveToPosition(keyHandle, nodeId, desPosition, Absolute, Immediately, &errorCode)) 
	{
		cout << "Move to position failed!, error code = " << errorCode << endl;
		halt();
	}
}

/*
Pings motor for its current position
 */
void MaxonMotor::getPosition(long& position)
{
	// attempts to acquire current position of the motor
	if (!VCS_GetPositionIs(keyHandle, nodeId, &position, &errorCode)) 
	{
		cout << " error while getting current position , error code = " << errorCode << endl;
	}

	// convert from encoder counts to degrees
	position = position / g_DEGREES_TO_COUNT;
}

/*
Pings motor to stop
 */
void MaxonMotor::halt()
{
	// attempts to stop motor in its place
	if (!VCS_HaltPositionMovement(keyHandle, nodeId, &errorCode))
	{
		cout << "Halt position movement failed!, error code = " << errorCode << endl;
	}
}

/*
Checks to see if the motor is still moving or if it has
reached its final destination
*/
BOOL MaxonMotor::targetReached()
{
	BOOL targetReached = FALSE;

	if (!VCS_GetMovementState(keyHandle, nodeId, &targetReached, &errorCode)) 
	{
		cout << "Motion check failed!, error code = " << errorCode << endl;
	}
	return targetReached;
}


//==============================================================================
// DATA RECORDER FUNCTIONS
//==============================================================================

///*
//Tells the controller to begin recording data
//*/
//void MaxonMotor::startRecord()
//{
//	if (!VCS_StartRecorder(keyHandle, nodeId,  &errorCode))
//	{
//		cout << "Data recorder start failed!, error code = " << errorCode << endl;
//	}
//}
//
///*
//Tells the controller to stop recording data
//*/
//void MaxonMotor::stopRecord()
//{
//	if (!VCS_StopRecorder(keyHandle, nodeId, &errorCode))
//	{
//		cout << "Data recorder stop failed!, error code = " << errorCode << endl;
//	}
//}
//
//void MaxonMotor::outputData()
//{
//	char* fileLocation = (char*)"test.csv";
//	if (!VCS_ExportChannelDataToFile(keyHandle, nodeId, fileLocation, &errorCode))
//	{
//		cout << "Data output file record failed!, error code = " << errorCode << endl;
//	}
//	
//	
//	DWORD* vectorSize = new DWORD;
//	BYTE* dataVector = new BYTE;
//	// read the size of the data vector in the channel
//	if (!VCS_ReadChannelVectorSize(keyHandle, nodeId, vectorSize, &errorCode))
//	{
//		cout << "Read channel vector size failed!, error code = " << errorCode << endl;
//	}
//
//	// read data channel
//	if (!VCS_ReadChannelDataVector(keyHandle, nodeId, channel, dataVector, *vectorSize, &errorCode))
//	{
//		cout << "Read channel data failed!, error code = " << errorCode << endl;
//	}
//
//	// convert BYTE array to int array
//	for (int i = 0; i < *vectorSize; i += 4)
//	{
//		outputArray[i / 4] = (int)dataVector[i] << 24 | (int)dataVector[i + 1] << 16
//			| (int)dataVector[i + 2] << 18 | dataVector[i + 3];
//	}
//
//	// free memory up
//	delete dataVector;
//	delete vectorSize;
//}