#include "DataLog.h"
#include <boost/filesystem.hpp>
#include <fstream>


namespace mel {

    DataLog::DataLog() :
        DataLog("data_log", true, 1000000) {}

    DataLog::DataLog(std::string name, bool autosave, size_t max_rows) :
        name_(name),
        max_rows_(max_rows),
        num_cols_(0),
        row_index_(0),
        log_saved_(true),
        autosave_(autosave)
    {
    }

    DataLog::~DataLog() {
        if (!log_saved_ && row_index_ > 0 && autosave_) {
            save_data(name_, "data_log_backups", true);
        }
    }

    DataLog& DataLog::add_col(std::string column_name) {
        column_names_.push_back(column_name);
        data_.push_back(std::vector<double>(max_rows_, 0));
        log_saved_ = false;
        num_cols_ += 1;
        return *this;
    }

    void DataLog::add_row(std::vector<double> row_data) {
        if (row_index_ == max_rows_)
            double_rows();
        for (int i = 0; i < std::min(num_cols_, row_data.size()); i++) {
            data_[i][row_index_] = row_data[i];
        }
        log_saved_ = false;
        row_index_ += 1;
    }

    void DataLog::double_rows() {
        print("WARNING: DataLog " + namify(name_) + " max rows exceeded. Automatically doubling size. Increase max_rows in the constructor to avoid delays.");
        max_rows_ *= 2;
        for (auto it = data_.begin(); it != data_.end(); ++it) {
            it->resize(max_rows_, 0);
        }
    }

    void DataLog::save_data(std::string filename, std::string directory, bool timestamp) {
        std::string full_filename = directory + "\\" + filename + "_" + get_ymdhms() + ".csv";
        boost::filesystem::path dir(directory.c_str());
        boost::filesystem::create_directories(dir);
        std::ofstream data_log;
        std::cout << "Saving DataLog " + namify(name_) + " to <" << full_filename << "> ... ";
        data_log.open(full_filename, std::ofstream::out | std::ofstream::trunc);
        for (auto it = column_names_.begin(); it != column_names_.end(); ++it) {
            data_log << *it << ",";
        }
        data_log << std::endl;
        for (uint32 i = 0; i < row_index_; i++) {
            for (size_t j = 0; j < num_cols_; j++) {
                data_log << data_[j][i] << ",";
            }
            data_log << std::endl;
        }
        log_saved_ = true;
        data_log.close();
        std::cout << "Done" << std::endl;
    }

    void DataLog::clear_data() {
        data_ = std::vector<std::vector<double>>();
        for (int i = 0; i < column_names_.size(); ++i)
            data_.push_back(std::vector<double>(max_rows_, 0));
        log_saved_ = true;
        row_index_ = 0;
    }

    void DataLog::save_and_clear_data(std::string filename, std::string directory, bool timestamp) {
        save_data(filename, directory, timestamp);
        clear_data();
    }
}