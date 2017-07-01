#pragma once
#include "OpenWrist.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace ip = boost::interprocess;

class OpenWristShare {

public:

	/* default constructor */
	OpenWristShare(OpenWrist *open_wrist);

	/* pointer to OpenWrist object that's being shared by this instance */
	OpenWrist *open_wrist_;

	/* variables to be shared */
	int(*ints_from)[16];
	int(*ints_to)[16];
	double(*doubles_from)[16];
	double(*doubles_to)[16];
	double(*state)[16];

	void update_state(double timestamp);

private:

	/* native windows shared memory objects for OpenWristShare DLL */
	ip::windows_shared_memory shmem_ints_from;     /* 16 ints    * 4 bytes/int    */
	ip::windows_shared_memory shmem_ints_to;       /* 16 ints    * 4 bytes/int    */
	ip::windows_shared_memory shmem_doubles_from;  /* 16 doubles * 8 bytes/double */
	ip::windows_shared_memory shmem_doubes_to;     /* 16 doubles * 8 bytes/double */
	ip::windows_shared_memory shmem_state;         /* 16 doubles * 8 bytes/double */

												   /* mapped regions for OpenWristShare DLL */
	ip::mapped_region region_ints_from;
	ip::mapped_region region_ints_to;
	ip::mapped_region region_doubles_from;
	ip::mapped_region region_doubes_to;
	ip::mapped_region region_state;

};