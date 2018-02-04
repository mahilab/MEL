// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
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
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_ROLLINGFILEWRITER_HPP
#define MEL_ROLLINGFILEWRITER_HPP

#include <MEL/Logging/Converters/UTF8Converter.hpp>
#include <MEL/Logging/File.hpp>
#include <MEL/Logging/Writers/Writer.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <algorithm>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

template <class Formatter, class Converter = UTF8Converter>
class RollingFileWriter : public Writer {
public:
    RollingFileWriter(const char* fileName,
                      size_t maxFileSize = 0,
                      int maxFiles       = 0)
        : m_fileSize(),
          m_maxFileSize(
              (std::max)(static_cast<off_t>(maxFileSize),
                         static_cast<off_t>(
                             1000)))  // set a lower limit for the maxFileSize
          ,
          m_lastFileNumber((std::max)(maxFiles - 1, 0)),
          m_firstWrite(true) {
        split_file_name(fileName, m_fileNameNoExt, m_fileExt);
    }

    virtual void write(const Record& record) {
        Lock lock(m_mutex);

        if (m_firstWrite) {
            openLogFile();
            m_firstWrite = false;
        } else if (m_lastFileNumber > 0 && m_fileSize > m_maxFileSize &&
                   -1 != m_fileSize) {
            rollLogFiles();
        }

        int bytesWritten =
            m_file.write(Converter::convert(Formatter::format(record)));

        if (bytesWritten > 0) {
            m_fileSize += bytesWritten;
        }
    }

private:
    void rollLogFiles() {
        m_file.close();

        std::string lastFileName = buildFileName(m_lastFileNumber);
        mel::File::unlink(lastFileName.c_str());

        for (int fileNumber = m_lastFileNumber - 1; fileNumber >= 0;
             --fileNumber) {
            std::string currentFileName = buildFileName(fileNumber);
            std::string nextFileName    = buildFileName(fileNumber + 1);

            File::rename(currentFileName.c_str(), nextFileName.c_str());
        }

        openLogFile();
    }

    void openLogFile() {
        std::string fileName = buildFileName();
        m_fileSize           = m_file.open(fileName.c_str());

        if (0 == m_fileSize) {
            int bytesWritten =
                m_file.write(Converter::header(Formatter::header()));

            if (bytesWritten > 0) {
                m_fileSize += bytesWritten;
            }
        }
    }

    std::string buildFileName(int fileNumber = 0) {
        std::ostringstream ss;
        ss << m_fileNameNoExt;

        if (fileNumber > 0) {
            ss << '.' << fileNumber;
        }

        if (!m_fileExt.empty()) {
            ss << '.' << m_fileExt;
        }

        return ss.str();
    }

private:
    Mutex m_mutex;
    File m_file;
    off_t m_fileSize;
    const off_t m_maxFileSize;
    const int m_lastFileNumber;
    std::string m_fileExt;
    std::string m_fileNameNoExt;
    bool m_firstWrite;
};
}  // namespace mel

#endif // MEL_ROLLINGFILEWRITER_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
