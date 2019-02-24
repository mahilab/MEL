// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#pragma once


#include <vector>
#include <string>

namespace mel{

class Table {
public:
	static const std::string table_id;

public:
	/// Constructor
	Table(const std::string &name = "", const std::vector<std::string> &col_names = std::vector<std::string>(), const std::vector<std::vector<double>> &values = std::vector<std::vector<double>>());

	const std::string &name() const;

	void rename(const std::string &name);

	bool set_col_names(const std::vector<std::string> &col_names);

	bool set_values(const std::vector<std::vector<double>> &values);

	bool push_back_row(const std::vector<double> &row);

	bool push_back_rows(const std::vector<std::vector<double>> &values);

	/// Insert a row of data before index.
	bool insert_row(const std::vector<double> &row, std::size_t index);

	/// Inserts a block of data starting before row index first.
	bool insert_rows(const std::vector<std::vector<double>> &rows, std::size_t index);

	/// Insert a column of data before index.
	bool insert_col(const std::string &col_name, const std::vector<double> &col, std::size_t index);

	/// Inserts a block of data starting before column index first.
	bool insert_cols(const std::vector<std::string> &col_names, const std::vector<std::vector<double>> &cols, std::size_t index);

	bool push_back_col(const std::string &col_name, const std::vector<double> &col = std::vector<double>());

	bool push_back_cols(const std::vector<std::string> &col_names, const std::vector<std::vector<double>> &values = std::vector<std::vector<double>>());

	const std::vector<std::string> &get_col_names() const;

	const std::string &get_col_name(std::size_t index) const;

	/// Read access in 2D
	const double &operator()(std::size_t row_index, std::size_t col_index) const;

	/// Read access to rows
	const std::vector<double> &operator()(std::size_t row_index) const;

	/// Read access to all values
	const std::vector<std::vector<double>> &values() const;

	std::vector<double> get_row(std::size_t index) const;

	std::vector<double> get_col(std::size_t index) const;

	void pop_back_row();

	bool erase_row(std::size_t index);

	bool erase_rows(std::size_t index_first, std::size_t index_last);

	void pop_back_col();

	bool erase_col(std::size_t index);

	bool erase_cols(std::size_t index_first, std::size_t index_last);

	void clear();

	void clear_values();

	std::size_t row_count() const;

	std::size_t col_count() const;

	bool empty() const;

	/// Overload the << stream operator with a Table as the rhs argument
	friend std::ostream& operator<<(std::ostream& os, const Table& table);

private:

	bool check_inner_dim(const std::vector<std::vector<double>> &values, std::size_t row_size) const;

private:
	std::string name_;

	std::size_t n_cols_;
	std::size_t n_rows_;

	std::vector<std::string> col_names_;
	std::vector<std::vector<double>> values_;
};

std::ostream& operator<<(std::ostream& os, const Table& table);

} // namespace mel
