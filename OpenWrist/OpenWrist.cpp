#include "OpenWrist.h"

#define q0  joint_positions[0]
#define q1  joint_positions[1]
#define q2  joint_positions[2]	

const int        OpenWrist::num_joints_ = 3;
const double_vec OpenWrist::kt_ = { 0.0603, 0.0603, 0.0538 };
const int_vec    OpenWrist::enc_resolutions_ = { 500, 500 ,500 };
const int_vec    OpenWrist::quad_factors_ = { 4, 4, 4 };
const double_vec OpenWrist::amp_gains_ = { 1, 1, 1 };
const double_vec OpenWrist::eta_ = { 0.468 / 8.750, 0.468 / 9.000, 0.234 / 6.000 };
const double_vec OpenWrist::kp_ = { 25, 20 ,20 };
const double_vec OpenWrist::kd_ = { 1.15, 1, 0.25 };
const double_vec OpenWrist::current_limits_ = { 3.17, 3.17, 1.74 };
const double_vec OpenWrist::kin_friction_ = { 0.1891 * 0.75, 0.0541, 0.1339 * 0.5 };

void OpenWrist::bind_daq(Daq *daq) {
	daq_ = daq;
}

void OpenWrist::enable_high() {
	daq_->set_analog_voltages(double_vec(3,0));
    daq_->set_digital_states(char_vec(3,1));
    daq_->write_analog();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	daq_->write_digital();
}

void OpenWrist::disable_low() {
    daq_->set_digital_states(char_vec(3, 0));
	daq_->write_digital();
}

void OpenWrist::read_joint_positions() {
	daq_->read_encoder();
	encoder_to_joint_positions(daq_->get_encoders_counts(), joint_positions_);
}

void OpenWrist::read_joint_velocities() {
	daq_->read_encoder_velocity();
	encoder_to_joint_velocities(daq_->enc_counts_per_sec_, joint_velocities_);
}

void OpenWrist::read_all() {
	read_joint_positions();
	read_joint_velocities();
}

void OpenWrist::zero_joint_positions() {
	daq_->zero_encoder_counts();
}


void OpenWrist::command_joint_torques() {
	/* convert joint torques to amplifer voltage commands */
	OpenWrist::joint_torques_to_motor_torques(joint_torques_, motor_torques_);
	OpenWrist::motor_torques_to_motor_currents(motor_torques_, motor_currents_);
	OpenWrist::limit_motor_currents(motor_currents_);
	OpenWrist::motor_currents_to_output_voltages(motor_currents_, daq_->ao_voltages_);
	/* write analog voltages */
	daq_->write_analog(daq_->ao_voltages_);
}

void OpenWrist::calibrate(bool &stop) {

	/* initialize Q8 USB */
	if (!daq_->init()) {
		std::cout << "Terminating controller" << std::endl;
		return;
	}

	/* zero current position */
	zero_joint_positions();
	/* enable OpenWrist */
	enable_high();
	/* start DAQ watchdog */
	daq_->start_watchdog(0.1);
	/* start joint 1 calibration loop */
	std::cout << "Calibrating Joint 1 ... ";

	std::vector<double> stored_positions;

	double joint_1_hard_stop_position = 1.19696;


	while (!stop) {
		/* reload watchdog */
		daq_->reload_watchdog();
		/* read joint positions and velocities */
		read_all();
		/* compute torques to lock joint 0 and 2*/
		joint_torques_[0] = compute_pd_control_joint_torque_single(joint_positions_[0], joint_velocities_[0], 0, 0, 50, 1);
		joint_torques_[2] = compute_pd_control_joint_torque_single(joint_positions_[2], joint_velocities_[2], 0, 0, 40, 0.25);
		/* apply small torque to joint 1 */
		joint_torques_[1] = 0.2;
		/* command joint torques */
		command_joint_torques();

		stored_positions.push_back(joint_positions_[1]);

		bool moving = true;
		if (stored_positions.size() > 500) {
			moving = false;
			for (int i = stored_positions.size() - 500; i < stored_positions.size(); i++) {
				moving = stored_positions[i] != stored_positions[i - 1];
				if (moving)
					break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!moving)
			break;
	}

	double go_to_position = joint_positions_[1] - joint_1_hard_stop_position;

	std::cout << "The hardstop was reached" << std::endl;

	std::cout << "Go to: " << go_to_position << std::endl;



	if (stop == 1)
		std::cout << "Ctrl-C pressed. Calibration terminated." << std::endl;
	else
		std::cout << "Done" << std::endl;
	/* stop and clear watchdog */
	daq_->stop_watchdog();
	/* disable OpenWrist */
	disable_low();
	daq_->terminate();

}

/* PUBLIC STATIC FUNCTIONS */

void OpenWrist::encoder_to_joint_positions(const int_vec &enc_counts, double_vec &joint_positions) {
	for (int i = 0; i < num_joints_; i++) {
		joint_positions[i] = 2.0 *PI* eta_[i] / (enc_resolutions_[i] * quad_factors_[i]) * enc_counts[i];
	}
}

void OpenWrist::encoder_to_joint_velocities(const double_vec &enc_counts_per_sec, double_vec &joint_velocities) {
	for (int i = 0; i < num_joints_; i++) {
		joint_velocities[i] = 2.0 *PI* eta_[i] / (enc_resolutions_[i] * quad_factors_[i]) * enc_counts_per_sec[i];
	}
}

void OpenWrist::compute_pd_control_joint_torques(const double *joint_positions, const double *joint_velocities, const double *joint_positions_ref, const double *joint_velocities_ref, double *joint_torques) {
	for (int i = 0; i < num_joints_; i++) {
		joint_torques[i] = kp_[i] * (joint_positions_ref[i] - joint_positions[i]) + kd_[i] * (joint_velocities_ref[i] - joint_velocities[i]);
	}
}

void OpenWrist::compute_gravity_compensation_joint_torques(const double_vec &joint_positions, double_vec &gravity_torques) {
	gravity_torques[0] = 1.3939*sin(q0) - 0.395038*cos(q0) + 0.351716*cos(q0)*cos(q1) + 0.0826463*cos(q0)*sin(q1) + 0.0397738*cos(q2)*sin(q0) - 0.0929844*sin(q0)*sin(q2) - 0.0929844*cos(q0)*cos(q2)*sin(q1) - 0.0397738*cos(q0)*sin(q1)*sin(q2);
	gravity_torques[1] = -8.50591e-18*sin(q0)*(4.13496e16*sin(q1) - 9.71634e15*cos(q1) + 1.09317e16*cos(q1)*cos(q2) + 4.67601e15*cos(q1)*sin(q2));
	gravity_torques[2] = 0.0929844*cos(q0)*cos(q2) + 0.0397738*cos(q0)*sin(q2) + 0.0929844*sin(q0)*sin(q1)*sin(q2) - 0.0397738*cos(q2)*sin(q0)*sin(q1);
}

void OpenWrist::compute_friction_compensation_joint_torques(const double_vec &joint_velocities, double_vec &friction_torques) {
	for (int i = 0; i < num_joints_; i++) {
		friction_torques[i] = kin_friction_[i] * tanh(10.0 * joint_velocities[i]);
	}
}

void OpenWrist::joint_torques_to_motor_torques(const double_vec &joint_torques, double_vec &motor_torques) {
	for (int i = 0; i < num_joints_; i++) {
		motor_torques[i] = eta_[i] * joint_torques[i];
	}
}

void OpenWrist::motor_torques_to_motor_currents(const double_vec &motor_torques, double_vec &motor_currents) {
	for (int i = 0; i < num_joints_; i++) {
		motor_currents[i] = (1.0 / kt_[i]) * motor_torques[i];
	}
}

void OpenWrist::limit_motor_currents(double_vec &motor_currents) {
	for (int i = 0; i < num_joints_; i++) {
		if (motor_currents[i] > current_limits_[i])
			motor_currents[i] = current_limits_[i];
		else if (motor_currents[i] < -current_limits_[i])
			motor_currents[i] = -current_limits_[i];
	}
}

void OpenWrist::motor_currents_to_output_voltages(const double_vec &motor_currents, double_vec &output_voltages) {
	for (int i = 0; i < num_joints_; i++) {
		output_voltages[i] = (1.0 / amp_gains_[i]) * motor_currents[i];
	}
}

double OpenWrist::compute_pd_control_joint_torque_single(const double joint_position, const double joint_velocity, const double joint_position_ref, const double joint_velocity_ref, const double kp, const double kd) {
	return kp * (joint_position_ref - joint_position) + kd * (joint_velocity_ref - joint_velocity);
}


double OpenWrist::compute_gravity_compensation_joint_torque_single(int joint, const double *joint_positions) {
	if (joint == 0)
		return 1.3939*sin(q0) - 0.395038*cos(q0) + 0.351716*cos(q0)*cos(q1) + 0.0826463*cos(q0)*sin(q1) + 0.0397738*cos(q2)*sin(q0) - 0.0929844*sin(q0)*sin(q2) - 0.0929844*cos(q0)*cos(q2)*sin(q1) - 0.0397738*cos(q0)*sin(q1)*sin(q2);
	if (joint == 1)
		return -8.50591e-18*sin(q0)*(4.13496e16*sin(q1) - 9.71634e15*cos(q1) + 1.09317e16*cos(q1)*cos(q2) + 4.67601e15*cos(q1)*sin(q2));
	if (joint == 2)
		return 0.0929844*cos(q0)*cos(q2) + 0.0397738*cos(q0)*sin(q2) + 0.0929844*sin(q0)*sin(q1)*sin(q2) - 0.0397738*cos(q2)*sin(q0)*sin(q1);
	return 0;
}




