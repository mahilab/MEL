#include "Daq.h"

Daq::Daq(std::string type, std::string id) :
    type_(type),
    id_(id),
    log_filename_("daq_logs\\" + type + "_" + id + "_" + get_current_data_time() + ".dat")
{
    std::string daq_log_dir_ = "daq_logs";
    boost::filesystem::path dir(daq_log_dir_.c_str());
    boost::filesystem::create_directory(dir);
    data_log_.open(log_filename_, std::ofstream::out | std::ofstream::trunc); // change trunc to app to append;
}