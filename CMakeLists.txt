project(mycmake)

#ProjectDependencies->DependsOn->BuildOrder
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>")

set (TOP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}) #CMAKE_SOURCE_DIR
set (ALL_SRC_DIR ${TOP_SRC_DIR}/src)

#include(cmake/dep_3rdlib_files.cmake)

include_directories(${TOP_SRC_DIR})

configure_file(${ALL_SRC_DIR}/config.h.win32              ${ALL_SRC_DIR}/config.h)

file(GLOB_RECURSE MYCMAKE_HDRS_LIST RELATIVE    ${TOP_SRC_DIR} ${ALL_SRC_DIR}/*.h*)
file(GLOB_RECURSE MYCMAKE_SRCS_LIST RELATIVE    ${TOP_SRC_DIR} ${ALL_SRC_DIR}/*.c*)

#add_library               (mycmake              STATIC      ${MYCMAKE_HDRS_LIST} ${MYCMAKE_SRCS_LIST} )
ADD_EXECUTABLE             (mycmake              ${MYCMAKE_HDRS_LIST} ${MYCMAKE_SRCS_LIST} )
target_compile_definitions (mycmake  PRIVATE     UNICODE;_UNICODE )
target_include_directories (mycmake  PUBLIC      ./src)
target_link_directories    (mycmake  PUBLIC      ./lib)
SET_TARGET_PROPERTIES      (mycmake  PROPERTIES  CLEAN_DIRECT_OUTPUT 1     OUTPUT_NAME "mycmake")

SET(install_targets mycmake )
SET(install_hdr_dir include)

INSTALL(TARGETS ${install_targets}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION ${install_hdr_dir}
       )

#install(FILES ${MYCMAKE_HDRS_LIST} DESTINATION ${install_hdr_dir})
install(DIRECTORY "inc_sub_dir" DESTINATION ${install_hdr_dir} FILES_MATCHING PATTERN "*.h")

