#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
SET(CMAKE_IMPORT_FILE_VERSION 1)

# Compute the installation prefix relative to this file.
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "dynamorio" for configuration "Debug"
SET_PROPERTY(TARGET dynamorio APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(dynamorio PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib32/debug/libdynamorio.so.4.2"
  IMPORTED_SONAME_DEBUG "libdynamorio.so.4.2"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS dynamorio )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_dynamorio "${_IMPORT_PREFIX}/lib32/debug/libdynamorio.so.4.2" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drinjectlib" for configuration "Debug"
SET_PROPERTY(TARGET drinjectlib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drinjectlib PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "drdecode"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib32/libdrinjectlib.so"
  IMPORTED_SONAME_DEBUG "libdrinjectlib.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drinjectlib )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drinjectlib "${_IMPORT_PREFIX}/lib32/libdrinjectlib.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drdecode" for configuration "Debug"
SET_PROPERTY(TARGET drdecode APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drdecode PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "ASM;C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib32/debug/libdrdecode.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drdecode )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drdecode "${_IMPORT_PREFIX}/lib32/debug/libdrdecode.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drconfiglib" for configuration "Debug"
SET_PROPERTY(TARGET drconfiglib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drconfiglib PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib32/libdrconfiglib.so"
  IMPORTED_SONAME_DEBUG "libdrconfiglib.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drconfiglib )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drconfiglib "${_IMPORT_PREFIX}/lib32/libdrconfiglib.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drcontainers" for configuration "Debug"
SET_PROPERTY(TARGET drcontainers APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drcontainers PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrcontainers.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drcontainers )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drcontainers "${_IMPORT_PREFIX}/ext/lib32/debug/libdrcontainers.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drutil" for configuration "Debug"
SET_PROPERTY(TARGET drutil APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drutil PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drmgr"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrutil.so"
  IMPORTED_SONAME_DEBUG "libdrutil.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drutil )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drutil "${_IMPORT_PREFIX}/ext/lib32/debug/libdrutil.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drutil_static" for configuration "Debug"
SET_PROPERTY(TARGET drutil_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drutil_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drmgr_static"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrutil_static.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drutil_static )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drutil_static "${_IMPORT_PREFIX}/ext/lib32/debug/libdrutil_static.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drgui" for configuration "Debug"
SET_PROPERTY(TARGET drgui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drgui PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/bin32/drgui"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drgui )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drgui "${_IMPORT_PREFIX}/ext/bin32/drgui" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drsyms" for configuration "Debug"
SET_PROPERTY(TARGET drsyms APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drsyms PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "dynamorio"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrsyms.so"
  IMPORTED_SONAME_DEBUG "libdrsyms.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drsyms )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drsyms "${_IMPORT_PREFIX}/ext/lib32/debug/libdrsyms.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drsyms_static" for configuration "Debug"
SET_PROPERTY(TARGET drsyms_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drsyms_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drcontainers;dwarf;elftc;elf"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrsyms_static.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drsyms_static )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drsyms_static "${_IMPORT_PREFIX}/ext/lib32/debug/libdrsyms_static.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drwrap" for configuration "Debug"
SET_PROPERTY(TARGET drwrap APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drwrap PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drmgr;drcontainers"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrwrap.so"
  IMPORTED_SONAME_DEBUG "libdrwrap.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drwrap )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drwrap "${_IMPORT_PREFIX}/ext/lib32/debug/libdrwrap.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drwrap_static" for configuration "Debug"
SET_PROPERTY(TARGET drwrap_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drwrap_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "ASM;C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drmgr_static;drcontainers"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrwrap_static.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drwrap_static )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drwrap_static "${_IMPORT_PREFIX}/ext/lib32/debug/libdrwrap_static.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drx" for configuration "Debug"
SET_PROPERTY(TARGET drx APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drx PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drcontainers;drmgr"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrx.so"
  IMPORTED_SONAME_DEBUG "libdrx.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drx )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drx "${_IMPORT_PREFIX}/ext/lib32/debug/libdrx.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drx_static" for configuration "Debug"
SET_PROPERTY(TARGET drx_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drx_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drcontainers;drmgr_static"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrx_static.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drx_static )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drx_static "${_IMPORT_PREFIX}/ext/lib32/debug/libdrx_static.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drmgr" for configuration "Debug"
SET_PROPERTY(TARGET drmgr APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drmgr PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrmgr.so"
  IMPORTED_SONAME_DEBUG "libdrmgr.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drmgr )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drmgr "${_IMPORT_PREFIX}/ext/lib32/debug/libdrmgr.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drmgr_static" for configuration "Debug"
SET_PROPERTY(TARGET drmgr_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drmgr_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/ext/lib32/debug/libdrmgr_static.a"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drmgr_static )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drmgr_static "${_IMPORT_PREFIX}/ext/lib32/debug/libdrmgr_static.a" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drltrace" for configuration "Debug"
SET_PROPERTY(TARGET drltrace APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drltrace PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drmgr;drwrap;drx"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/tools/lib32/debug/libdrltrace.so"
  IMPORTED_SONAME_DEBUG "libdrltrace.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drltrace )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drltrace "${_IMPORT_PREFIX}/tools/lib32/debug/libdrltrace.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drcov" for configuration "Debug"
SET_PROPERTY(TARGET drcov APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drcov PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "dynamorio;drmgr;drx;drcontainers"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/tools/lib32/debug/libdrcov.so"
  IMPORTED_SONAME_DEBUG "libdrcov.so"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drcov )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drcov "${_IMPORT_PREFIX}/tools/lib32/debug/libdrcov.so" )

# Make sure the targets which have been exported in some other 
# export set exist.

# Import target "drcov2lcov" for configuration "Debug"
SET_PROPERTY(TARGET drcov2lcov APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
SET_TARGET_PROPERTIES(drcov2lcov PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/tools/bin32/drcov2lcov"
  )

LIST(APPEND _IMPORT_CHECK_TARGETS drcov2lcov )
LIST(APPEND _IMPORT_CHECK_FILES_FOR_drcov2lcov "${_IMPORT_PREFIX}/tools/bin32/drcov2lcov" )

# Loop over all imported files and verify that they actually exist
FOREACH(target ${_IMPORT_CHECK_TARGETS} )
  FOREACH(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    IF(NOT EXISTS "${file}" )
      MESSAGE(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    ENDIF()
  ENDFOREACH()
  UNSET(_IMPORT_CHECK_FILES_FOR_${target})
ENDFOREACH()
UNSET(_IMPORT_CHECK_TARGETS)

# Cleanup temporary variables.
SET(_IMPORT_PREFIX)

# Commands beyond this point should not need to know the version.
SET(CMAKE_IMPORT_FILE_VERSION)
