#include <math.h>

#define QBADMIN_VERSION "v6.1.0"

#define NUM_OF_MOTORS 2
#define NUM_OF_EMGS 2
//#define PI 3.14159265359

#define DEFAULT_RESOLUTION 1
#define DEFAULT_INF_LIMIT -15000
#define DEFAULT_SUP_LIMIT 15000
#define BROADCAST_ID 1
#define DEFAULT_PID_P 0.1
#define DEFAULT_PID_I 0
#define DEFAULT_PID_D 0.8
#define DEFAULT_INCREMENT 1 //in degrees
#define DEFAULT_STIFFNESS 30 //in degrees
#define DEFAULT_MAX_EXCURSION 330 //in degrees

#define ZERO 0
#define MAX_FORWARD_STIFFNESS  32767
#define MAX_REVERSE_STIFFNESS -32768

#define DEG_TICK_MULTIPLIER (65536.0 / (360.0 * (pow(2, DEFAULT_RESOLUTION))))

#define BAUD_RATE_T_2000000	0
#define BAUD_RATE_T_460800	1

#define SIN_FILE "./../conf_files/sin.conf"
#define MOTOR_FILE "./../conf_files/motor.conf"
#define QBMOVE_FILE "./../conf_files/qbmove.conf"
#define QBBACKUP_FILE "./../conf_files/qbbackup.conf"
#define QBMOVE_FILE_BR "./../conf_files/qbmoveBR.conf"
#define EMG_SAVED_VALUES "./../emg_values.csv"			///< Default location where the emg sensors values are saved
