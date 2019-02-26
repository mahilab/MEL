#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/File.hpp>
#include <sstream>
#include <fstream>
#include <thread>

namespace mel {

DataLogger::DataLogger(WriterType writer_type, bool autosave, size_t max_rows) :
    writer_type_(writer_type),
    file_size_(),
    file_opened_(false),
    autosave_(autosave),
    log_saved_(true),
    saving_(false),
    row_count_(0),
    col_count_(0),
    max_rows_(max_rows),
    format_(DataFormat::Default),
    precision_(6) {
    if (writer_type == WriterType::Buffered) {
        data_buffer_.reserve(max_rows);
    }
}

DataLogger::~DataLogger() {
    if (autosave_ && row_count_ > 0 && !log_saved_ && !saving_) {
        save_data("log", "autosaved_logs", true);
    }
    wait_for_save();
}

void DataLogger::buffer(const std::vector<double>& data_record) {
    if (writer_type_ == WriterType::Buffered) {
        Lock lock(mutex_);
        if (row_count_ == max_rows_)
            double_rows();
        data_buffer_.push_back(data_record);
        log_saved_ = false;
        row_count_ += 1;
    }
    else {
        LOG(Warning) << "Nothing written to DataLogger. Can only use buffer() on DataLogger with WriterType::Buffered.";
    }
}

void DataLogger::save_data(const std::string& filename, const std::string& directory, bool timestamp) {
    if (writer_type_ == WriterType::Buffered) {
        saving_ = true;
        split_filename(filename.c_str(), filename_no_ext_, file_ext_);
        if (file_ext_.empty()) {
            file_ext_ = "csv";
        }
        std::string full_filename;
        if (timestamp) {
            Timestamp stamp;
            full_filename = directory + get_separator() + filename_no_ext_ + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext_;
        }
        else
            full_filename = directory + get_separator() + filename_no_ext_ + "." + file_ext_;
        LOG(Verbose) << "Saving data to " << full_filename;
        std::thread t(&DataLogger::save_thread_func, this, full_filename, directory, data_buffer_);
        t.detach();
    }
    else {
        LOG(Warning) << "Nothing written to DataLogger. Can only use save_data() on DataLogger with WriterType::Buffered.";
    }
}

void DataLogger::clear_data() {
    if (writer_type_ == WriterType::Buffered) {
        data_buffer_.clear();
        data_buffer_.reserve(max_rows_);
        log_saved_ = true;
        row_count_ = 0;
    }
    else {
        LOG(Warning) << "No effect on DataLogger. Can only use clear_data() on DataLogger with WriterType::Buffered.";
    }
}

void DataLogger::save_and_clear_data(std::string filename, std::string directory, bool timestamp) {
    if (writer_type_ == WriterType::Buffered) {
        save_data(filename, directory, timestamp);
        clear_data();
    }
    else {
        LOG(Warning) << "Nothing written to DataLogger. Can only use save_and_clear_data() on DataLogger with WriterType::Buffered.";
    }
}

void DataLogger::wait_for_save() {
    if (saving_) {
        Lock lock(mutex_);
    }
}


void DataLogger::write(const std::vector<double>& data_record) {
    if (writer_type_ == WriterType::Immediate) {
        Lock lock(mutex_);

        if (!file_opened_) {
            LOG(Warning) << "Nothing written to DataLogger. Must first use open() on DataLogger with WriterType::Immediate.";
            return;
        }

        if (file_size_ == 0) {
            write_header();
        }

        int bytes_written =
            file_.write(format(data_record));
        if (bytes_written > 0) {
            file_size_ += bytes_written;
            row_count_ += 1;
        }
    }
    else {
        LOG(Warning) << "Nothing written to DataLogger. Can only use write() on DataLogger with WriterType::Immediate.";
    }
}


void DataLogger::open(const std::string& filename, const std::string& directory, bool timestamp) {
    if (writer_type_ == WriterType::Immediate) {
        split_filename(filename.c_str(), filename_no_ext_, file_ext_);
        if (file_ext_.empty()) {
            file_ext_ = "csv";
        }
        std::string full_filename;
        if (timestamp) {
            Timestamp stamp;
            full_filename = directory + get_separator() + filename_no_ext_ + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext_;
        }
        else
            full_filename = directory + get_separator() + filename_no_ext_ + "." + file_ext_;
        LOG(Verbose) << "Opening data file " << full_filename;
        file_size_ = file_.open(full_filename, WriteMode::Truncate);
        file_opened_ = true;
    }
    else {
        LOG(Warning) << "DataLogger file not opened. Can only use open() on DataLogger with WriterType::Immediate.";
    }
}

void DataLogger::reopen(const std::string& filename, const std::string& directory, bool timestamp) {
    if (writer_type_ == WriterType::Immediate) {
        split_filename(filename.c_str(), filename_no_ext_, file_ext_);
        if (file_ext_.empty()) {
            file_ext_ = "csv";
        }
        std::string full_filename;
        if (timestamp) {
            Timestamp stamp;
            full_filename = directory + get_separator() + filename_no_ext_ + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext_;
        }
        else
            full_filename = directory + get_separator() + filename_no_ext_ + "." + file_ext_;
        LOG(Verbose) << "Reopening data file " << full_filename;
        file_size_ = file_.open(full_filename, WriteMode::Append);
        file_opened_ = true;
    }
    else {
        LOG(Warning) << "DataLogger file not opened. Can only use reopen() on DataLogger with WriterType::Immediate.";
    }
}

void DataLogger::close() {
    if (writer_type_ == WriterType::Immediate) {
        file_.close();
        file_opened_ = false;
    }
    else {
        LOG(Warning) << "DataLogger file not opened. Can only use open() on DataLogger with WriterTyp::Immediate.";
    }
}

void DataLogger::set_writer_type(WriterType writer_type) {
    switch (writer_type) {
    case WriterType::Buffered:
        if (writer_type_ != WriterType::Buffered) {
            if (file_opened_) {
                file_.close();
            }
            writer_type_ = WriterType::Buffered;

            data_buffer_.clear();
            data_buffer_.reserve(max_rows_);
            log_saved_ = true;
            row_count_ = 0;
        }
        break;
    case WriterType::Immediate:
        if (writer_type_ != WriterType::Immediate) {
            writer_type_ = WriterType::Immediate;
        }
        break;
    }
}

void DataLogger::set_header(const std::vector<std::string>& header) {
    header_ = header;
    col_count_ = header.size();
}

void DataLogger::set_record_format(DataFormat data_format, std::size_t precision) {
    format_ = data_format;
    precision_ = precision;
}

std::size_t DataLogger::get_row_count() const {
    return row_count_;
}

std::size_t DataLogger::get_col_count() const {
    return col_count_;
}

std::vector<double> DataLogger::get_row(std::size_t row) {
    return data_buffer_[row];
}

std::vector<double> DataLogger::get_col(std::size_t col) {
    std::vector<double> col_data(row_count_);
    for (std::size_t i = 0; i < col_data.size(); ++i) {
        col_data[i] = data_buffer_[i][col];
    }
    return col_data;
}



std::string DataLogger::format(const std::vector<double>& data_record) {
    std::ostringstream ss;
    ss << std::setprecision(precision_);
    if (format_ == DataFormat::Fixed)
        ss << std::fixed;
    else if (format_ == DataFormat::Scientific)
        ss << std::scientific;
    for (size_t i = 0; i < data_record.size() - 1; ++i) {
        ss << data_record[i] << ",";
    }
    ss << data_record[data_record.size() - 1] << "\r\n";
    return ss.str();
}

void DataLogger::write_header() {
    if (!header_.empty()) {
        std::ostringstream ss;
        for (size_t i = 0; i < header_.size() - 1; ++i) {
            ss << header_[i] << ",";
        }
        ss << header_[header_.size() - 1] << "\r\n";
        file_.write(ss.str());
    }
}


void DataLogger::double_rows() {
    LOG(Warning) << "DataLog max rows exceeded. Automatically doubling size. Increase max_rows in the constructor to avoid delays.";
    max_rows_ *= 2;
    data_buffer_.reserve(max_rows_);
}

void DataLogger::save_thread_func(const std::string& full_filename, const std::string& directory, std::vector<std::vector<double>> temp_data) {
    Lock lock(mutex_);
    create_directory(directory);
    file_.open(full_filename, WriteMode::Truncate);
    file_opened_ = true;
    write_header();
    for (std::size_t i = 0; i < temp_data.size(); i++) {
        file_.write(format(temp_data[i]));
    }
    file_.close();
    file_opened_ = false;
    log_saved_ = true;
    saving_ = false;
}

} // namespace mel
