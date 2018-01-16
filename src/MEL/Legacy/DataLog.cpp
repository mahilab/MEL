#include <MEL/Utility/DataLog.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <fstream>
#include <thread>
#include <algorithm>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

DataLog::DataLog(bool autosave, size_t max_rows) :
    max_rows_(max_rows),
    num_cols_(0),
    row_index_(0),
    log_saved_(true),
    saving_(false),
    autosave_(autosave)
{
}

DataLog::~DataLog() {
    if (!log_saved_ && row_index_ > 0 && autosave_) {
        save_data("log", "data_log_backups", true);
    }
    if (saving_) {
        wait_for_save();
    }
}

void DataLog::add_col(const std::string& column_name) {
    column_names_.push_back(column_name);
    data_.push_back(std::vector<double>(max_rows_, 0));
    log_saved_ = false;
    num_cols_ += 1;
}

void DataLog::add_cols(const std::vector<std::string>& column_names) {
    for (std::size_t i = 0; i  < column_names.size(); ++i)
        add_col(column_names[i]);
}

void DataLog::add_row(const std::vector<double>& row_data) {
    if (row_index_ == max_rows_)
        double_rows();
    for (int i = 0; i < std::min(num_cols_, row_data.size()); i++) {
        data_[i][row_index_] = row_data[i];
    }
    log_saved_ = false;
    row_index_ += 1;
}

uint32 DataLog::get_col_count() {
    return static_cast<uint32>(num_cols_);
}

uint32 DataLog::get_row_count() {
    return row_index_;
}

std::vector<double> DataLog::get_col(uint32 index) {
    if (index < num_cols_)
        return std::vector<double>(data_[index].begin(), data_[index].begin() + row_index_);
    else
        return std::vector<double>();
}

std::vector<double> DataLog::get_col(const std::string& column_name) {
    auto result = std::find(column_names_.begin(), column_names_.end(), column_name);
    if (result != column_names_.end()) {
        return std::vector<double>(data_[result - column_names_.begin()].begin(), data_[result - column_names_.begin()].begin() + row_index_);
    }
    else {
        return std::vector<double>();
    }
}

std::vector<double> DataLog::get_row(uint32 index) {
    std::vector<double> row(num_cols_);
    if (index < row_index_) {
        for (int i = 0; i < num_cols_; ++i)
            row[i] = data_[i][index];
    }
    return row;
}

void DataLog::save_data(std::string filename, std::string directory, bool timestamp) {
    saving_ = true;
    std::string full_filename;
    if (timestamp) {
        full_filename = directory + get_path_slash() + filename + "_" + get_ymdhms() + ".csv";
    }
    else {
        full_filename = directory + get_path_slash() + filename + ".csv";
    }
    create_directory(directory);
    print("Saving DataLog to <" + full_filename + ">.");

    std::thread t(&DataLog::save_thread_function, this, full_filename, column_names_, data_, row_index_, num_cols_);
    t.detach();

    log_saved_ = true;
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

void DataLog::wait_for_save() {
    print("Waiting for DataLog to finish saving ... ", false);
    while (saving_) {
        sleep(milliseconds(10));
    }
    print("Done");
}

void DataLog::double_rows() {
    print("WARNING: DataLog max rows exceeded. Automatically doubling size. Increase max_rows in the constructor to avoid delays.");
    max_rows_ *= 2;
    for (auto it = data_.begin(); it != data_.end(); ++it) {
        it->resize(max_rows_, 0);
    }
}

void DataLog::save_thread_function(std::string full_filename, std::vector<std::string> column_names, std::vector<std::vector<double>> data, uint32 num_rows, uint32 num_cols) {
    std::ofstream file_stream;
    file_stream.precision(12);
    file_stream.open(full_filename, std::ofstream::out | std::ofstream::trunc);
    for (auto it = column_names.begin(); it != column_names.end(); ++it) {
        file_stream << *it << ",";
    }
    file_stream << std::endl;
    for (uint32 i = 0; i < num_rows; i++) {
        for (size_t j = 0; j < num_cols; j++) {
            file_stream << data[j][i] << ",";
        }
        file_stream << std::endl;
    }
    file_stream.close();
    saving_ = false;
}

} // namespace mel
