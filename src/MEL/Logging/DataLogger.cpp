#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/System.hpp>
#include <sstream>
#include <fstream>
#include <thread>

#if defined(__linux__) ||  defined(__APPLE__)
#include <unistd.h>
#endif

namespace mel {


bool DataLogger::write_to_csv(const std::vector<std::string> &header, const std::string &filename, const std::string& directory, bool timestamp) {
	std::string filename_no_ext;
	std::string file_ext;
	split_file_name(filename.c_str(), filename_no_ext, file_ext);
	if (file_ext.empty()) {
		file_ext = "csv";
	}
	std::string full_filename;
	if (timestamp) {
		Timestamp stamp;
		full_filename = directory + get_path_slash() + filename_no_ext + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext;
	}
	else {
		full_filename = directory + get_path_slash() + filename_no_ext + "." + file_ext;
	}
	LOG(Verbose) << "Writing header to " << full_filename;
	create_directory(directory);
	_unlink(full_filename.c_str());
	File file;
	if (!header.empty()) {
		file.open(full_filename.c_str());
		std::ostringstream ss;
		for (std::size_t i = 0; i < header.size() - 1; i++) {
			ss << header[i] << ",";
		}
		ss << header.back() << "\r\n";
		file.write(ss.str());
		file.close();
	}
	return true;
}

bool DataLogger::write_to_csv(const std::vector<std::vector<double>> &data, const std::string &filename, const std::string& directory, bool timestamp) {
	std::string filename_no_ext;
	std::string file_ext;
	split_file_name(filename.c_str(), filename_no_ext, file_ext);
	if (file_ext.empty()) {
		file_ext = "csv";
	}
	std::string full_filename;
	if (timestamp) {
		Timestamp stamp;
		full_filename = directory + get_path_slash() + filename_no_ext + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext;
	}
	else {
		full_filename = directory + get_path_slash() + filename_no_ext + "." + file_ext;
	}
	LOG(Verbose) << "Saving data to " << full_filename;
	create_directory(directory);
	_unlink(full_filename.c_str());
	File file;
	file.open(full_filename.c_str());

	for (std::size_t i = 0; i < data.size(); i++) {
		std::ostringstream ss;
		ss << std::setprecision(6);
		for (size_t j = 0; j < data[i].size() - 1; ++j) {
			ss << data[i][j] << ",";
		}
		ss << data[i].back() << "\r\n";
		file.write(ss.str());
	}
	file.close();
	return true;
}

bool DataLogger::write_to_csv(const Table &data, const std::string &filename, const std::string& directory, bool timestamp) {
	std::string new_filename = filename.empty() ? (data.name().empty() ? "mel_table" : data.name()) : filename;
	std::string filename_no_ext;
	std::string file_ext;
	split_file_name(new_filename.c_str(), filename_no_ext, file_ext);
	if (file_ext.empty()) {
		file_ext = "csv";
	}
	std::string full_filename;
	if (timestamp) {
		Timestamp stamp;
		full_filename = directory + get_path_slash() + filename_no_ext + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext;
	}
	else {
		full_filename = directory + get_path_slash() + filename_no_ext + "." + file_ext;
	}
	LOG(Verbose) << "Saving data to " << full_filename;
	create_directory(directory);
	_unlink(full_filename.c_str());
	File file;
	file.open(full_filename.c_str());
	std::ostringstream oss;
	oss << std::setprecision(6);
	oss << make_csv_header(data);
	if (!data.empty()) {
		for (std::size_t i = 0; i < data.col_count() - 1; i++) {
			oss << data.get_col_name(i) << ",";
		}
		oss << data.get_col_name(data.col_count() - 1) << "\r\n";
		for (std::size_t i = 0; i < data.row_count(); i++) {
			for (size_t j = 0; j < data.col_count() - 1; ++j) {
				oss << data(i, j) << ",";
			}
			oss << data(i, data.col_count() - 1) << "\r\n";
		}
	}
	file.write(oss.str());
	file.close();
	return true;
}

bool DataLogger::write_to_csv(const std::vector<Table> &data, const std::string &filename, const std::string& directory, bool timestamp) {
	std::string new_filename = filename.empty() ? ( data.empty() ? "empty_table" : ( data[0].name().empty() ? "mel_table" : data[0].name() ) ) : filename;
	std::string filename_no_ext;
	std::string file_ext;
	split_file_name(filename.c_str(), filename_no_ext, file_ext);
	if (file_ext.empty()) {
		file_ext = "csv";
	}
	std::string full_filename;
	if (timestamp) {
		Timestamp stamp;
		full_filename = directory + get_path_slash() + filename_no_ext + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext;
	}
	else {
		full_filename = directory + get_path_slash() + filename_no_ext + "." + file_ext;
	}
	LOG(Verbose) << "Saving data to " << full_filename;
	create_directory(directory);
	_unlink(full_filename.c_str());
	File file;
	file.open(full_filename.c_str());
	std::ostringstream oss;
	oss << std::setprecision(6);
	for (std::size_t k = 0; k < data.size(); ++k) {
		oss << make_csv_header(data[k]);
		if (!data[k].empty()) {
			for (std::size_t i = 0; i < data[k].col_count() - 1; i++) {
				oss << data[k].get_col_name(i) << ",";
			}
			oss << data[k].get_col_name(data[k].col_count() - 1) << "\r\n";
		}
		for (std::size_t i = 0; i < data[k].row_count(); i++) {
			for (size_t j = 0; j < data[k].col_count() - 1; ++j) {
				oss << data[k](i, j) << ",";
			}
			oss << data[k](i, data[k].col_count() - 1) << "\r\n";
		}
		oss << "\r\n";
	}
	file.write(oss.str());
	file.close();
	return true;
}

bool DataLogger::read_from_csv(std::vector<std::vector<double>> &data, const std::string &filename, const std::string& directory) {
	std::string filename_no_ext;
	std::string file_ext;
	split_file_name(filename.c_str(), filename_no_ext, file_ext);
	if (file_ext.empty()) {
		file_ext = "csv";
	}
	std::string full_filename;
	full_filename = directory + get_path_slash() + filename_no_ext + "." + file_ext;
	LOG(Verbose) << "Reading data from " << full_filename;
	std::vector<std::vector<double>> output;
	std::ifstream input(full_filename);
	input.precision(6);
	if (!input.is_open()) {
		LOG(Warning) << "File not found in DataLogger::read_from_csv().";
		return false;
	}
	data.clear();
	std::string csv_line;
	while (std::getline(input, csv_line)) {
		std::istringstream csv_stream(csv_line);
		std::vector<double> row;
		std::string value_str;
		double value;
		while (std::getline(csv_stream, value_str, ',')) {
			std::istringstream ss(value_str);
			ss >> value;
			row.push_back(value);
		}
		data.push_back(row);
	}
	return true;
}

bool DataLogger::read_from_csv(Table &data, const std::string &filename, const std::string& directory) {
	std::string filename_no_ext;
	std::string file_ext;
	split_file_name(filename.c_str(), filename_no_ext, file_ext);
	if (file_ext.empty()) {
		file_ext = "csv";
	}
	std::string full_filename;
	full_filename = directory + get_path_slash() + filename_no_ext + "." + file_ext;
	LOG(Verbose) << "Reading data from " << full_filename;
	std::vector<std::vector<double>> output;
	std::ifstream input(full_filename);
	input.precision(6);
	if (!input.is_open()) {
		LOG(Warning) << "File not found in DataLogger::read_from_csv().";
		return false;
	}
	bool is_table = false;
	data.clear();
	std::string csv_line;
	std::string el_str;
	double value;
	std::vector<std::string> name_row;
	std::vector<double> value_row;
	while (std::getline(input, csv_line)) {
		std::istringstream iss(csv_line);
		if (std::getline(iss, el_str, ',')) {
			if (el_str.compare(Table::table_id) == 0) {
				if (!parse_csv_header(data, csv_line)) {
					LOG(Warning) << "Table header in " << full_filename << " could not be parsed.";
					return false;
				}
				is_table = true;
				break;
			}
		}
	}
	if (!is_table) {
		LOG(Warning) << "File does not contain valid MEL::Table header.";
		return false;
	}
	if (std::getline(input, csv_line)) {
		std::istringstream iss(csv_line);
		name_row.clear();
		while (std::getline(iss, el_str, ',')) {
			name_row.push_back(el_str);
		}
		data.set_col_names(name_row);
	}
	while (std::getline(input, csv_line)) {
		std::istringstream iss(csv_line);
		value_row.clear();
		while (std::getline(iss, el_str, ',')) {
			std::istringstream el_iss(el_str);
			el_iss >> value;
			value_row.push_back(value);
		}
		data.push_back_row(value_row);
	}
	return true;
}

bool DataLogger::read_from_csv(std::vector<Table> &data, const std::string &filename, const std::string& directory) {
	std::string filename_no_ext;
	std::string file_ext;
	split_file_name(filename.c_str(), filename_no_ext, file_ext);
	if (file_ext.empty()) {
		file_ext = "csv";
	}
	std::string full_filename;
	full_filename = directory + get_path_slash() + filename_no_ext + "." + file_ext;
	LOG(Verbose) << "Reading data from " << full_filename;
	std::vector<std::vector<double>> output;
	std::ifstream input(full_filename);
	input.precision(6);
	if (!input.is_open()) {
		LOG(Warning) << "File not found in DataLogger::read_from_csv().";
		return false;
	}
	data.clear();
	bool is_table = false;
	bool scan_for_table = true;
	bool new_table = false;
	std::size_t table_index = 0;
	std::string csv_line;
	std::string el_str;
	double value;
	std::vector<std::string> header_row;
	std::vector<double> value_row;
	while (std::getline(input, csv_line)) {
		std::istringstream iss(csv_line);
		if (scan_for_table) {
			if (std::getline(iss, el_str, ',')) {
				if (el_str.compare(Table::table_id) == 0) {
					data.emplace_back();
					if (!parse_csv_header(data[table_index], csv_line)) {
						LOG(Warning) << "Table header in " << full_filename << " could not be parsed.";
						return false;
					}
					is_table = true;
					scan_for_table = false;
					new_table = true;

				}
			}
		}
		else {
			if (new_table) {
				//std::istringstream csv_stream(csv_line);
				//std::vector<std::string> header_row;
				//std::string value_str;
				header_row.clear();
				while (std::getline(iss, el_str, ',')) {
					header_row.push_back(el_str);
				}
				data[table_index].set_col_names(header_row);
				new_table = false;
			}
			//std::istringstream csv_stream(csv_line);
			//std::vector<double> row;
			//std::string value_str;
			else {
				if (!csv_line.empty()) {
					value_row.clear();
					while (std::getline(iss, el_str, ',')) {
						std::istringstream el_iss(el_str);
						el_iss >> value;
						value_row.push_back(value);
					}
					data[table_index].push_back_row(value_row);
				}
				else {
					scan_for_table = true;
					table_index++;
				}
			}
		}
	}
	if (!is_table) {
		LOG(Warning) << "File does not contain valid MEL::Table header.";
		return false;
	}


	//data.clear();
	//data.emplace_back();
	//
	//bool new_table = true;
	//std::string csv_line;
	//while (std::getline(input, csv_line)) {
	//	if (csv_line.empty()) {
	//		new_table = true;
	//		data.emplace_back();
	//		table_index++;
	//	}
	//	if (new_table) {
	//		std::istringstream csv_stream(csv_line);
	//		std::vector<std::string> header_row;
	//		std::string value_str;
	//		while (std::getline(csv_stream, value_str, ',')) {
	//			header_row.push_back(value_str);
	//		}
	//		data[table_index].set_header(header_row);
	//		new_table = false;
	//	}
	//	std::istringstream csv_stream(csv_line);
	//	std::vector<double> row;
	//	std::string value_str;
	//	double value;
	//	while (std::getline(csv_stream, value_str, ',')) {
	//		std::istringstream ss(value_str);
	//		ss >> value;
	//		row.push_back(value);
	//	}
	//	data[table_index].push_back_row(row);
	//}

	return true;
}

std::string DataLogger::make_csv_header(const Table &table) {
	std::ostringstream oss;
	oss << Table::table_id << ",";
	oss << "name" << "=" << table.name() << ",";
	oss << "n_rows" << "=" << table.row_count() << ",";
	oss << "n_cols" << "=" << table.col_count() << "\r\n";
	return oss.str();
}

bool DataLogger::parse_csv_header(Table &table, const std::string &header) {
	std::istringstream iss(header);
	std::string el_str;
	std::string param_name_str;
	std::string param_str;
	if (std::getline(iss, el_str, ',')) {
		if (el_str.compare(Table::table_id) != 0) {
			return false;
		}
		while (std::getline(iss, el_str, ',')) {
			std::istringstream el_iss(el_str);
			if (std::getline(el_iss, param_name_str, '=')) {
				if (param_name_str.compare("name") == 0) {
					std::getline(el_iss, param_str);
					table.rename(param_str);
				}
				else if (param_name_str.compare("n_rows") == 0) {
					std::size_t n_rows;
					el_iss >> n_rows;

				}
				else if (param_name_str.compare("n_cols") == 0) {
					std::size_t n_cols;
					el_iss >> n_cols;
				}
				else {
					return false;
				}
			}
		}
	}
	else {
		return false;
	}
	return true;
}


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
        split_file_name(filename.c_str(), filename_no_ext_, file_ext_);
        if (file_ext_.empty()) {
            file_ext_ = "csv";
        }
        std::string full_filename;
        if (timestamp) {
            Timestamp stamp;
            full_filename = directory + get_path_slash() + filename_no_ext_ + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext_;
        }
        else
            full_filename = directory + get_path_slash() + filename_no_ext_ + "." + file_ext_;
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
        split_file_name(filename.c_str(), filename_no_ext_, file_ext_);
        if (file_ext_.empty()) {
            file_ext_ = "csv";
        }
        std::string full_filename;
        if (timestamp) {
            Timestamp stamp;
            full_filename = directory + get_path_slash() + filename_no_ext_ + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext_;
        }
        else
            full_filename = directory + get_path_slash() + filename_no_ext_ + "." + file_ext_;
        LOG(Verbose) << "Opening data file " << full_filename;
        _unlink(full_filename.c_str());
        file_size_ = file_.open(full_filename.c_str());
        file_opened_ = true;
    }
    else {
        LOG(Warning) << "DataLogger file not opened. Can only use open() on DataLogger with WriterType::Immediate.";
    }
}

void DataLogger::reopen(const std::string& filename, const std::string& directory, bool timestamp) {
    if (writer_type_ == WriterType::Immediate) {
        split_file_name(filename.c_str(), filename_no_ext_, file_ext_);
        if (file_ext_.empty()) {
            file_ext_ = "csv";
        }
        std::string full_filename;
        if (timestamp) {
            Timestamp stamp;
            full_filename = directory + get_path_slash() + filename_no_ext_ + "_" + stamp.yyyy_mm_dd_hh_mm_ss() + "." + file_ext_;
        }
        else
            full_filename = directory + get_path_slash() + filename_no_ext_ + "." + file_ext_;
        LOG(Verbose) << "Reopening data file " << full_filename;
        file_size_ = file_.open(full_filename.c_str());
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
    _unlink(full_filename.c_str());
    file_.open(full_filename.c_str());
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
