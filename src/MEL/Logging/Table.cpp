#include <MEL/Logging/Table.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/File.hpp>
#include <sstream>
#include <fstream>
#include <thread>

namespace mel {

const std::string Table::table_id = "MEL::Table";

Table::Table(const std::string &name, const std::vector<std::string> &col_names, const std::vector<std::vector<double>> &values) :
	name_(name),
	n_cols_(col_names.size()),
	n_rows_(values.size()),
	col_names_(col_names),
	values_(values)
{ 
	if (!check_inner_dim(values, col_names.size())) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not stored.";
		n_rows_ = 0;
		values_.clear();
		return;
	}
}

const std::string &Table::name() const {
	return name_;
}

void Table::rename(const std::string &name) {
	name_ = name;
}

bool Table::set_col_names(const std::vector<std::string> &col_names) {
	if (n_rows_ != 0) {
		if (col_names.size() != n_cols_) {
			LOG(Warning) << "New column names given to Table does not match size of stored values. Header not changed.";
			return false;
		}
	}
	n_cols_ = col_names.size();
	col_names_ = col_names;
	return true;
}

bool Table::set_values(const std::vector<std::vector<double>> &values) {	
	if (!check_inner_dim(values, n_cols_)) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not stored.";
		n_rows_ = 0;
		values_.clear();
		return false;
	}
	n_rows_ = values.size();
	values_ = values;
	return true;
}

bool Table::push_back_row(const std::vector<double> &row) {
	if (row.size() != n_cols_) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not stored.";
		return false;
	}
	n_rows_++;
	values_.push_back(row);
	return true;
}

bool Table::push_back_rows(const std::vector<std::vector<double>> &values) {
	if (!check_inner_dim(values, n_cols_)) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not stored.";
		return false;
	}
	for (std::size_t i = 0; i < values.size(); ++i) {
		n_rows_++;
		values_.push_back(values[i]);
	}
	return true;

}

bool Table::insert_row(const std::vector<double> &row, std::size_t index) {
	if (row.size() != n_cols_) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not inserted.";
		return false;
	}
	if (index > n_rows_) {
		LOG(Warning) << "Row index given to Table outside of range. Values not inserted.";
		return false;
	}
	values_.insert(values_.begin() + index, row);
	n_rows_++;
	return true;
}

bool Table::insert_rows(const std::vector<std::vector<double>> &rows, std::size_t index) {	
	if (!check_inner_dim(rows, n_cols_)) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not inserted.";
		return false;
	}
	if (index > n_rows_) {
		LOG(Warning) << "Row index given to Table outside of range. Values not inserted.";
		return false;
	}
	values_.insert(values_.begin() + index, rows.begin(), rows.end());
	n_rows_ += rows.size();
	return true;
}

bool Table::push_back_col(const std::string &col_name, const std::vector<double> &col) {
	if (n_cols_ > 0 && col.size() != n_rows_) {
		LOG(Warning) << "Values given to Table do not match size of previously stored values. Values not stored.";
		return false;
	}
	if (n_cols_ == 0) {
		n_rows_ = col.size();
		values_.resize(n_rows_);
	}
	n_cols_++;
	col_names_.push_back(col_name);		
	for (std::size_t i = 0; i < col.size(); ++i) {
		values_[i].push_back(col[i]);
	}
	return true;
}

bool Table::push_back_cols(const std::vector<std::string> &col_names, const std::vector<std::vector<double>> &values) {
	if (n_cols_ > 0 && values.size() != n_rows_) {
		LOG(Warning) << "Values given to Table do not match size of previously stored values. Values not stored.";
		return false;
	}
	if (!check_inner_dim(values, col_names.size())) {
		LOG(Warning) << "Values given to Table do not match number of column names given. Values not stored.";
		return false;
	}
	if (n_cols_ == 0) {
		n_rows_ = values.size();
		values_.resize(n_rows_);
	}
	n_cols_ += col_names.size();
	for (std::size_t i = 0; i < values.size(); ++i) {
		for (std::size_t j = 0; j < col_names.size(); ++j) {
			values_[i].push_back(values[i][j]);
		}		
	}
	return true;	
}

bool Table::insert_col(const std::string &col_name, const std::vector<double> &col, std::size_t index) {
	if (col.size() != n_rows_) {
		LOG(Warning) << "Values given to Table do not match number of rows. Values not inserted.";
		return false;
	}
	if (index > n_cols_) {
		LOG(Warning) << "Column index given to Table outside of range. Values not inserted.";
		return false;
	}
	col_names_.insert(col_names_.begin() + index, col_name);
	if (n_cols_ == 0) {
		n_rows_ = 1;
		values_.resize(n_rows_);
	}
	for (std::size_t i = 0; i < n_rows_; ++i) {
		values_[i].insert(values_[i].begin() + index, col[i]);
	}
	n_cols_++;
	return true;
}

bool Table::insert_cols(const std::vector<std::string> &col_names, const std::vector<std::vector<double>> &cols, std::size_t index) {
	if (cols.size() != n_rows_) {
		LOG(Warning) << "Values given to Table do not match number of rows. Values not inserted.";
		return false;
	}
	if (!check_inner_dim(cols, col_names.size())) {
		LOG(Warning) << "Values given to Table do not match number of column names given. Values not inserted.";
		return false;
	}
	if (index > n_cols_) {
		LOG(Warning) << "Column index given to Table outside of range. Values not inserted.";
		return false;
	}
	if (n_cols_ == 0) {
		n_rows_ = cols.size();
		values_.resize(n_rows_);
	}
	n_cols_ += col_names.size();
	col_names_.insert(col_names_.begin() + index, col_names.begin(), col_names.end());
	for (std::size_t i = 0; i < n_rows_; ++i) {
		values_[i].insert(values_[i].begin() + index, cols[i].begin(), cols[i].end());
	}	
	return true;
}

const std::vector<std::string> &Table::get_col_names() const {
	return col_names_;
}

const std::string &Table::get_col_name(std::size_t index) const {
	if (index >= n_cols_) {
		LOG(Warning) << "Column index given to Table outside of range. Returning last column header.";
		return col_names_.back();
	}
	return col_names_[index];
}

const double& Table::operator()(std::size_t row_index, std::size_t col_index) const {
	if (row_index >= n_rows_ || col_index >= n_cols_) {
		LOG(Warning) << "Indices given to Table outside of range. Returning last value within range.";
		return values_[row_index >= n_rows_ ? n_rows_ : row_index][col_index >= n_cols_ ? n_cols_ : col_index];
	}
	return values_[row_index][col_index];
}

const std::vector<double> &Table::operator()(std::size_t row_index) const {
	if (row_index >= n_rows_) {
		LOG(Warning) << "Row index given to Table outside of range. Returning last row.";
		return values_.back();
	}
	return values_[row_index];
}

const std::vector<std::vector<double>> &Table::values() const {
	return values_;
}

std::vector<double> Table::get_row(std::size_t index) const {
	if (index >= n_rows_) {
		LOG(Warning) << "Row index given to Table outside of range. Returning empty vector.";
		return values_.back();
	}
	return values_[index];
}

std::vector<double> Table::get_col(std::size_t index) const {
	if (index >= n_cols_) {
		LOG(Warning) << "Row index given to Table outside of range. Returning empty vector.";
		return std::vector<double>();
	}
	std::vector<double> col(n_rows_);
	for (std::size_t i = 0; i < col.size(); ++i) {
		col[i] = values_[i][index];
	}
	return col;
}

void Table::pop_back_row() {
	n_rows_--;
	values_.pop_back();
}

bool Table::erase_row(std::size_t index) {
	if (index >= n_rows_) {
		LOG(Warning) << "Row index given to Table outside of range. Values not erased.";
		return false;
	}
	n_rows_--;
	values_.erase(values_.begin() + index);
	return true;
}

bool Table::erase_rows(std::size_t index_first, std::size_t index_last) {
	if (index_last >= n_rows_ || index_first >= index_last ) {
		LOG(Warning) << "Row indices given to Table outside of range or out of order. Values not erased.";
		return false;
	}
	n_rows_ -= index_last - index_first;
	values_.erase(values_.begin() + index_first, values_.begin() + index_last);
	return true;
}

void Table::pop_back_col() {
	n_cols_--;
	col_names_.pop_back();
	for (std::size_t i = 0; i < values_.size(); ++i) {
		values_[i].pop_back();
	}
}

bool Table::erase_col(std::size_t index) {
	if (index >= n_cols_) {
		LOG(Warning) << "Column index given to Table outside of range. Values not erased.";
		return false;
	}
	n_rows_--;
	col_names_.erase(col_names_.begin() + index);
	for (std::size_t i = 0; i < values_.size(); ++i) {
		values_[i].erase(values_[i].begin() + index);
	}
	return true;
}

bool Table::erase_cols(std::size_t index_first, std::size_t index_last) {
	if (index_last >= n_cols_ || index_first >= index_last) {
		LOG(Warning) << "Column indices given to Table outside of range or out of order. Values not erased.";
		return false;
	}
	n_cols_ -= index_last - index_first;
	col_names_.erase(col_names_.begin() + index_first, col_names_.begin() + index_last);
	for (std::size_t i = 0; i < values_.size(); ++i) {
		values_[i].erase(values_[i].begin() + index_first, values_[i].begin() + index_last);
	}
	return true;
}

void Table::clear() {
	n_cols_ = 0;
	n_rows_ = 0;
	col_names_.clear();
	values_.clear();
}

void Table::clear_values() {
	n_rows_ = 0;
	values_.clear();
}

std::size_t Table::row_count() const {
	return n_rows_;
}

std::size_t Table::col_count() const {
	return n_cols_;
}

bool Table::empty() const {
	return col_names_.empty();
}

bool Table::check_inner_dim(const std::vector<std::vector<double>> &values, std::size_t row_size) const {
	bool dim_valid = true;
	for (std::size_t i = 0; i < values.size(); ++i) {
		if (values[i].size() != row_size) {
			dim_valid = false;
		}
	}
	return dim_valid;
}

std::ostream& operator<<(std::ostream& os, const Table& table) {
	os << table.name() << ": " << table.n_rows_ << " rows by " << table.n_cols_ << " columns" << std::endl;
	if (!table.empty()) {
		for (std::size_t i = 0; i < table.n_cols_ - 1; i++) {
			os << table.col_names_[i] << "\t";
		}
		os << table.col_names_[table.n_cols_ - 1] << std::endl;
	}
	for (std::size_t i = 0; i < table.n_rows_; i++) {
		for (size_t j = 0; j < table.n_cols_ - 1; ++j) {
			os << table(i, j) << "\t";
		}
		os << table(i, table.n_cols_ - 1) << std::endl;
	}
	return os;
}



bool Table::write(const std::string &filepath, const Table &data) {
	File file(filepath, WriteMode::Truncate);
	std::ostringstream oss;
	oss << std::setprecision(6);
	oss << make_header(data);
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

bool Table::write(const std::string &filepath, const std::vector<Table> &data) {
	File file(filepath, WriteMode::Truncate);

	std::ostringstream oss;
	oss << std::setprecision(6);
	for (std::size_t k = 0; k < data.size(); ++k) {
		oss << make_header(data[k]);
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


bool Table::read(const std::string &filepath, Table &data) {
    std::string directory, filename, ext, full;
    if (!parse_filepath(filepath, directory, filename, ext, full))
        return false;
    std::ifstream file(full);
    file.precision(6);
    if (!file.is_open())
        return false;
	bool is_table = false;
	data.clear();
	std::string csv_line;
	std::string el_str;
	double value;
	std::vector<std::string> name_row;
	std::vector<double> value_row;
	while (std::getline(file, csv_line)) {
		std::istringstream iss(csv_line);
		if (std::getline(iss, el_str, ',')) {
			if (el_str.compare(Table::table_id) == 0) {
				if (!parse_header(data, csv_line)) {
					LOG(Warning) << "Table header in " << filename << " could not be parsed.";
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
	if (std::getline(file, csv_line)) {
		std::istringstream iss(csv_line);
		name_row.clear();
		while (std::getline(iss, el_str, ',')) {
			name_row.push_back(el_str);
		}
		data.set_col_names(name_row);
	}
	while (std::getline(file, csv_line)) {
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

bool Table::read(const std::string &filepath, std::vector<Table> &data) {
    std::string directory, filename, ext, full;
    if (!parse_filepath(filepath, directory, filename, ext, full))
        return false;
    std::ifstream file(full);
    file.precision(6);
    if (!file.is_open())
        return false;
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
	while (std::getline(file, csv_line)) {
		std::istringstream iss(csv_line);
		if (scan_for_table) {
			if (std::getline(iss, el_str, ',')) {
				if (el_str.compare(Table::table_id) == 0) {
					data.emplace_back();
					if (!parse_header(data[table_index], csv_line)) {
						LOG(Warning) << "Table header in " << filename << " could not be parsed.";
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

std::string Table::make_header(const Table &table) {
	std::ostringstream oss;
	oss << Table::table_id << ",";
	oss << "name" << "=" << table.name() << ",";
	oss << "n_rows" << "=" << table.row_count() << ",";
	oss << "n_cols" << "=" << table.col_count() << "\r\n";
	return oss.str();
}

bool Table::parse_header(Table &table, const std::string &header) {
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







} // namespace mel