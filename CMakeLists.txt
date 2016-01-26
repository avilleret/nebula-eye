project(nebula-eye)
set(APP nebula-eye)

cmake_minimum_required(VERSION 2.8)

include(../of/openFrameworks.cmake)
find_package( OpenCV REQUIRED )

ofxaddon(ofxGui)
ofxaddon(ofxOsc)
ofxaddon(ofxCv)
# this addons are needed if NAT transversal is enabled in ofGstRTPConstants.h
ofxaddon(ofxGStreamer)
ofxaddon(ofxGstRTP)
ofxaddon(ofxDepthStreamCompression)
ofxaddon(ofxSnappy)

include_directories (
 ${CMAKE_CURRENT_LIST_DIR}/ofxCv/libs/ofxCv/include/
)

set(SOURCES
    src/main.cpp
    src/nebulaEye.cpp
)

add_executable(
    ${APP}
    MACOSX_BUNDLE
    ${SOURCES}
    ${OFXADDONS_SOURCES}
)

target_link_libraries(
    ${APP}
    ${OPENFRAMEWORKS_LIBRARIES}
    ${OpenCV_LIBS}
)

set_target_properties(
    ${APP} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST
    "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
)

cotire(${APP})
