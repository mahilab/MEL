#include "mahiexoii_util.h"



double moving_set_point(double init_pos, double goal_pos, double init_time, double time, double speed) { 

    double ref_pos = init_pos + (goal_pos - init_pos) * mel::saturate((time - init_time) * speed / abs(goal_pos - init_pos), 1, 0);

    return ref_pos;

}

