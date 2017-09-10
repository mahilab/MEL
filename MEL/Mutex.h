#pragma once
#include <string>

namespace mel {

    namespace util {

        class Mutex {

        public:

            /// The mode by which a mutex can be constructed. 
            enum class Mode {
                CREATE, ///< create the named mutex if it does not exit
                OPEN    ///< only attempt to open an existing named mutex
            };

            enum class Result {
                LOCK_ACQUIRED,  ///< mutex lock aquired
                LOCK_ABANDONED, ///< wait on mutex lock abandoned
                LOCK_TIMEOUT,   ///< wait on mutex lock timed out
                LOCK_FAILED,    ///< wait on mutex lock failed
                NOT_OPEN,       ///< mutex lock failed because mutex is not open
                RELEASE_SUCCESS, ///< release of mutex lock succeeded
                RELEASE_FAILED, ///< realse of mutex lock failed
                CLOSE_FAILED    ///< close of mutex failed
            };

            /// Defaut constructor
            Mutex();
            Mutex(std::string name, Mode mode);
            /// Default destructor. Releases mutex if it is currently open.
            ~Mutex();

            /// Waits for mutex to release and attempts to lock it
            Result try_lock();
            /// Releases lock on mutex
            Result release();

        private:

            std::string name_;     /// name of the mutex
            Mode mode_;            /// mode by which this Mutex instance was created
            void* handle_ = NULL;; /// handle to underlying Windows named mutex

        };

    }

}