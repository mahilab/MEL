#include "OpenWristShare.h" 

OpenWristShare::OpenWristShare(OpenWrist *open_wrist) {

	/* bind this OpenWristShare to an OpenWrist instance */
	open_wrist_ = open_wrist;

	/* create a native windows shared memory objects for OpenWristShare DLL */
	shmem_ints_from = ip::windows_shared_memory(ip::create_only, "ints_from_open_wrist", ip::read_write, 64);        /* 16 ints    * 4 bytes/int    */
	shmem_ints_to = ip::windows_shared_memory(ip::create_only, "ints_to_open_wrist", ip::read_write, 64);            /* 16 ints    * 4 bytes/int    */
	shmem_doubles_from = ip::windows_shared_memory(ip::create_only, "doubles_from_open_wrist", ip::read_write, 128); /* 16 doubles * 8 bytes/double */
	shmem_doubes_to = ip::windows_shared_memory(ip::create_only, "doubles_to_open_wrist", ip::read_write, 128);      /* 16 doubles * 8 bytes/double */
	shmem_state = ip::windows_shared_memory(ip::create_only, "open_wrist_state", ip::read_write, 128);               /* 16 doubles * 8 bytes/double */

																													 /* create mapped regions for OpenWristShare DLL */
	region_ints_from = ip::mapped_region(shmem_ints_from, ip::read_write);
	region_ints_to = ip::mapped_region(shmem_ints_to, ip::read_write);
	region_doubles_from = ip::mapped_region(shmem_doubles_from, ip::read_write);
	region_doubes_to = ip::mapped_region(shmem_doubes_to, ip::read_write);
	region_state = ip::mapped_region(shmem_state, ip::read_write);

	/* set the mapped region addresses to local variables for manipulation */
	ints_from = (int(*)[16])(region_ints_from.get_address());
	ints_to = (int(*)[16])(region_ints_to.get_address());
	doubles_from = (double(*)[16])(region_doubles_from.get_address());
	doubles_to = (double(*)[16])(region_doubes_to.get_address());
	state = (double(*)[16])(region_state.get_address());

	/* zero all local values used in OpenWristShare */
	for (int i = 0; i < 16; i++) {
		*ints_from[i] = 0;
		*ints_to[i] = 0;
		*doubles_from[i] = 0.0;
		*doubles_to[i] = 0.0;
		*state[i] = 0.0;
	}
}

void OpenWristShare::update_state(double timestamp) {
	/* broadcast state values over OpenWristShare */
	*state[0] = timestamp;
	*state[1] = open_wrist_->joint_positions_[0];
	*state[2] = open_wrist_->joint_positions_[1];
	*state[3] = open_wrist_->joint_positions_[2];
	*state[4] = open_wrist_->joint_velocities_[0];
	*state[5] = open_wrist_->joint_velocities_[1];
	*state[6] = open_wrist_->joint_velocities_[2];
	*state[7] = open_wrist_->joint_torques_[0];
	*state[8] = open_wrist_->joint_torques_[1];
	*state[9] = open_wrist_->joint_torques_[2];
}


