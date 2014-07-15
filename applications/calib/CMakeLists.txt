#https://github.com/gwu-robotics/Calibu
find_package( Calibu 0.1 REQUIRED )
include_directories( ${Calibu_INCLUDE_DIRS} )

# git://github.com/stevenlovegrove/Pangolin.git
find_package( Pangolin 0.1 QUIET )
include_directories( ${Pangolin_INCLUDE_DIRS} )

# git clone https://ceres-solver.googlesource.com/ceres-solver
list(APPEND SEARCH_HEADERS ${EIGEN3_INCLUDE_DIR}) # Help Ceres find Eigen
find_package( CeresSolver 1.8.0 QUIET )
include_directories( ${CeresSolver_INCLUDE_DIRS} )

find_package( GLog REQUIRED )
include_directories( ${GLog_INCLUDE_DIRS} )

# Check that OPENCV is available
set(NO_OPENCV_GPU TRUE)
find_package( OpenCV QUIET )

option(BUILD_CALIBGRID "Toggle build calibgrid." ON )
if( Pangolin_FOUND AND CeresSolver_FOUND AND OpenCV_FOUND AND BUILD_CALIBGRID)
    add_executable( calibgrid main.cpp )
    target_link_libraries( calibgrid ${CeresSolver_LIBRARIES} )
    target_link_libraries( calibgrid ${Pangolin_LIBRARIES} )
    target_link_libraries( calibgrid ${Calibu_LIBRARIES} )
    target_link_libraries( calibgrid ${CVARS_LIBRARY} )
    target_link_libraries( calibgrid ${GLog_LIBRARIES} )

    install(TARGETS calibgrid
        EXPORT CalibuTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    )
endif()
