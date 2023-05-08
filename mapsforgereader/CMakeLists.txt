project(mapsforgereader)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")


set(mapsforgereader_SRC_DIR "${PROJECT_SOURCE_DIR}/src/mapsforgereader")
set(mapsforgereader_LIBS_DIR "${PROJECT_SOURCE_DIR}/../../libs-external")

file(GLOB mapsforgereader_SRC_FILES "${mapsforgereader_SRC_DIR}/*.cpp" "${mapsforgereader_SRC_DIR}/*.h")

set(mapsforgereader_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src" PARENT_SCOPE)

if(SINGLE_LIBRARY)
  set(mapsforgereader_SRC_FILES ${mapsforgereader_SRC_FILES} PARENT_SCOPE)
  add_library(mapsforgereader INTERFACE)
else()
  include_directories(
    "${PROJECT_SOURCE_DIR}/src/"
    "${mapsforgereader_SRC_DIR}/utils/"
    "${mapsforgereader_SRC_DIR}/model/"
    "${mapsforgereader_SRC_DIR}/header/"
    "${PROJECT_SOURCE_DIR}/../mapnikvt/src/"
    "${PROJECT_SOURCE_DIR}/../vt/src"
    "${mapsforgereader_LIBS_DIR}/cglib"
    "${mapsforgereader_LIBS_DIR}/stdext"
    "${mapsforgereader_LIBS_DIR}/tinyformat"
    "${mapsforgereader_LIBS_DIR}/boost"
  )
  add_library(mapsforgereader OBJECT ${mapsforgereader_SRC_FILES})
endif()