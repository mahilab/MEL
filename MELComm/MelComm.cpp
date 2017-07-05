#include "MelComm.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;

int receive_ints(int(&values)[16]) {
    try {
        windows_shared_memory shm(open_only, "ints_from_open_wrist", read_only);
        mapped_region region(shm, read_only);
        int(*ints_from)[16] = (int(*)[16])(region.get_address());
        for (int i = 0; i < 16; i++)
            values[i] = *ints_from[i];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int send_ints(int values[16]) {
    try {
        windows_shared_memory shm(open_only, "ints_to_open_wrist", read_write);
        mapped_region region(shm, read_write);
        int(*ints_to)[16] = (int(*)[16])(region.get_address());
        for (int i = 0; i < 16; i++)
            *ints_to[i] = values[i];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int receive_doubles(double(&values)[16]) {
    try {
        windows_shared_memory shm(open_only, "doubles_from_open_wrist", read_only);
        mapped_region region(shm, read_only);
        double(*doubles_from)[16] = (double(*)[16])(region.get_address());
        for (int i = 0; i < 16; i++)
            values[i] = *doubles_from[i];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int send_doubles(double values[16]) {
    try {
        windows_shared_memory shm(open_only, "doubles_to_open_wrist", read_write);
        mapped_region region(shm, read_write);
        double(*doubles_to)[16] = (double(*)[16])(region.get_address());
        for (int i = 0; i < 16; i++)
            *doubles_to[i] = values[i];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int get_joint_positions(double(&joint_positions)[3]) {
    try {
        windows_shared_memory shm(open_only, "open_wrist_state", read_only);
        mapped_region region(shm, read_only);
        double(*state)[16] = (double(*)[16])(region.get_address());
        joint_positions[0] = *state[1];
        joint_positions[1] = *state[2];
        joint_positions[2] = *state[3];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int get_joint_velocities(double(&joint_velocities)[3]) {
    try {
        windows_shared_memory shm(open_only, "open_wrist_state", read_only);
        mapped_region region(shm, read_only);
        double(*state)[16] = (double(*)[16])(region.get_address());
        joint_velocities[0] = *state[4];
        joint_velocities[1] = *state[5];
        joint_velocities[2] = *state[6];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int get_joint_positions_and_velocities(double(&joint_positions_and_velocities)[6]) {
    try {
        windows_shared_memory shm(open_only, "open_wrist_state", read_only);
        mapped_region region(shm, read_only);
        double(*state)[16] = (double(*)[16])(region.get_address());
        joint_positions_and_velocities[0] = *state[1];
        joint_positions_and_velocities[1] = *state[2];
        joint_positions_and_velocities[2] = *state[3];
        joint_positions_and_velocities[3] = *state[4];
        joint_positions_and_velocities[4] = *state[5];
        joint_positions_and_velocities[5] = *state[6];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int get_joint_torques(double(&joint_torques)[3]) {
    try {
        windows_shared_memory shm(open_only, "open_wrist_state", read_only);
        mapped_region region(shm, read_only);
        double(*state)[16] = (double(*)[16])(region.get_address());
        joint_torques[0] = *state[7];
        joint_torques[1] = *state[8];
        joint_torques[2] = *state[9];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}

int get_full_state(double(&full_state)[16]) {
    try {
        windows_shared_memory shm(open_only, "open_wrist_state", read_only);
        mapped_region region(shm, read_only);
        double(*state)[16] = (double(*)[16])(region.get_address());
        for (int i = 0; i < 16; i++)
            full_state[i] = *state[i];
        return 1;
    }
    catch (interprocess_exception &ex) {
        return 0;
    }
}