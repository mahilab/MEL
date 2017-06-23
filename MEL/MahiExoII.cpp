#include "MahiExoII.h"

MahiExoII::MahiExoII(Daq* e0_daq, uint e0_chan, Daq* e1_daq, uint e1_chan, Daq* ao0_daq, uint ao0_chan, Daq* ao1_daq, uint ao1_chan, Daq* ai0_daq, uint ai0_chan, Daq* ai1_daq, uint ai1_chan, Daq* do0_daq, uint do0_chan, Daq* do1_daq, uint do1_chan) : 
    e0_daq_(e0_daq), e0_chan_(e0_chan),
    e1_daq_(e1_daq), e1_chan_(e1_chan),
    ao0_daq_(ao0_daq), ao0_chan_(ao0_chan),
    ao1_daq_(ao1_daq), ao1_chan_(ao1_chan),
    ai0_daq_(ai0_daq), ai0_chan_(ai0_chan),
    ai1_daq_(ai1_daq), ai1_chan_(ai1_chan),
    do0_daq_(do0_daq), do0_chan_(do0_chan),
    do1_daq_(do1_daq), do1_chan_(do1_chan),
    Robot(2)
{

}


double_vec Robot::get_joint_positions() {
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        joint_positions_[it - joints_.begin()] = it->get_position();
    }
    return joint_positions_;
}