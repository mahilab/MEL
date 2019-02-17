#include <MEL/Logging/Csv.hpp>
#include <MEL/Logging/Log.hpp>
#include <thread>

namespace mel
{

bool Csv::parse_filepath(const std::string &in, std::string &directory, std::string &filename, std::string &ext, std::string &full)
{
    // can't do anythign with empty string
    if (in == "" || in.empty())
        return false;
    // split path
    auto splits = split_path(in);
    // split filename
    auto filename_ext = splits.back();
    if (filename_ext == "" || filename_ext.empty())
        return false;
    split_filename(filename_ext, filename, ext);
    if (ext.empty())
        ext = "csv";
    // make directory string
    directory.clear();
    for (std::size_t i = 0; i < splits.size() - 1; ++i)
        directory += splits[i] + get_separator();
    directory = tidy_path(directory, false);
    full = directory + filename + "." + ext;
    return true;
}

bool Csv::read(std::vector<std::vector<double>> &data, const std::string &filename, const std::string &directory)
{
    // std::string filename_no_ext;
    // std::string file_ext;
    // split_filename(filename, filename_no_ext, file_ext);
    // if (file_ext.empty())
    // {
    //     file_ext = "csv";
    // }
    // std::string full_filename;
    // full_filename = directory + get_separator() + filename_no_ext + "." + file_ext;
    // LOG(Verbose) << "Reading data from " << full_filename;
    // std::vector<std::vector<double>> output;
    // std::ifstream input(full_filename);
    // input.precision(6);
    // if (!input.is_open())
    // {
    //     LOG(Warning) << "File not found in DataLogger::read_from_csv().";
    //     return false;
    // }
    // data.clear();
    // std::string csv_line;
    // while (std::getline(input, csv_line))
    // {
    //     std::istringstream csv_stream(csv_line);
    //     std::vector<double> row;
    //     std::string value_str;
    //     double value;
    //     while (std::getline(csv_stream, value_str, ','))
    //     {
    //         std::istringstream ss(value_str);
    //         ss >> value;
    //         row.push_back(value);
    //     }
    //     data.push_back(row);
    // }
    return true;
}

bool Csv::read(std::vector<std::vector<double>> &data_out, std::size_t row_offset, std::size_t col_offset,
                               const std::string &filename, const std::string &directory)
{
    // std::string filename_no_ext;
    // std::string file_ext;
    // split_filename(filename, filename_no_ext, file_ext);
    // if (file_ext.empty())
    // {
    //     file_ext = "csv";
    // }
    // std::string full_filename;
    // full_filename = directory + get_separator() + filename_no_ext + "." + file_ext;
    // LOG(Verbose) << "Reading data from " << full_filename;
    // std::vector<std::vector<double>> output;
    // std::ifstream input(full_filename);
    // input.precision(6);
    // if (!input.is_open())
    // {
    //     LOG(Warning) << "File " << full_filename << " not found";
    //     return false;
    // }
    // data_out.clear();
    // std::size_t row_idx = 0;
    // std::string csv_line;
    // while (std::getline(input, csv_line))
    // {
    //     if (row_idx++ < row_offset)
    //         continue;
    //     std::istringstream csv_stream(csv_line);
    //     std::vector<double> row;
    //     std::string value_str;
    //     double value;
    //     std::size_t col_idx = 0;
    //     while (std::getline(csv_stream, value_str, ','))
    //     {
    //         if (col_idx++ < col_offset)
    //             continue;
    //         std::istringstream ss(value_str);
    //         ss >> value;
    //         row.push_back(value);
    //     }
    //     data_out.push_back(row);
    // }
    return true;
}

} // mel
