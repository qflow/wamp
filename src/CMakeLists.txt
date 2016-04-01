cmake_minimum_required(VERSION 2.8.11)
project(wamp)
add_definitions(-DWAMP_LIBRARY -DQT_NO_KEYWORDS -DSECRET_WITH_UNSTABLE)
find_package(LibSecret REQUIRED)
find_package(MsgPack REQUIRED)
aux_source_directory(. SRC_LIST)
#aux_source_directory(./client SRC_LIST)
#aux_source_directory(./router SRC_LIST)
include_directories(./client ./router ${MSGPACK_INCLUDE_DIRS} ${LIBSECRET_INCLUDE_DIRS})
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(QT_MIN_VERSION "5.6.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Qml)
#find_package(MsgPack REQUIRED)

add_library(wamp SHARED ${SRC_LIST} qmldir)
set_property(TARGET wamp PROPERTY CXX_STANDARD 14)
add_dependencies(wamp core websockets)

target_link_libraries(wamp core Qt5::Core Qt5::Qml)
set(WAMP_INSTALL_PATH "bin/plugins/wamp")
install (TARGETS wamp DESTINATION ${WAMP_INSTALL_PATH})
install (FILES qmldir DESTINATION ${WAMP_INSTALL_PATH})
