#include <MEL/Core/Types.hpp>
#include <cstdlib>
#include <memory>
#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace mel {

#ifdef __GNUG__

std::string demangle( const char* mangled_name ) {

    std::size_t len = 0 ;
    int status = 0 ;
    std::unique_ptr< char, decltype(&std::free) > ptr(
                __cxxabiv1::__cxa_demangle( mangled_name, nullptr, &len, &status ), &std::free ) ;
    return ptr.get() ;
}

#else

// does nothing if not g++
std::string demangle(const char* name) {
    return name;
}

#endif

} // namespace mel
