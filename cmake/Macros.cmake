macro(mel_source_group_by_folder target)
  set(SOURCE_GROUP_DELIMITER "/")
  set(last_dir "")
  set(files "")
  foreach(file ${${target}_SRC} ${${target}_HEADERS})
    file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}/src/${target}" ${file})
    get_filename_component(dir "${relative_file}" PATH)
    if (NOT "${dir}" STREQUAL "${last_dir}")
      if (files)
        SOURCE_GROUP("${last_dir}" FILES ${files})
      endif (files)
      set(files "")
    endif (NOT "${dir}" STREQUAL "${last_dir}")
    set(files ${files} ${file})
    set(last_dir "${dir}")
  endforeach(file)
  if (files)
    source_group("${last_dir}" FILES ${files})
  endif (files)
endmacro(mel_source_group_by_folder)

macro(mel_incompatible first second)
  if (${first} AND ${second})
    message("Options " ${first} " and " ${second} " incompatible - disabling " ${second})
    set(${second} OFF)
  endif()
endmacro(mel_incompatible)

macro(mel_add_library target)

    # parse the arguments
    cmake_parse_arguments(THIS "" "" "SOURCES" ${ARGN})
    if (NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Extra unparsed arguments when calling mel_add_library: ${THIS_UNPARSED_ARGUMENTS}")
    endif()

    # Build MEL as shared or static library?
    if (BUILD_SHARED_LIBS)
        # message("Building MEL::${target} as a Shared library")
        # if (MEL_NI_LRT)
        #     set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
        # endif()
        # add_library(${target} SHARED "")
        # target_compile_definitions(${target} PRIVATE -DMEL_EXPORTS)      
    else()
        message("Building MEL::${target}")
        add_library(${target} STATIC "")
        target_compile_definitions(${target} PUBLIC)
    endif()
    
    # change filename of sublibaries
    if (NOT ${target} MATCHES "MEL")
      set_target_properties(${target} PROPERTIES OUTPUT_NAME "MEL-${target}")
    endif()

    # add alias so that find_package(MEL) and add_subdirectory(MEL) provide the same targets and namespaces
    add_library(MEL::${target} ALIAS ${target})

    # add sources
    target_sources(${target} PRIVATE ${THIS_SOURCES})
    
    # add <project>/include as public include directory
    target_include_directories(${target}
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        PRIVATE
            ${PROJECT_SOURCE_DIR}/src                
    )

    # For Visual Studio on Windows, export debug symbols (PDB files) to lib directory
    if(BUILD_SHARED_LIBS)
        # DLLs export debug symbols in the linker PDB (the compiler PDB is an intermediate file)
        set_target_properties(${target} PROPERTIES
                                PDB_NAME "${target}$"
                                PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
    else()
        # Static libraries have no linker PDBs, thus the compiler PDBs are relevant
        set_target_properties(${target} PROPERTIES
                                COMPILE_PDB_NAME "${target}"
                                COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
    endif()

    # add compile features
    # complete listing:
    # https://cmake.org/cmake/help/v3.14/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html
    target_compile_features(${target}
        PUBLIC
            cxx_std_11
        PRIVATE
    )

    # add install rule
    install(TARGETS ${target} EXPORT MEL-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    # add misc properties
    set_target_properties(${target} PROPERTIES EXPORT_NAME ${target})
    set_target_properties(${target} PROPERTIES FOLDER "MEL")
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)

endmacro(mel_add_library)

macro(mel_example target)

    cmake_parse_arguments(THIS "" "" "DEPENDS" ${ARGN})

    # create executable
    add_executable(${target} "ex_${target}.cpp")

    # set dependencies
    target_link_libraries(${target} PRIVATE MEL::MEL ${THIS_DEPENDS})

    # add install rule
    install(TARGETS ${target}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    set_target_properties(${target} PROPERTIES FOLDER "Examples")
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)

endmacro(mel_example)
