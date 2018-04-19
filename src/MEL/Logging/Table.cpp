#include <MEL/Logging/Table.hpp>
#include <MEL/Logging/Log.hpp>
#include <sstream>

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

bool Table::add_rows(const std::vector<std::vector<double>> &values) {
	if (!check_inner_dim(values, n_cols_)) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not stored";
		return false;
	}
	for (std::size_t i = 0; i < values.size(); ++i) {
		n_rows_++;
		values_.push_back(values[i]);
	}
	return true;

}

bool Table::push_back_col(const std::string &col_name, const std::vector<double> &col) {
	if (n_cols_ > 0 && col.size() != n_rows_) {
		LOG(Warning) << "Values given to Table do not match size of previously stored values. Values not stored";
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

bool Table::add_cols(const std::vector<std::string> &col_names, const std::vector<std::vector<double>> &values) {
	if (n_cols_ > 0 && values.size() != n_rows_) {
		LOG(Warning) << "Values given to Table do not match size of previously stored values. Values not stored";
		return false;
	}
	if (!check_inner_dim(values, col_names.size())) {
		LOG(Warning) << "Values given to Table do not match number of columns. Values not stored";
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
	os << table.name() << std::endl;
	if (!table.empty()) {
		for (std::size_t i = 0; i < table.n_cols_ - 1; i++) {
			os << table.col_names_[i] << " ";
		}
		os << table.col_names_[table.n_cols_ - 1] << std::endl;
	}
	for (std::size_t i = 0; i < table.n_rows_; i++) {
		for (size_t j = 0; j < table.n_cols_ - 1; ++j) {
			os << table(i, j) << " ";
		}
		os << table(i, table.n_cols_ - 1) << std::endl;
	}
	os << std::endl;
	return os;
}

} // namespace mel