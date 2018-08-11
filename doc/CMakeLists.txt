find_package(Doxygen REQUIRED)

set(MEL_INPUT_DOXYFILE "${PROJECT_SOURCE_DIR}/doc/Doxyfile.txt")
set(MEL_OUTPUT_DOXYFILE "${PROJECT_BINARY_DIR}/doc/Doxyfile.txt")

# Copy doxyfile, replacing only @variables@
configure_file(${MEL_INPUT_DOXYFILE} ${MEL_OUTPUT_DOXYFILE} @ONLY)

# Copy other files for documentation
# configure_file(${PROJECT_SOURCE_DIR}/doc/Doxygen.css ${PROJECT_BINARY_DIR}/doc/html/Doxygen.css COPYONLY)

# Add target for documentation, invokes Doxygen
add_custom_target(doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_BINARY_DIR}/doc/Doxyfile.txt"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
