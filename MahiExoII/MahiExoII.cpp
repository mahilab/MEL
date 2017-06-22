#include "MahiExoII.h"

MahiExoII::MahiExoII() {


	// set up joints
	joints_[0] = RevoluteJoint();
	joints_[1] = RevoluteJoint();
	joints_ptrs_[0] = new RevoluteJoint();
	
}