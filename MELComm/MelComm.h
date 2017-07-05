/* Evan Pezent (epezent@rice.edu, evanpezent.com) */
/* 05/2017 */

/* OpenWristShare API enables ultra-fast, bidirectional communication between
* the main OpenWrist executable and any external application or code that will
* import a C/C++ dynamically linked library (.dll). This is accomplished
* through five named shared memory maps maintained by the main OpenWrist
* executable, while this API exposes several functions for receiving and
* sending values through the shared memory maps.
*
* User defined integers may be received and sent using the functions
* ReceiveInts and SendInts, respectively, while doubles may be received and
* sent using the functions ReceiveDoubles and SendDoubles, respectively.
* What you choose to send through these channels is up to you, and you will
* need to implement their functionality in your applilcation as well as the
* main OpenWrist executbale. It may be benificial to think of the integer
* channels as analogous to DI/DO channels which can be used to send logic (0,1)
* and the double channels as analogous to AI/AO channels which can be used to
* send precesise values. As an example, you may want to send data over a
* double channel, and notify the other program when data has been updated by
* bit-flipping the corresponding int channel.
*
* Note that all Receive/Get functions expect a reference to an array of the
* correct size, which will serve as an output buffer for your values. Send
* functions also expect an array of the correct size, but the array is not
* modified by the function. Each function returns either 1 to indicate
* success or 0 to indicate failure.
*
* OpenWrist state variables such as joint position, joint velocity, joint
* torques, motor currents, etc. are ALWAYS retreivable through the GetX
* functions. It is most efficient to uses as few functions as possible,
* e.g. call GetJointPositionsAndVelocities instead of GetJointPositions and
* GetJointVelocities, etc. The full state information for the OpenWrist can
* be obtained by calling GetFullState. The ordering of the output array is
* as follows:
*
* state[0]  = elapsed controller time  [s]
* state[1]  = joint 0 position         [rad]
* state[2]  = joint 1 position         [rad]
* state[3]  = joint 2 position         [rad]
* state[4]  = joint 0 velocity         [rad/s]
* state[5]  = joint 1 velocity         [rad/s]
* state[6]  = joint 2 velocity         [rad/s]
* state[7]  = joint 0 commanded torque [N-m]
* state[8]  = joint 1 commanded torque [N-m]
* state[9]  = joint 2 commanded torque [N-m]
* state[10] = user defined
* state[11] = user defined
* state[12] = user defined
* state[13] = user defined
* state[14] = user defined
* state[15] = user defined
*/

#ifdef _WIN32
#ifdef MELCOMM_EXPORTS
#	define MELCOMM_API __declspec(dllexport)
#else
#	define MELCOMM_API __declspec(dllimport)
#endif
#elif
#	define MELCOMM_API
#endif

/* generic channels to send/receive integers and doubles */

extern "C" {
    MELCOMM_API int receive_ints(int(&values)[16]);
}

extern "C" {
    MELCOMM_API int send_ints(int values[16]);
}

extern "C" {
    MELCOMM_API int receive_doubles(double(&values)[16]);
}

extern "C" {
    MELCOMM_API int send_doubles(double values[16]);
}

/* channels to receive various OpenWrist states */

extern "C" {
    MELCOMM_API int get_joint_positions(double(&joint_positions)[3]);
}

extern "C" {
    MELCOMM_API int get_joint_velocities(double(&joint_velocities)[3]);
}

extern "C" {
    MELCOMM_API int get_joint_positions_and_velocities(double(&joint_positions_and_velocities)[6]);
}

extern "C" {
    MELCOMM_API int get_joint_torques(double(&joint_torques)[3]);
}

extern "C" {
    MELCOMM_API int get_full_state(double(&full_state)[16]);
}

