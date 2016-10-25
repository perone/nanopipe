cmake_minimum_required(VERSION 2.8.4)
project(nanopipe)

set(NANOPIPE_VERSION_MAJOR 0)
set(NANOPIPE_VERSION_MINOR 1)
set(NANOPIPE_VERSION_PATCH 0)
set(NANOPIPE_VERSION_STRING ${NANOPIPE_VERSION_MAJOR}.${NANOPIPE_VERSION_MINOR}.${NANOPIPE_VERSION_PATCH})

# set(CMAKE_VERBOSE_MAKEFILE ON)

# Use Release if no configuration specified
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# ---------------------------------------------------------
# Check for required dependencies
# ---------------------------------------------------------
if(CMAKE_VERSION VERSION_GREATER 2.8.5 OR CMAKE_VERSION VERSION_EQUAL 2.8.5)
  include(GNUInstallDirs)
endif()

find_package(Threads REQUIRED)
find_package(hiredis REQUIRED)
find_package(libuv REQUIRED)
find_package(Mosquitto REQUIRED)
find_package(AMQP REQUIRED)
find_package(uWS REQUIRED)
find_package(Doxygen)
find_package(SWIG)
find_package(PythonLibs)

set(NANOPIPE_LIB_INCLUDES
  ${HIREDIS_INCLUDE_DIRS}
  ${LIBUV_INCLUDE_DIRS}
  ${MOSQUITTO_INCLUDE_DIR}
  ${AMQP_INCLUDE_DIR}
  ${UWS_INCLUDE_DIR})

set(NANOPIPE_LIB_DEPS
  ${HIREDIS_LIBRARIES}
  ${LIBUV_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${MOSQUITTO_LIBRARIES}
  ${AMQP_LIBRARIES}
  ${UWS_LIBRARIES})

# ---------------------------------------------------------
# Options
# ---------------------------------------------------------
option(lib "Build Nanopipe as a dynamic library." ON)
option(static_lib "Build Nanopipe as a static library." ON)
option(examples "Build all examples." ON)
option(python_bindings "Build Python bindings." ON)
option(documentation "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

# ---------------------------------------------------------
# Sources
# ---------------------------------------------------------

add_definitions(-DELPP_THREAD_SAFE)

set(DOC_NANOPIPE_DIR ${PROJECT_SOURCE_DIR}/docs)
set(SRC_NANOPIPE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INC_NANOPIPE_DIR ${PROJECT_SOURCE_DIR}/include)

file(GLOB INC_NANOPIPE_CORE ${INC_NANOPIPE_DIR}/nanopipe/*.hpp ${INC_NANOPIPE_DIR}/nanopipe/*.h)
file(GLOB SRC_NANOPIPE_CORE ${SRC_NANOPIPE_DIR}/*.cpp)

set(SRC_NANOPIPE_ALL ${SRC_NANOPIPE_CORE})
set(INC_NANOPIPE_ALL ${INC_NANOPIPE_CORE})

include_directories(${NANOPIPE_LIB_INCLUDES})
include_directories(${INC_NANOPIPE_DIR})
include_directories(${INC_NANOPIPE_DIR}/nanopipe)

# ---------------------------------------------------------
# Configure file
# ---------------------------------------------------------
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
    "${INC_NANOPIPE_DIR}/nanopipe/nanopipe_version.hpp.in"
    "${INC_NANOPIPE_DIR}/nanopipe/nanopipe_version.hpp"
)

# ---------------------------------------------------------
# Documentation
# ---------------------------------------------------------
if(documentation)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${DOC_NANOPIPE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile})

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()



# ---------------------------------------------------------
# Library generation
# ---------------------------------------------------------

if (lib)

  add_library(nanopipe SHARED ${SRC_NANOPIPE_ALL} ${INC_NANOPIPE_CORE})
  target_link_libraries(nanopipe ${NANOPIPE_LIB_DEPS})

  set_target_properties(nanopipe
    PROPERTIES VERSION ${NANOPIPE_VERSION_STRING}
    SOVERSION ${NANOPIPE_VERSION_MAJOR})

endif()

if (static_lib)

  add_library(nanopipe_static STATIC ${SRC_NANOPIPE_ALL})
  target_link_libraries(nanopipe_static ${NANOPIPE_LIB_DEPS})

  set_target_properties(nanopipe_static
    PROPERTIES VERSION ${NANOPIPE_VERSION_STRING}
    SOVERSION ${NANOPIPE_VERSION_MAJOR})

endif()

# ---------------------------------------------------------
# Examples
# ---------------------------------------------------------
if (examples)

  add_executable(basic examples/main.cpp)
  target_link_libraries(basic nanopipe)

  add_executable(mqtt_to_redis examples/mqtt_to_redis.cpp)
  target_link_libraries(mqtt_to_redis nanopipe)

  add_executable(amqp_to_mqtt examples/amqp_to_mqtt.cpp)
  target_link_libraries(amqp_to_mqtt nanopipe)


  add_custom_target(examples)
  add_dependencies(examples
    basic
    mqtt_to_redis
    amqp_to_mqtt
  )

endif()

# ---------------------------------------------------------
# Python Bindings
# ---------------------------------------------------------
if (python_bindings)

  include(${SWIG_USE_FILE})
  include_directories(${PYTHON_INCLUDE_PATH})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  set(CMAKE_SWIG_FLAGS "-c++")

  set_source_files_properties(python/nanopipe.i PROPERTIES CPLUSPLUS ON)
  swig_add_module(nanopipe_mod python python/nanopipe.i)

  swig_link_libraries(nanopipe_mod ${PYTHON_LIBRARIES} nanopipe)

endif()

# ---------------------------------------------------------
# Install (sudo make install)
# ---------------------------------------------------------

if(NOT CMAKE_INSTALL_PREFIX)
   set(CMAKE_INSTALL_PREFIX /usr/)
endif(NOT CMAKE_INSTALL_PREFIX)

# Check if it is a 64 bit system
if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT LIB_SUFFIX)
    set(LIB_SUFFIX "64")
endif()

if (lib)
  # Install the dynamic library to /usr/lib[64]
  install(TARGETS nanopipe DESTINATION lib${LIB_SUFFIX})
endif()

if (static_lib)
  # Install the static library to /usr/lib[64]
  install(TARGETS nanopipe_static DESTINATION lib${LIB_SUFFIX})
endif()

# Install the headers into /usr/include/NANOPIPE
install(FILES ${INC_NANOPIPE_CORE} DESTINATION include/nanopipe)

# Install the top-level header directly into /usr/include
install(FILES ${INC_NANOPIPE_WRAPPER} DESTINATION include)

# ---------------------------------------------------------
# Create system package (make package)
# ---------------------------------------------------------

# Determine build architecture
execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message("Building for ${ARCHITECTURE}")

# build CPack driven installer packages
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nanopipe - Connecting babel of services")
set(CPACK_PACKAGE_VENDOR "Christian S. perone")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")
set(CPACK_PACKAGE_VERSION_MAJOR "${NANOPIPE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${NANOPIPE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${NANOPIPE_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_RELEASE "1") # Increase this if a failed build was published
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CPACK_PACKAGE_VERSION_RELEASE}")
set(CPACK_SYSTEM_NAME "${ARCHITECTURE}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}.${ARCHITECTURE}")
# Debian specific fields
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Christian S. Perone <christian.perone@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${ARCHITECTURE}")
# RPM specific fields
set(CPACK_RPM_PACKAGE_ARCHITECTURE "${ARCHITECTURE}")
# Select CPack generators
set(CPACK_GENERATOR "DEB" "RPM")
include(CPack)