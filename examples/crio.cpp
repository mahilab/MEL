#include <MEL/Utility/Console.hpp>
#include <MEL/Communications/MelNet.hpp>
#include "MEL/Daq/NI/NiFpga_quadrature.h"
#include <MEL/Utility/Clock.hpp>
#include <vector>

using namespace mel;

int main(int argc, char *argv[]) {

    if (argc > 1) {
        std::string remote_host = argv[1];

        MelNet melnetA(55001, 55002, IpAddress(remote_host), false);

        NiFpga_Session session;
        NiFpga_Status status = NiFpga_Initialize();
        NiFpga_MergeStatus(&status,
         NiFpga_Open("/home/admin/examples/cpp/NiFpga_quadrature.lvbitx",
             NiFpga_quadrature_Signature, "RIO0", 0, &session));
        int count;
        std::vector<double> position(1);

        while(NiFpga_IsNotError(status)) {
            NiFpga_MergeStatus(&status,
                NiFpga_ReadI32(session, NiFpga_quadrature_IndicatorI32_count, &count));
            position[0] = (double)count / 2000 * 360;
            if (melnetA.receive_message() == "Feed Me") {
                melnetA.send_data(position);
            }
            // print(position[0]);
        }
    }
    return 0;
}

