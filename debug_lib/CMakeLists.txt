project(debug_lib)

set(mapsforgevtbuilder_SRC_DIR "${PROJECT_SOURCE_DIR}/src/debug_lib")
set(mapsforgevtbuilder_LIBS_DIR "${PROJECT_SOURCE_DIR}/../../libs-external")

file(GLOB mapsforgevtbuilder_SRC_FILES "${debug_lib_SRC_DIR}/*.cpp" "${debug_lib_SRC_DIR}/*.h")

set(mapsforgevtbuilder_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src" PARENT_SCOPE)

if(SINGLE_LIBRARY)
  set(debug_lib_SRC_FILES ${debug_lib_SRC_FILES} PARENT_SCOPE)
  add_library(debug_lib INTERFACE)
else()
  include_directories(
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/../mapnikvt/src"
    "${debug_lib_LIBS_DIR}/pbf"
    "${debug_lib_LIBS_DIR}/cglib"
    "${debug_lib_LIBS_DIR}/stdext"
  )
  add_library(debug_lib OBJECT ${debug_lib_SRC_FILES})
endif()