namespace mel {

template <typename Array1D>
bool Csv::write_row(const std::string &filepath, const Array1D &data)
{
    std::string directory, filename, ext, full;
    if (!parse_filepath(filepath, directory, filename, ext, full))
        return false;
    create_directory(directory);
    File::unlink(full.c_str());
    File file(full.c_str());
    std::ostringstream ss;
    ss << std::setprecision(6);
    for (size_t j = 0; j < data.size() - 1; ++j)
        ss << data[j] << ",";
    ss << data[data.size() - 1] << "\r\n";
    file.write(ss.str());
    file.close();
    return true;
}

template <typename Array2D>
bool Csv::write_rows(const std::string &filepath, const Array2D &data)
{
    std::string directory, filename, ext, full;
    if (!parse_filepath(filepath, directory, filename, ext, full))
        return false;
    create_directory(directory);
    File::unlink(full.c_str());
    File file(full.c_str());
    for (std::size_t i = 0; i < data.size(); i++)
    {
        std::ostringstream ss;
        ss << std::setprecision(6);
        for (size_t j = 0; j < data[i].size() - 1; ++j)
            ss << data[i][j] << ",";
        ss << data[i][data[i].size() - 1] << "\r\n";
        file.write(ss.str());
    }
    file.close();
    return true;
}

template <typename Array1D>
bool Csv::append_row(const std::string &filepath, const Array1D &data)
{
    std::string directory, filename, ext, full;
    if (!parse_filepath(filepath, directory, filename, ext, full))
        return false;
    create_directory(directory);
    File file(full.c_str());
    std::ostringstream ss;
    ss << std::setprecision(6);
    for (size_t j = 0; j < data.size() - 1; ++j)
        ss << data[j] << ",";
    ss << data[data.size() - 1] << "\r\n";
    file.write(ss.str());
    file.close();
    return true;
}

template <typename Array2D>
bool Csv::append_rows(const std::string &filepath, const Array2D &data)
{
    std::string directory, filename, ext, full;
    if (!parse_filepath(filepath, directory, filename, ext, full))
        return false;
    create_directory(directory);
    File file(full.c_str());
    for (std::size_t i = 0; i < data.size(); i++)
    {
        std::ostringstream ss;
        ss << std::setprecision(6);
        for (size_t j = 0; j < data[i].size() - 1; ++j)
        {
            ss << data[i][j] << ",";
        }
        ss << data[i][data[i].size() - 1] << "\r\n";
        file.write(ss.str());
    }
    file.close();
    return true;
}

} // namespace mel