cmake_minimum_required( VERSION 2.8 )

include( ExternalProject )

externalproject_add( miniglog
  PREFIX sources
  GIT_REPOSITORY https://github.com/arpg/miniglog.git
  GIT_TAG master 
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/local -DCMAKE_BUILD_TYPE=release
  BUILD_COMMAND make -j4
  INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local
)

externalproject_add_step( miniglog gitpull
  COMMAND echo "checking for updates"
  ALWAYS 1
  DEPENDERS update
)


externalproject_add( sophus
  PREFIX sources
  GIT_REPOSITORY https://github.com/strasdat/Sophus.git
  GIT_TAG master 
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/local -DCMAKE_BUILD_TYPE=release
  BUILD_COMMAND make -j4
  INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local
)

externalproject_add_step( sophus gitpull
  COMMAND echo "checking for updates"
  ALWAYS 1
  DEPENDERS update
)

externalproject_add( pangolin
  PREFIX sources
  GIT_REPOSITORY https://github.com/arpg/Pangolin.git
  GIT_TAG master 
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/local -DCMAKE_BUILD_TYPE=release
  BUILD_COMMAND make -j4
  INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local
)

externalproject_add_step( pangolin gitpull
  COMMAND echo "checking for updates"
  ALWAYS 1
  DEPENDERS update
)

externalproject_add( ceres
  PREFIX sources
  GIT_REPOSITORY https://ceres-solver.googlesource.com/ceres-solver
  GIT_TAG 1.8.0 
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/local -DCMAKE_BUILD_TYPE=release -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF
  BUILD_COMMAND make -j4
  INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local
)

externalproject_add_step( ceres gitpull
  COMMAND echo "checking for updates"
  ALWAYS 1
  DEPENDERS update
)

externalproject_add( glconsole
  PREFIX sources
  GIT_REPOSITORY https://github.com/arpg/GLConsole.git
  GIT_TAG master 
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/local -DCMAKE_BUILD_TYPE=release
  BUILD_COMMAND make -j4
  INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local
)

externalproject_add_step( glconsole gitpull
  COMMAND echo "checking for updates"
  ALWAYS 1
  DEPENDERS update
)

# there is a circular dependency between HAL and some of the applications
# in Calibu. So we build HAL only after Calibu was build the first time and thus
# CMake is able to find it.
find_package( Calibu QUIET )
if ( Calibu_FOUND )

  externalproject_add( hal
    DEPENDS miniglog sophus
    PREFIX sources
    GIT_REPOSITORY https://github.com/arpg/HAL.git
    GIT_TAG master 
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/local -DCMAKE_BUILD_TYPE=release -DBUILD_APPLICATIONS=OFF
    BUILD_COMMAND make -j4
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/local
  )

  externalproject_add_step( hal gitpull
    COMMAND echo "checking for updates"
    ALWAYS 1
    DEPENDERS update
  )
endif( Calibu_FOUND )
