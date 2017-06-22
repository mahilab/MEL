#pragma once
#include <vector>
#include "Joint.h"
#include "RevoluteJoint.h"
#include "Actuator.h"
#include "Encoder.h"
#include "Daq.h"

class Robot {

public:

	std::vector<Joint> joints_;
	std::vector<Joint*> joints_ptrs_;
	std::vector<Actuator> actuators_;
	std::vector<Encoder> encoders_;
	std::vector<Daq> daqs_;

};