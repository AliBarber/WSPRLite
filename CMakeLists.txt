cmake_minimum_required(VERSION 3.26)
project(
  "WSPRLite"
  VERSION 1.1
  LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

## The library
add_library(WSPRLite INTERFACE)

target_compile_features(WSPRLite INTERFACE cxx_std_11)

target_include_directories(
  WSPRLite
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(TARGETS WSPRLite
        EXPORT WSPRLite
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)


install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/wsprlite DESTINATION include)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
				DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
