# Install script for directory: C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Hand")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.0.0-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-5.0" TYPE FILE FILES
    "C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/assimp-config.cmake"
    "C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/assimp-config-version.cmake"
    "C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/assimpTargets.cmake"
    "C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/assimpTargets-debug.cmake"
    "C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/assimpTargets-release.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.0.0-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/assimp.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/contrib/zlib/cmake_install.cmake")
  include("C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/contrib/cmake_install.cmake")
  include("C:/Users/xzz/Documents/pku/2023spring/CG/hw1/hand-graphics-homework-main/third_party/assimp/code/cmake_install.cmake")

endif()

