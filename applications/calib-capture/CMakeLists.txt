#https://github.com/gwu-robotics/Calibu
find_package( Calibu 0.1 REQUIRED )
include_directories( ${Calibu_INCLUDE_DIRS} )

find_package( PbMsgs )

# git://github.com/stevenlovegrove/Pangolin.git
find_package( Pangolin 0.1 QUIET )
include_directories( ${Pangolin_INCLUDE_DIRS} )

#https://github.com/gwu-robotics/HAL
find_package( HAL 0.1 QUIET )
include_directories( ${HAL_INCLUDE_DIRS} )

# git clone https://ceres-solver.googlesource.com/ceres-solver
list(APPEND SEARCH_HEADERS ${EIGEN3_INCLUDE_DIR}) # Help Ceres find Eigen

# Check that OPENCV is available
set(NO_OPENCV_GPU TRUE)
find_package( OpenCV QUIET )

# This depends on HAL, which in turns depends on Calibu, so we disable this by default
option( BUILD_CALIB_CAPTURE "To build or not build calib-capture" ON )

if(BUILD_CALIBGRID_HAL AND Pangolin_FOUND AND HAL_FOUND )
    add_executable( calib-capture 
      main.cpp
    )
    target_link_libraries( calib-capture ${Pangolin_LIBRARIES} )
    target_link_libraries( calib-capture ${Calibu_LIBRARIES} )
    target_link_libraries( calib-capture ${HAL_LIBRARIES} )
    target_link_libraries( calib-capture ${CVARS_LIBRARY} )
    target_link_libraries( calib-capture ${PbMsgs_LIBRARY} )
    target_link_libraries( calib-capture ${OpenCV_LIBRARIES} )

    install(TARGETS calib-capture
        EXPORT CalibuTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    )
else()
  message( STATUS "found HAL      ${HAL_FOUND}" )
  message( STATUS "found Ceres    ${CeresSolver_FOUND}" )
  message( STATUS "found Pangolin ${Pangolin_FOUND}" )
endif()
