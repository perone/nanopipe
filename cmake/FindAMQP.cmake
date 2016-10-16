# - Find AMQP-CPP
# Find the native AMQP-CPP includes and libraries
#
#  AMQP_INCLUDE_DIR - where to find amqpcpp.h, etc.
#  AMQP_LIBRARIES   - List of libraries when using AMQP-CPP.
#  AMQP_FOUND       - True if AMQP-CPP found.

if(AMQP_INCLUDE_DIR)
    # Already in cache, be silent
    set(AMQP_FIND_QUIETLY TRUE)
endif(AMQP_INCLUDE_DIR)

find_path(AMQP_INCLUDE_DIR amqpcpp.h)

find_library(AMQP_LIBRARY NAMES libamqpcpp amqpcpp)

# Handle the QUIETLY and REQUIRED arguments and set AMQP_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AMQP DEFAULT_MSG AMQP_LIBRARY AMQP_INCLUDE_DIR)

if(AMQP_FOUND)
  set(AMQP_LIBRARIES ${AMQP_LIBRARY})
else(AMQP_FOUND)
  set(AMQP_LIBRARIES)
endif(AMQP_FOUND)

mark_as_advanced(AMQP_INCLUDE_DIR AMQP_LIBRARY)