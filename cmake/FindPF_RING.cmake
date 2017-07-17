# - Try to find pf_ring includes.
#
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  PF_RING_ROOT_DIR          Set this variable to the root installation of
#                            pf_ring if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  PF_RING_FOUND             System has pf_ring API files.
#  PF_RING_INCLUDE_DIR       The pf_ring include directory.
#  PF_RING_LIBRARY           The libpfring library

find_path(PF_RING_ROOT_DIR
    NAMES include/pfring.h
)

find_path(PF_RING_INCLUDE_DIR
    NAMES pfring.h
    HINTS ${PF_RING_ROOT_DIR}/userland/lib ${PF_RING_ROOT_DIR}/include
)

find_library(PF_RING_LIBRARY
    NAMES pfring
    HINTS ${PF_RING_ROOT_DIR}/userland/lib ${PF_RING_ROOT_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PF_RING DEFAULT_MSG
    PF_RING_LIBRARY
    PF_RING_INCLUDE_DIR
)

mark_as_advanced(
    PF_RING_ROOT_DIR
    PF_RING_INCLUDE_DIR
    PF_RING_LIBRARY
)

