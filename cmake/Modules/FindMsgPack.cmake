find_path(MSGPACK_INCLUDE_DIR msgpack.hpp
    /usr/include
    /usr/include/msgpack
    /usr/local/include
    /usr/local/include/msgpack
    ~/workspace/msgpack-c/include
    ~/msgpack-c/include
)
set(MSGPACK_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MsgPack REQUIRED_VARS MSGPACK_INCLUDE_DIR)
