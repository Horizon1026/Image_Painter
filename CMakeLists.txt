cmake_minimum_required( VERSION 3.0 )

project( sensor_model )

set( CMAKE_BUILD_TYPE "Debug" )
set( CMAKE_CXX_FLAGS "-std=c++17 -O3 -Wall" )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set( CMAKE_CXX_STANDARD 20 )

# Add image painter.
if ( NOT TARGET lib_image_painter )
    add_subdirectory( src ${PROJECT_SOURCE_DIR}/build/lib_image_painter )
endif()

# Add visualizor.
set( VISUALIZOR_PATH ${PROJECT_SOURCE_DIR}/../Visualizor2D )
if ( NOT TARGET lib_2d_visualizor )
    add_subdirectory( ${VISUALIZOR_PATH}/src ${PROJECT_SOURCE_DIR}/build/lib_2d_visualizor )
endif()

# Create executable target to test image painter.
add_executable( test_image_painter
    test/test_image_painter.cpp
)
target_link_libraries( test_image_painter
    lib_image_painter
    lib_2d_visualizor
)
