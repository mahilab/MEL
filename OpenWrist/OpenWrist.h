#pragma once
#include <math.h>
#include <iostream>
#include "Daq.h"
#include <vector>
#include "util.h"
#include <thread>
#include <chrono>

class OpenWrist {

public:

	Daq *daq_;
	static const int num_joints_;

	//double joint_positions_[3] = { 0, 0, 0 };
	double_vec joint_positions_ = { 0, 0, 0 };
	double_vec joint_velocities_ = { 0, 0, 0 };
	double_vec joint_torques_ = { 0, 0, 0 };
	double_vec motor_torques_ = { 0, 0, 0 };
	double_vec motor_currents_ = { 0, 0, 0 };

	/* PUBLIC FUNCTIONS */

	void enable();
	void disable();
	void bind_daq(Daq *daq);
	void read_joint_positions();
	void read_joint_velocities();
	void read_all();
	void command_joint_torques();
	void calibrate(bool &stop);
	void zero_joint_positions();

	/* PUBLIC STATIC FUNCTIONS */

	/* converts encoder counts to joint positions [rad] */
	static void encoder_to_joint_positions(const int_vec &enc_counts, double_vec &joint_positions);
	/* converts encoder counts per second to joint velocities [rad/s] */
	static void encoder_to_joint_velocities(const double_vec &enc_counts_per_sec, double_vec &joint_velocities);
	/* computes joint torques [N-m] per proportional-derivative control law*/
	static void compute_pd_control_joint_torques(const double *joint_positions, const double *joint_velocities, const double *joint_positions_ref, const double *joint_velocities_ref, double *joint_torques);
	/* computes gravity compensation joint torques based on current joint positions */
	static void compute_gravity_compensation_joint_torques(const double_vec &joint_positions, double_vec &gravity_torques);
	/* computes kinetic friction joint torqes based on a tanh Coulomb friction model */
	static void compute_friction_compensation_joint_torques(const double_vec &joint_velocities, double_vec &friction_torques);
	/* converts joint torques [N-m] to motor torques [N-m] */
	static void joint_torques_to_motor_torques(const double_vec &joint_torques, double_vec &motor_torques);
	/* converts motor torques [N-m] to motor currents [A] */
	static void motor_torques_to_motor_currents(const double_vec &motor_torques, double_vec &motor_currents);
	/* converts motor currents [A] to output command voltges to amplifier [V] */
	static void motor_currents_to_output_voltages(const double_vec &motor_currents, double_vec &output_voltages);
	/* imposes hard limits on motor currents */
	static void limit_motor_currents(double_vec &motor_currents);
	/* compute single joint PD control effort */
	static double compute_pd_control_joint_torque_single(const double joint_position, const double joint_velocity, const double joint_position_ref, const double joint_velocity_ref, const double kp, const double kd);
	static double OpenWrist::compute_gravity_compensation_joint_torque_single(int joint, const double *joint_positions);


private:

	static const double_vec kt_;              /* motor torque constants [N-m/A] */
	static const int_vec    enc_resolutions_; /* encoder resolutions [counts/rev] */
	static const int_vec    quad_factors_;    /* quadrature mode scale factors */
	static const double_vec amp_gains_;       /* amplifier gains [A/V] */
	static const double_vec eta_;             /* joint transmission ratios [in/in] */
	static const double_vec kp_;              /* proportional control gains */
	static const double_vec kd_;              /* derivative control gains */
	static const double_vec current_limits_;  /* hard current limits (max. cont. motor current) [A] */
	static const double_vec kin_friction_;    /* joint kinetic friction parameters [N-m] */



};