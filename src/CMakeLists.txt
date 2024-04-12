aux_source_directory( . AUX_SRC_IMAGE_PAINTER )

# Add all components of slam utility.
set( SLAM_UTILITY_PATH ${PROJECT_SOURCE_DIR}/../Slam_Utility/src )
if ( NOT TARGET lib_slam_utility_datatype )
    add_subdirectory( ${SLAM_UTILITY_PATH}/datatype ${PROJECT_SOURCE_DIR}/build/lib_slam_utility_datatype )
endif()
if ( NOT TARGET lib_slam_utility_math )
    add_subdirectory( ${SLAM_UTILITY_PATH}/math ${PROJECT_SOURCE_DIR}/build/lib_slam_utility_math )
endif()
if ( NOT TARGET lib_slam_utility_operate )
    add_subdirectory( ${SLAM_UTILITY_PATH}/operate ${PROJECT_SOURCE_DIR}/build/lib_slam_utility_operate )
endif()
if ( NOT TARGET lib_slam_utility_log )
    add_subdirectory( ${SLAM_UTILITY_PATH}/log ${PROJECT_SOURCE_DIR}/build/lib_slam_utility_log )
endif()

# Add dependence for painter.
if ( NOT TARGET lib_assic_fonts )
    add_subdirectory( fonts ${PROJECT_SOURCE_DIR}/build/lib_assic_fonts )
endif()

# Create library.
add_library( lib_image_painter ${AUX_SRC_IMAGE_PAINTER} )
target_include_directories( lib_image_painter PUBLIC
    .
    fonts
)
target_link_libraries( lib_image_painter
    lib_slam_utility_datatype
    lib_slam_utility_operate
    lib_slam_utility_math
    lib_slam_utility_log

    lib_assic_fonts
)
