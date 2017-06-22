#pragma once
#include <string>
#include "Encoder.h"
#include "Actuator.h"

class Joint {

public:


	// constructor
	Joint();

	// type and identifier
	std::string type_;

	double position_;
	double velocity_;

	Encoder *enc_;
	Actuator *act_;

};