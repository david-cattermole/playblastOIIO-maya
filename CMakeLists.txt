cmake_minimum_required(VERSION 2.6)
project(playblastOIIO-maya)
set(PLUGIN_NAME playblastOIIO)
set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BUILD_TYPE RELEASE)

# Maya SDK
set(MAYA_ROOT "/usr/autodesk/maya2016")
set(MAYA_INCLUDE ${MAYA_ROOT}/include)
set(MAYA_LIB ${MAYA_ROOT}/lib)

# OpenImageIO
set(OIIO_INCLUDE "/usr/local/include")
set(OIIO_LIB "/usr/local/lib64")

## Utilities
#set(UTILS_INCLUDE "/home/user/dev/cppTests/utilities")

# C++ Flags needed for Maya
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBits64_ -m64 -DUNIX -D_BOOL -DLINUX -DFUNCPROTO -D_GNU_SOURCE -DLINUX_64 -fPIC -fno-strict-aliasing -DREQUIRE_IOSTREAM -O3 -Wall -Wno-multichar -Wno-comment -Wno-sign-compare -funsigned-char -pthread")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -Wno-deprecated -Wno-reorder -ftemplate-depth-25 -fno-gnu-keywords")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--version-script=${CMAKE_SOURCE_DIR}/plugin.map  -Wl,--strip-all")

set(SOURCE_FILES
        include/playblastOIIO.h
        src/playblastOIIO.cpp
        src/playblastOIIOMain.cpp)

include_directories(
        include
        # ${UTILS_INCLUDE}
        ${MAYA_INCLUDE}
        $(OIIO_INCLUDE)
)

link_directories(
        ${LIBRARY_OUTPUT_PATH}
        ${MAYA_LIB}
        ${OIIO_LIB}
)

add_library(${PLUGIN_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PLUGIN_NAME}
        OpenMaya
        OpenMayaUI
        OpenMayaAnim
        OpenMayaRender
        Foundation
        GL
        OpenImageIO
        z
        m
        )
set_target_properties(${PLUGIN_NAME} PROPERTIES
        PREFIX "" # no 'lib' prefix to .so files
        )