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
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once

#include <string>

namespace mel {

/// Class that allows for launching external applications
class ExternalApp {
public:
    /// Constructor. #name is for MEL only. The path should be an
    /// absoulute path and include the filename and extension. For example
    /// "C:\\dev\\Python27\\python.exe". Use double backslashes only.
    ExternalApp(std::string name, std::string path);

    /// Launches the external app *.exe at the specified path location
    void launch();

    void launch(std::string params);

    std::string name_;  ///< name of the application (only used in MEL)
    std::string path_;  ///< the full path to the application including the app
                        ///< filename (eg  .../application.exe)
};

}  // namespace mel
