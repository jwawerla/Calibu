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
find_package( CeresSolver 1.8.0 REQUIRED )
include_directories( ${CeresSolver_INCLUDE_DIRS} )

# Check that OPENCV is available
set(NO_OPENCV_GPU TRUE)
find_package( OpenCV QUIET )

# This depends on HAL, which in turns depends on Calibu, so we disable this by default
option( BUILD_CALIBGRID_HAL "To build or not build calibgrid-hal" ON )

if(BUILD_CALIBGRID_HAL AND Pangolin_FOUND AND CeresSolver_FOUND AND HAL_FOUND )
    add_executable( calibgrid-hal main.cpp )
    target_link_libraries( calibgrid-hal ${CeresSolver_LIBRARIES} )
    target_link_libraries( calibgrid-hal ${Pangolin_LIBRARIES} )
    target_link_libraries( calibgrid-hal ${Calibu_LIBRARIES} )
    target_link_libraries( calibgrid-hal ${HAL_LIBRARIES} )
    target_link_libraries( calibgrid-hal ${CVARS_LIBRARY} )
    target_link_libraries( calibgrid-hal ${PbMsgs_LIBRARY} )
    target_link_libraries( calibgrid-hal ${OpenCV_LIBRARIES} )

    install(TARGETS calibgrid-hal
        EXPORT CalibuTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    )
else()
  message( STATUS "found HAL      ${HAL_FOUND}" )
  message( STATUS "found Ceres    ${CeresSolver_FOUND}" )
  message( STATUS "found Pangolin ${Pangolin_FOUND}" )
endif()
