cmake_minimum_required(VERSION 3.24)

project(mycmake)

set(CMAKE_C_STANDARD 99) 
set(CMAKE_C_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

set(ENV{HomeDir}  ${CMAKE_CURRENT_LIST_DIR}/../out/${CMAKE_SYSTEM_NAME})

#ProjectDependencies->DependsOn->BuildOrder
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>")

set (MYCMAKE_BIN_DIR  ${CMAKE_CURRENT_BINARY_DIR})  
set (MYCMAKE_TOP_DIR  ${CMAKE_CURRENT_SOURCE_DIR}) #CMAKE_SOURCE_DIR

set (ALL_SRC_DIR      ${MYCMAKE_TOP_DIR}/src)

set (MYCMAKE_INC_DIRS ${MYCMAKE_TOP_DIR}/inc     ${MYCMAKE_TOP_DIR}/src    ) 
set (MYCMAKE_LIB_DIRS ${MYCMAKE_TOP_DIR}/lib                               ) #CMAKE_SOURCE_DIR

#include(cmake/dep_3rdlib_files.cmake)

configure_file(${ALL_SRC_DIR}/config.h.win32              ${ALL_SRC_DIR}/config.h)

file(GLOB           MYCMAKE_HDR_LIST        RELATIVE    ${MYCMAKE_TOP_DIR}    ${ALL_SRC_DIR}/*.h*   )
file(GLOB           MYCMAKE_SRC_LIST        RELATIVE    ${MYCMAKE_TOP_DIR}    ${ALL_SRC_DIR}/*.c*   )
file(GLOB           MYCMAKE_SRC_MAIN        RELATIVE    ${MYCMAKE_TOP_DIR}    ${ALL_SRC_DIR}/main.c*)

list(REMOVE_ITEM    MYCMAKE_SRC_LIST                    ${MYCMAKE_SRC_MAIN}    )

message ("[INFO] MYCMAKE_TOP_DIR :${MYCMAKE_TOP_DIR}")
message ("[INFO] MYCMAKE_INC_DIRS:${MYCMAKE_INC_DIRS}")
message ("[INFO] MYCMAKE_SRC_LIST:${MYCMAKE_SRC_LIST}")

source_group(TREE          ${MYCMAKE_TOP_DIR}  FILES      ${MYCMAKE_SRC_LIST}    ${MYCMAKE_HDR_LIST})

set                       (  MYCMAKE_STATIC               libmycmake_static                          )
add_library               (${MYCMAKE_STATIC}   STATIC     ${MYCMAKE_SRC_LIST}     ${MYCMAKE_SRC_MAIN})
target_compile_definitions(${MYCMAKE_STATIC}   PRIVATE    ${MYCMAKE_DEF_LIST}     HAVE_CONFIG_H      )
target_include_directories(${MYCMAKE_STATIC}   PUBLIC     ${MYCMAKE_INC_DIRS}                        )
target_link_directories   (${MYCMAKE_STATIC}   PUBLIC     ${MYCMAKE_LIB_DIRS}                        )
target_link_libraries     (${MYCMAKE_STATIC}              ${MYCMAKE_DEP_LIBS}     ${dep_libs}        )
target_compile_options    (${MYCMAKE_STATIC}   PRIVATE     /Z7     /W4                               )
set_target_properties     (${MYCMAKE_STATIC}   PROPERTIES CLEAN_DIRECT_OUTPUT 1   OUTPUT_NAME libavcodec_static  )

set                       (  MYCMAKE_SHARED               libmycmake_shared                          )
add_library               (${MYCMAKE_SHARED}   SHARED     ${MYCMAKE_SRC_LIST}     ${MYCMAKE_SRC_MAIN})
target_compile_definitions(${MYCMAKE_SHARED}   PRIVATE    ${MYCMAKE_DEF_LIST}     HAVE_CONFIG_H      )
target_include_directories(${MYCMAKE_SHARED}   PUBLIC     ${MYCMAKE_INC_DIRS}                        )
target_link_directories   (${MYCMAKE_SHARED}   PUBLIC     ${MYCMAKE_LIB_DIRS}                        )
target_link_libraries     (${MYCMAKE_SHARED}              ${MYCMAKE_DEP_LIBS}     ${dep_libs}        )
target_compile_options    (${MYCMAKE_SHARED}   PRIVATE     /Z7     /W4                               )
set_target_properties     (${MYCMAKE_SHARED}   PROPERTIES CLEAN_DIRECT_OUTPUT 1   OUTPUT_NAME libavcodec_shared  )

set                       (  MYCMAKE_APP                   mycmake_app                                   )
add_executable            (${MYCMAKE_APP}                  ${MYCMAKE_HDR_LIST}    ${MYCMAKE_SRC_LIST}    )
target_compile_definitions(${MYCMAKE_APP}      PRIVATE     UNICODE;_UNICODE       _CRT_SECURE_NO_WARNINGS)
target_include_directories(${MYCMAKE_APP}      PUBLIC      ./src                                         )
target_link_directories   (${MYCMAKE_APP}      PUBLIC      ./lib                                         )
target_link_libraries     (${MYCMAKE_APP}                  ${LIBAVCODEC_STATIC}                          )
target_compile_options    (${MYCMAKE_APP}      PRIVATE     /Z7     /W4                                   )
set_target_properties     (${MYCMAKE_APP}      PROPERTIES  CLEAN_DIRECT_OUTPUT 1  OUTPUT_NAME "mycmake"  )

SET(install_targets ${MYCMAKE_STATIC} ${MYCMAKE_SHARED} ${MYCMAKE_APP} )
SET(install_hdr_dir include        )

INSTALL(TARGETS ${install_targets}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION ${install_hdr_dir}
    )

install(FILES        ${MYCMAKE_HDRS_LIST}    DESTINATION    ${install_hdr_dir} )
install(DIRECTORY    "inc_sub_dir"           DESTINATION    ${install_hdr_dir} FILES_MATCHING PATTERN "*.h")

