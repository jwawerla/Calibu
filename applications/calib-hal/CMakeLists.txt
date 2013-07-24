find_package( Calibu 0.1 REQUIRED )
include_directories( ${Calibu_INCLUDE_DIRS} )
link_libraries( ${LIBRARY_NAME} )

find_package( Pangolin 0.1 REQUIRED )
include_directories( ${Pangolin_INCLUDE_DIRS} )
link_libraries( ${Pangolin_LIBRARIES} )

find_package( HAL 0.1 REQUIRED )
include_directories( ${HAL_INCLUDE_DIRS} )
link_libraries( ${HAL_LIBRARIES} )

# Make sure Ceres knows where to find Eigen
list( APPEND SEARCH_HEADERS ${EIGEN3_INCLUDE_DIR} )

# git clone https://ceres-solver.googlesource.com/ceres-solver
find_package( Ceres 1.6.0 REQUIRED )
include_directories( ${CERES_INCLUDES} )
link_libraries( ${CERES_LIBRARIES} )

add_executable( calibgrid-hal main.cpp )

install(TARGETS calibgrid-hal
  EXPORT CalibuTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
