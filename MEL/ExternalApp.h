#pragma once
#include <string>

namespace mel {

    namespace util {

        class ExternalApp {

        public:

            /// Default constructor. #name is for MEL only. The path should be an
            /// absoulute path and include the filename and extension. For example
            /// "C:\\dev\\Python27\\python.exe". Use double backslashes only.
            ExternalApp(std::string name, std::string path);

            /// Launches the external app *.exe at the specified path location
            void launch();

            std::string name_; ///< name of the application (only used in MEL)
            std::string path_; ///< the full path to the application including the app filename (eg  .../application.exe)
        };

    }

}