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
This is the header file for the Epos Motor class. This class holds all the 
lower level commands sent to the Maxon controllers. This specific version is 
customized to work with the EPOS4 controller but it can be modified to work 
with other controllers
*/

#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <string>
#include <windows.h>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

// Implements a new noncopyable EposMotor derived from the MEL device class 
class EposMotor : public Device, NonCopyable {

public:

	// constructor/destructor for the epos motor class. require device name
    EposMotor(const std::string& name, const std::string& portName);
    ~EposMotor();

	// device connection functions
	void		 start();
	void		 end();

	// device parameter functions
	void		 setPort(char* port);
	void		 setControlParam(unsigned int desVel, unsigned int desAcc, unsigned int desDec);
	//void setRecorderParam(unsigned int desSampleFreq, unsigned int desSamples);

	// movement functions
	void		 move(long desPosition);
	void		 getPosition(long& position);
	BOOL		 targetReached();

private:
    // device variable
	char*		 portName_;
	DWORD		 errorCode_;
	byte		 nodeId_;
	void*		 keyHandle_;

	// control parameter variables
	unsigned int desVelocity_;
	unsigned int desAcceleration_;
	unsigned int desDeceleration_;

	// data recorder variables
	//WORD		 samplePeriod; 
	//WORD		 samples;

private: 

	// device connection functions
	void		 enableControl();
	void		 disableControl();

	// device parameter functions
	void		 setControlParam();
	//void setRecorderParam();

	// movement functions
	void		 halt();

	// device enable/disable
    bool on_enable() 	override;
    bool on_disable() 	override;
};

} // namespace mel