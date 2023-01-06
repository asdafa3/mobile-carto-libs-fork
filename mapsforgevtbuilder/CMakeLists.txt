project(mapsforgevtbuilder)

set(mapsforgevtbuilder_SRC_DIR "${PROJECT_SOURCE_DIR}/src/mapsforgevtbuilder")
set(mapsforgevtbuilder_LIBS_DIR "${PROJECT_SOURCE_DIR}/../../libs-external")

file(GLOB mapsforgevtbuilder_SRC_FILES "${mapsforgevtbuilder_SRC_DIR}/*.cpp" "${mapsforgevtbuilder_SRC_DIR}/*.h")

set(mapsforgevtbuilder_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src" PARENT_SCOPE)

if(SINGLE_LIBRARY)
  set(mapsforgevtbuilder_SRC_FILES ${mapsforgevtbuilder_SRC_FILES} PARENT_SCOPE)
  add_library(mapsforgevtbuilder INTERFACE)
else()
  include_directories(
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/../mapnikvt/src"
    "${mapsforgevtbuilder_LIBS_DIR}/pbf"
    "${mapsforgevtbuilder_LIBS_DIR}/cglib"
    "${mapsforgevtbuilder_LIBS_DIR}/stdext"
  )
  add_library(mapsforgevtbuilder OBJECT ${mapsforgevtbuilder_SRC_FILES})
endif()